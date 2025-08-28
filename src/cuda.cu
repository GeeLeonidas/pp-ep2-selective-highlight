// SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
//
// SPDX-License-Identifier: AGPL-3.0-only

#include "filter.h"
#include "ppm.h"
#include <stddef.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// PPM SECTION

#define ASSERT(expr, msg, exit_label)                                          \
  if (!(expr)) {                                                               \
    puts(msg);                                                                 \
    goto exit_label;                                                           \
  }
#define MAX_LINE 4096

PpmImage *read_ppm_image(FILE *source_file) {
  PpmImage *image = NULL;
  size_t image_size = 0;
  ASSERT(source_file != NULL, "Source file is NULL", read_ppm_image_error);
  image = (PpmImage*)malloc(sizeof(PpmImage));
  char header[2];
  ASSERT(fscanf(source_file, "%c%c", &header[0], &header[1]),
         "Error reading the file header", read_ppm_image_error);
  ASSERT(header[0] == 'P' && header[1] == '3',
         "Unsupported format (expected `P3`)", read_ppm_image_error)
  char line[MAX_LINE];
  do {
    ASSERT(fgets(line, MAX_LINE, source_file),
           "Error reading line(s) after header", read_ppm_image_error);
  } while (line[0] == '#' || line[0] == '\n');
  ASSERT(sscanf(line, "%lu %lu", &image->width, &image->height),
         "Error reading `width` and `height` integers", read_ppm_image_error);
  ASSERT(fscanf(source_file, "%hu", &image->max_value),
         "Error reading `max_value` integer", read_ppm_image_error);
  image_size = image->width * image->height;
  image->color_values_write = (RgbTriplet*)malloc(image_size * sizeof(RgbTriplet));
  image->color_values_read = (RgbTriplet*)malloc(image_size * sizeof(RgbTriplet));
  image->needs_flushing = 0;
  for (size_t idx = 0; idx < image_size; idx++) {
    uint16_t red, green, blue;
    ASSERT(fscanf(source_file, "%hu %hu %hu", &red, &green, &blue),
           "Error reading `red`, `blue` and `green` integers",
           read_ppm_image_error);
    RgbTriplet rgb =
        (RgbTriplet){.r = ((float)red) / ((float)image->max_value),
                     .g = ((float)green) / ((float)image->max_value),
                     .b = ((float)blue) / ((float)image->max_value)};
    write_at_idx_ppm_image(image, idx, rgb);
  }
  ASSERT(flush_ppm_image(image), "Error flushing the image write buffer",
         read_ppm_image_error);
  return image;
read_ppm_image_error:
  free_ppm_image(&image);
  return NULL;
}

int write_at_idx_ppm_image(PpmImage *image, size_t idx, RgbTriplet rgb) {
  ASSERT(image != NULL, "PPM image is NULL", write_at_idx_ppm_image_error);
  ASSERT(image->color_values_write != NULL, "PPM image write buffer is NULL",
         write_at_idx_ppm_image_error);
  ASSERT(idx < (image->width * image->height),
         "Error writing at out of bounds index from PPM image",
         write_at_idx_ppm_image_error);
  image->color_values_write[idx] = rgb;
  image->needs_flushing = 1;
  return 1;
write_at_idx_ppm_image_error:
  return 0;
}

int write_at_xy_ppm_image(PpmImage *image, size_t x, size_t y, RgbTriplet rgb) {
  ASSERT(image != NULL, "PPM image is NULL", write_at_xy_ppm_image_error);
  return write_at_idx_ppm_image(image, x + y * image->width, rgb);
write_at_xy_ppm_image_error:
  return 0;
}

