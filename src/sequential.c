// SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
//
// SPDX-License-Identifier: AGPL-3.0-only

#include "filter.h"
#include "ppm.h"
#include <math.h>
#include <stddef.h>

int grayscale(PpmImage *image) {
  if (image == NULL)
    return 0;
  size_t image_size = image->width * image->height;
  for (size_t idx = 0; idx < image_size; idx++) {
    RgbTriplet rgb;
    if (!read_at_idx_ppm_image(image, idx, &rgb))
      return 0;
    float y = roundf(0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b);
    RgbTriplet grayscale_rgb = (RgbTriplet){.r = y, .g = y, .b = y};
    if (!write_at_idx_ppm_image(image, idx, grayscale_rgb))
      return 0;
  }
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

int blur_at(PpmImage *image, size_t m, size_t x, size_t y) {
  if (image == NULL)
    return 0;
  if (x > image->width || y > image->height)
    return 0;
  size_t idx = x + y * image->width;
  size_t radius = r_pixel(image, m, x, y);
  float sum_r = 0.0, sum_g = 0.0, sum_b = 0.0;
  for (long i = -radius; i <= radius; i++) {
    for (long j = -radius; j <= radius; j++) {
      if (-i > x || -j > y)
        continue;
      if ((x + i) > image->width || (y + j) > image->height)
        continue;
      size_t neighbour_idx = (x + i) + (y + j) * image->width;
      RgbTriplet neighbour_rgb;
      if (!read_at_idx_ppm_image(image, neighbour_idx, &neighbour_rgb))
        return 0;
      sum_r += neighbour_rgb.r;
      sum_g += neighbour_rgb.g;
      sum_b += neighbour_rgb.b;
    }
  }
  float n = (float)(1 + radius * 2);
  RgbTriplet rgb = (RgbTriplet){.r = sum_r / n, .g = sum_g / n, .b = sum_b / n};
  if (!write_at_idx_ppm_image(image, idx, rgb))
    return 0;
  return 1;
}

int filter_ppm_image(PpmImage *image) {
  if (image == NULL)
    return 0;
  if (!grayscale(image))
    return 0;
  for (size_t x = 0; x < image->width; x++)
    for (size_t y = 0; y < image->height; y++)
      if (!blur_at(image, 5, x, y))
        return 0;
  // TODO
  return 1;
}
