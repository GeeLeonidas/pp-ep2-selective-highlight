// SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
//
// SPDX-License-Identifier: AGPL-3.0-only

#include "ppm.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(expr, msg, exit_label)                                          \
  if (!(expr)) {                                                               \
    puts(msg);                                                                 \
    goto exit_label;                                                           \
  }

PpmImage *read_ppm_image(FILE *source_file) {
  ASSERT(source_file != NULL, "Source file is NULL", read_ppm_image_error);
  PpmImage *image = malloc(sizeof(PpmImage));
  char header[2];
  ASSERT(fscanf(source_file, "%c%c", &header[0], &header[1]),
         "Error reading the file header", read_ppm_image_error);
  ASSERT(header[0] == 'P' && header[1] == '3',
         "Unsupported format (expected `P3`)", read_ppm_image_error)
  ASSERT(fscanf(source_file, "%lu %lu", &image->width, &image->height),
         "Error reading `width` and `height` integers", read_ppm_image_error);
  ASSERT(fscanf(source_file, "%hu", &image->max_value),
         "Error reading `max_value` integer", read_ppm_image_error);
  size_t image_size = image->width * image->height;
  image->color_values_write = malloc(image_size * sizeof(RgbTriplet));
  image->color_values_read = malloc(image_size * sizeof(RgbTriplet));
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
  if (image == NULL || image->color_values_write == NULL ||
      idx > image->width * image->height)
    return 0;
  image->color_values_write[idx] = rgb;
  image->needs_flushing = 1;
  return 1;
}

int write_at_xy_ppm_image(PpmImage *image, size_t x, size_t y, RgbTriplet rgb) {
  if (image == NULL || x > image->width || y > image->height)
    return 0;
  return write_at_idx_ppm_image(image, x + y * image->width, rgb);
}

int flush_ppm_image(PpmImage *image) {
  if (image == NULL || image->color_values_write == NULL ||
      image->color_values_read == NULL)
    return 0;
  if (image->needs_flushing) {
    size_t image_size = image->width * image->height;
    for (size_t idx = 0; idx < image_size; idx++)
      image->color_values_read[idx] = image->color_values_write[idx];
    image->needs_flushing = 0;
  }
  return 1;
}

int read_at_idx_ppm_image(PpmImage *image, size_t idx, RgbTriplet *rgb) {
  if (image == NULL || image->color_values_read == NULL || rgb == NULL)
    return 0;
  *rgb = image->color_values_read[idx];
  return 1;
}

int read_at_xy_ppm_image(PpmImage *image, size_t x, size_t y, RgbTriplet *rgb) {
  if (image == NULL || x > image->width || y > image->height)
    return 0;
  return read_at_idx_ppm_image(image, x + y * image->width, rgb);
}

int save_ppm_image(PpmImage *image, FILE *output_file) {
  ASSERT(image != NULL, "PPM image is NULL", save_ppm_image_error);
  ASSERT(output_file != NULL, "Output file is NULL", save_ppm_image_error);
  ASSERT(fprintf(output_file, "P3\n"), "Error writing PPM image header",
         save_ppm_image_error);
  ASSERT(fprintf(output_file, "%lu %lu\n", image->width, image->height),
         "Error writing `width` and `height` integers", save_ppm_image_error);
  ASSERT(fprintf(output_file, "%hu\n", image->max_value),
         "Error writing `max_value` integer", save_ppm_image_error);
  size_t image_size = image->width * image->height;
  for (int idx = 0; idx < image_size; idx++) {
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