int flush_ppm_image(PpmImage *image) {
  ASSERT(image != NULL, "PPM image is NULL", flush_ppm_image_error);
  ASSERT(image->color_values_read != NULL, "PPM image read buffer is NULL",
         flush_ppm_image_error);
  ASSERT(image->color_values_write != NULL, "PPM image write buffer is NULL",
         flush_ppm_image_error);
  if (image->needs_flushing) {
    size_t image_size = image->width * image->height;
    for (size_t idx = 0; idx < image_size; idx++)
      image->color_values_read[idx] = image->color_values_write[idx];
    image->needs_flushing = 0;
  }
  return 1;
flush_ppm_image_error:
  return 0;
}

int read_at_idx_ppm_image(PpmImage *image, size_t idx, RgbTriplet *rgb) {
  ASSERT(image != NULL, "PPM image is NULL", read_at_idx_ppm_image_error);
  ASSERT(image->color_values_read != NULL, "PPM image read buffer is NULL",
         read_at_idx_ppm_image_error);
  ASSERT(rgb != NULL, "RgbTriplet is NULL", read_at_idx_ppm_image_error);
  ASSERT(idx < (image->width * image->height),
         "Error reading at out of bounds index from PPM image",
         read_at_idx_ppm_image_error);
  *rgb = image->color_values_read[idx];
  return 1;
read_at_idx_ppm_image_error:
  return 0;
}

int read_at_xy_ppm_image(PpmImage *image, size_t x, size_t y, RgbTriplet *rgb) {
  ASSERT(image != NULL, "PPM image is NULL", read_at_xy_ppm_image_error);
  return read_at_idx_ppm_image(image, x + y * image->width, rgb);
read_at_xy_ppm_image_error:
  return 0;
}

int save_ppm_image(PpmImage *image, FILE *output_file) {
  size_t image_size = 0;
  ASSERT(image != NULL, "PPM image is NULL", save_ppm_image_error);
  ASSERT(output_file != NULL, "Output file is NULL", save_ppm_image_error);
  ASSERT(fprintf(output_file, "P3\n"), "Error writing PPM image header",
         save_ppm_image_error);
  ASSERT(fprintf(output_file, "%lu %lu\n", image->width, image->height),
         "Error writing `width` and `height` integers", save_ppm_image_error);
  ASSERT(fprintf(output_file, "%hu\n", image->max_value),
         "Error writing `max_value` integer", save_ppm_image_error);
  image_size = image->width * image->height;
  for (size_t idx = 0; idx < image_size; idx++) {
    RgbTriplet rgb;
    ASSERT(read_at_idx_ppm_image(image, idx, &rgb),
           "Error reading at index from PPM image", save_ppm_image_error);
    uint16_t red, green, blue;
    red = (uint16_t)roundf(rgb.r * ((float)image->max_value));
    green = (uint16_t)roundf(rgb.g * ((float)image->max_value));
    blue = (uint16_t)roundf(rgb.b * ((float)image->max_value));
    ASSERT(fprintf(output_file, "%hu %hu %hu\n", red, green, blue),
           "Error writing `red`, `green` and `blue` integers",
           save_ppm_image_error);
  }
  return 1;
save_ppm_image_error:
  return 0;
}

void free_ppm_image(PpmImage **image) {
  if (image == NULL || *image == NULL)
    return;
  if ((*image)->color_values_write) {
    free((*image)->color_values_write);
    (*image)->color_values_write = NULL;
  }
  if ((*image)->color_values_read) {
    free((*image)->color_values_read);
    (*image)->color_values_read = NULL;
  }
  free(*image);
  *image = NULL;
}

// FILTER SECTION

__device__ void r_pixel(PpmImage *image, size_t m, size_t x, size_t y, size_t *radius) {
  RgbTriplet rgb;
  size_t idx = x + y * image->width;
  rgb = image->color_values_read[idx];
  float sum = rgb.r + rgb.g + rgb.b;
  *radius = (((size_t)(sum * 255)) % m) + 1;
}

