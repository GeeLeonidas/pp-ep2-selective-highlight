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

int filter_ppm_image(PpmImage *image) {
  if (image == NULL || image->color_values == NULL)
    return 0;
  if (!grayscale(image))
    return 0;
  // TODO
  return 1;
}
