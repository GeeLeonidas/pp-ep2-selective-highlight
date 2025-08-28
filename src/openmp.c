// SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
//
// SPDX-License-Identifier: AGPL-3.0-only

#include "filter.h"
#include "ppm.h"
#include <stddef.h>

#define OMP_ASSERT(expr, msg, error_msg)                                       \
  if (!(expr)) {                                                               \
    error_msg = msg;                                                           \
    continue;                                                                  \
  }
#define OMP_SKIP_ON_ERROR(error_msg)                                           \
  if (error_msg != NULL) {                                                     \
    continue;                                                                  \
  }
#define OMP_HANDLE_ASSERTS(error_msg)                                          \
  if (error_msg != NULL) {                                                     \
    puts(error_msg);                                                           \
    return 0;                                                                  \
  }

int grayscale(PpmImage *image, int thread_count) {
  if (image == NULL)
    return 0;
  size_t image_size = image->width * image->height;
  char *error_msg = NULL;
#pragma omp parallel for num_threads(thread_count)
  for (size_t idx = 0; idx < image_size; idx++) {
    OMP_SKIP_ON_ERROR(error_msg);
    RgbTriplet rgb;
    OMP_ASSERT(read_at_idx_ppm_image(image, idx, &rgb),
               "Error reading PPM image at index", error_msg);
    float y = 0.299f * rgb.r + 0.587f * rgb.g + 0.114f * rgb.b;
    RgbTriplet grayscale_rgb = (RgbTriplet){.r = y, .g = y, .b = y};
    OMP_ASSERT(write_at_idx_ppm_image(image, idx, grayscale_rgb),
               "Error writing PPM image at index", error_msg);
  }
  OMP_HANDLE_ASSERTS(error_msg);
  if (!flush_ppm_image(image))
    return 0;
  return 1;
}

size_t r_pixel(PpmImage *image, size_t m, size_t x, size_t y) {
  if (image == NULL)
    return 0;
  if (x > image->width || y > image->height)
    return 0;
  RgbTriplet rgb;
  size_t idx = x + y * image->width;
  if (!read_at_idx_ppm_image(image, idx, &rgb))
    return 0;
  float sum = rgb.r + rgb.g + rgb.b;
  return (((size_t)(sum * 255)) % m) + 1;
}

int blur_at(PpmImage *image, size_t m, size_t x, size_t y, RgbTriplet *rgb) {
  if (image == NULL)
    return 0;
  if (x > image->width || y > image->height)
    return 0;
  size_t radius = r_pixel(image, m, x, y);
  if (radius == 0)
    return 0;
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
      if (!read_at_idx_ppm_image(image, neighbour_idx, &neighbour_rgb))
        return 0;
      sum_r += neighbour_rgb.r;
      sum_g += neighbour_rgb.g;
      sum_b += neighbour_rgb.b;
    }
  }
  float n = (float)((1 + radius * 2) * (1 + radius * 2));
  *rgb = (RgbTriplet){.r = sum_r / n, .g = sum_g / n, .b = sum_b / n};
  return 1;
}

float clamp_zero_one(float input) {
  return (input >= 1.0f) ? 1.0f : ((input <= 0.0f) ? 0.0f : input);
}

int sharpen(PpmImage *image, float threshold, float sharpen_factor, size_t m,
            int thread_count) {
  if (image == NULL)
    return 0;
  char *error_msg = NULL;
#pragma omp parallel for num_threads(thread_count) collapse(2)
  for (size_t x = 0; x < image->width; x++) {
    for (size_t y = 0; y < image->height; y++) {
      OMP_SKIP_ON_ERROR(error_msg);
      RgbTriplet rgb, blur, new_rgb;
      OMP_ASSERT(read_at_xy_ppm_image(image, x, y, &rgb),
                 "Error reading PPM image at (X,Y) coords", error_msg);
      OMP_ASSERT(blur_at(image, m, x, y, &blur),
                 "Error calculating blur at (X,Y) coords", error_msg);
      if (rgb.r <= threshold)
        new_rgb = blur;
      else
        new_rgb = (RgbTriplet){
            .r = clamp_zero_one(rgb.r + sharpen_factor * (rgb.r - blur.r)),
            .g = clamp_zero_one(rgb.g + sharpen_factor * (rgb.g - blur.g)),
            .b = clamp_zero_one(rgb.b + sharpen_factor * (rgb.b - blur.b))};
      OMP_ASSERT(write_at_xy_ppm_image(image, x, y, new_rgb),
                 "Error writing PPM image at (X,Y) coords", error_msg);
    }
  }
  OMP_HANDLE_ASSERTS(error_msg);
  if (!flush_ppm_image(image))
    return 0;
  return 1;
}

int filter_ppm_image(PpmImage *image, float threshold, float sharpen_factor,
                     size_t m, int thread_count) {
  if (image == NULL)
    return 0;
  if (!sharpen(image, threshold, sharpen_factor, m, thread_count))
    return 0;
  if (!grayscale(image, thread_count))
    return 0;
  return 1;
}