__device__ void blur_at(PpmImage *image, size_t m, size_t x, size_t y, RgbTriplet *rgb) {
  size_t radius;
  r_pixel(image, m, x, y, &radius);
  float sum_r = 0.0f, sum_g = 0.0f, sum_b = 0.0f;
  for (size_t i = 0; i <= 2 * radius; i++) {
    for (size_t j = 0; j <= 2 * radius; j++) {
      size_t neighbour_x = x + i;
      if (neighbour_x < radius)
        neighbour_x = 0;
      else
        neighbour_x -= radius;
      size_t neighbour_y = y + j;
      if (neighbour_y < radius)
        neighbour_y = 0;
      else
        neighbour_y -= radius;
      if (neighbour_x >= image->width)
        neighbour_x = image->width - 1;
      if (neighbour_y >= image->height)
        neighbour_y = image->height - 1;
      size_t neighbour_idx = neighbour_x + neighbour_y * image->width;
      RgbTriplet neighbour_rgb;
      neighbour_rgb = image->color_values_read[neighbour_idx];
      sum_r += neighbour_rgb.r;
      sum_g += neighbour_rgb.g;
      sum_b += neighbour_rgb.b;
    }
  }
  float n = (float)((1 + radius * 2) * (1 + radius * 2));
  *rgb = (RgbTriplet){.r = sum_r / n, .g = sum_g / n, .b = sum_b / n};
}

__global__ void sharpen_kernel(PpmImage image, size_t image_size, float threshold, float sharpen_factor, size_t m) {
    size_t global_idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (global_idx >= image_size)
        return;
    size_t x = global_idx % image.width;
    size_t y = global_idx / image.width;
    RgbTriplet rgb, blur, new_rgb;
    rgb = image.color_values_read[global_idx];
    blur_at(&image, m, x, y, &blur);
    if (rgb.r <= threshold)
      new_rgb = blur;
    else
      new_rgb = (RgbTriplet){
          .r = rgb.r + sharpen_factor * (rgb.r - blur.r),
          .g = rgb.g + sharpen_factor * (rgb.g - blur.g),
          .b = rgb.b + sharpen_factor * (rgb.b - blur.b)};
    new_rgb.r = (new_rgb.r >= 1.0f) ? 1.0f : ((new_rgb.r <= 0.0f) ? 0.0f : new_rgb.r);
    new_rgb.g = (new_rgb.g >= 1.0f) ? 1.0f : ((new_rgb.g <= 0.0f) ? 0.0f : new_rgb.g);
    new_rgb.b = (new_rgb.b >= 1.0f) ? 1.0f : ((new_rgb.b <= 0.0f) ? 0.0f : new_rgb.b);
    image.color_values_write[global_idx] = new_rgb;
}

__global__ void grayscale_kernel(PpmImage image, size_t image_size) {
    size_t global_idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (global_idx >= image_size)
        return;
    RgbTriplet rgb;
    rgb = image.color_values_read[global_idx];
    float y = 0.299f * rgb.r + 0.587f * rgb.g + 0.114f * rgb.b;
    RgbTriplet grayscale_rgb = (RgbTriplet){.r = y, .g = y, .b = y};
    image.color_values_write[global_idx] = grayscale_rgb;
}

int filter_ppm_image(PpmImage *image, float threshold, float sharpen_factor,
                     size_t m, int threads_per_block) {
  int exit_code = 0;
  size_t image_size = image->width * image->height;
  PpmImage device_image = (PpmImage){
      .width = image->width,
      .height = image->height,
      .max_value = image->max_value,
      .color_values_write = NULL,
      .color_values_read = NULL,
      .needs_flushing = image->needs_flushing,
  };
  size_t blocks = image_size / threads_per_block;
  if (image_size % threads_per_block > 0)
    blocks++;
  ASSERT(image != NULL, "PPM image is null", filter_error);
  cudaMalloc(&device_image.color_values_read, image_size * sizeof(RgbTriplet));
  cudaMalloc(&device_image.color_values_write, image_size * sizeof(RgbTriplet));
  cudaMemcpy(device_image.color_values_read, image->color_values_read, image_size * sizeof(RgbTriplet), cudaMemcpyHostToDevice);
  sharpen_kernel<<<blocks, threads_per_block>>>(device_image, image_size, threshold, sharpen_factor, m);
  ASSERT(cudaGetLastError() == cudaSuccess, "Error while invoking CUDA kernel on the device", filter_error);
  cudaDeviceSynchronize();
  cudaMemcpy(device_image.color_values_read, device_image.color_values_write, image_size * sizeof(RgbTriplet), cudaMemcpyDeviceToDevice);
  grayscale_kernel<<<blocks, threads_per_block>>>(device_image, image_size);
  ASSERT(cudaGetLastError() == cudaSuccess, "Error while invoking CUDA kernel on the device", filter_error);
  cudaDeviceSynchronize();
  cudaMemcpy(image->color_values_read, device_image.color_values_write, image_size * sizeof(RgbTriplet), cudaMemcpyDeviceToHost);
  exit_code = 1;
filter_error:
  if (device_image.color_values_read != NULL)
    cudaFree(device_image.color_values_read);
  if (device_image.color_values_write != NULL)
    cudaFree(device_image.color_values_write);
  return exit_code;
}

