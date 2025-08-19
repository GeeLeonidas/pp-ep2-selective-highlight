// SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
//
// SPDX-License-Identifier: AGPL-3.0-only

#include "filter.h"
#include "ppm.h"
#include <math.h>
#include <stddef.h>

int grayscale(PpmImage *image) {
  if (image == NULL || image->color_values == NULL)
    return 0;
  size_t image_size = image->width * image->height;
  for (size_t idx = 0; idx < image_size; idx++) {
    float r, g, b;
    r = image->color_values[idx].r;
    g = image->color_values[idx].g;
    b = image->color_values[idx].b;
    float y = roundf(0.299 * r + 0.587 * g + 0.114 * b);
    image->color_values[idx].r = y;
    image->color_values[idx].g = y;
    image->color_values[idx].b = y;
  }
  return 1;
}

size_t r_pixel(PpmImage *image, size_t m, size_t x, size_t y) {
  if (image == NULL || image->color_values == NULL)
    return 0;
  if (x > image->width || y > image->height)
    return 0;
  float r, g, b;
  size_t idx = x + y * image->width;
  r = image->color_values[idx].r;
  g = image->color_values[idx].g;
  b = image->color_values[idx].b;
  float sum = r + g + b;
  return (((size_t)(sum * 255)) % m) + 1;
}

int blur_at(PpmImage *image, size_t m, size_t x, size_t y) {
  if (image == NULL || image->color_values == NULL)
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
      sum_r += image->color_values[neighbour_idx].r;
      sum_g += image->color_values[neighbour_idx].g;
      sum_b += image->color_values[neighbour_idx].b;
    }
  }
  float n = (float)(1 + radius * 2);
  image->color_values[idx].r = sum_r / n;
  image->color_values[idx].g = sum_g / n;
  image->color_values[idx].b = sum_b / n;
  return 1;
}

int filter_ppm_image(PpmImage *image) {
  if (image == NULL || image->color_values == NULL)
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