// MAIN SECTION

int main(int argc, char **argv) {
  int exit_code = EXIT_FAILURE;
  PpmImage *image = NULL;
  FILE *output_file = NULL, *source_file = NULL;
  int threads_per_block = 1024;
  ASSERT(argc >= 6, "Missing arguments (min.: 5)", exit);
  // Reads the runtime parameters
  size_t m, raw_threshold;
  float sharpen_factor, threshold;
  ASSERT(sscanf(argv[3], "%lu", &m),
         "Error reading variable radius' `m` integer", exit);
  ASSERT(sscanf(argv[4], "%lu", &raw_threshold),
         "Error reading sharpen's `threshold` integer", exit);
  threshold = ((float)raw_threshold) / 255.0f;
  ASSERT(threshold >= 0.0f && threshold <= 1.0f,
         "Sharpen's `threshold` integer isn't inside 0..255 interval", exit);
  ASSERT(sscanf(argv[5], "%f", &sharpen_factor),
         "Error reading sharpen's `sharpen_factor` float", exit);
  ASSERT(sharpen_factor >= 0.0f && sharpen_factor <= 2.0f,
         "Sharpen's `sharpen_factor` float isn't inside 0..2 interval", exit);
  if (argc >= 7) {
      ASSERT(sscanf(argv[6], "%d", &threads_per_block),
             "Error reading `threads_per_block` integer multiplier", exit);
      threads_per_block *= 256;
      ASSERT(threads_per_block >= 1 && threads_per_block <= 1024, "Filter's "
          "`threads_per_block` is multiplied by 256, it should be inside 1..4",
          exit);
  }
  // Tries to open/close the output file in append-mode just to test if it's possible
  output_file = fopen(argv[2], "a");
  ASSERT(output_file != NULL, "Error opening the output file", exit);
  ASSERT(fclose(output_file) == 0, "Error closing the output file", exit);
  output_file = NULL;
  // Opens the source file and reads the PPM image
  source_file = fopen(argv[1], "r");
  ASSERT(source_file != NULL, "Error opening the source file", exit);
  image = read_ppm_image(source_file);
  ASSERT(fclose(source_file) == 0, "Error closing the source file", exit);
  source_file = NULL;
  ASSERT(image != NULL, "Error reading the PPM image", exit);
  // Apply the PPM image filter
  ASSERT(filter_ppm_image(image, threshold, sharpen_factor, m, threads_per_block),
         "Error applying the filter to the PPM image", exit);
  // Saves the PPM image to the output file
  output_file = fopen(argv[2], "w");
  ASSERT(output_file != NULL, "Error opening the output file", exit);
  ASSERT(save_ppm_image(image, output_file), "Error saving the PPM image",
         exit);
  ASSERT(fclose(output_file) == 0, "Error closing the output file", exit);
  output_file = NULL;
  exit_code = EXIT_SUCCESS;
exit:
  free_ppm_image(&image);
  return exit_code;
}
