// SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
//
// SPDX-License-Identifier: AGPL-3.0-only

#include "ppm.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

PpmImage *read_ppm_image(FILE *source_file) {
  if (source_file == NULL)
    return NULL;
  PpmImage *image = malloc(sizeof(PpmImage));
  if (!fscanf(source_file, "P3"))
    goto read_ppm_image_error;
  if (!fscanf(source_file, "%lu %lu", &image->width, &image->height))
    goto read_ppm_image_error;
  if (!fscanf(source_file, "%hu", &image->max_value))
    goto read_ppm_image_error;
  size_t image_size = image->width * image->height;
  image->color_values_write = malloc(image_size * sizeof(RgbTriplet));
  image->color_values_read = malloc(image_size * sizeof(RgbTriplet));
  image->needs_flushing = 0;
  for (size_t idx = 0; idx < image_size; idx++) {
    uint16_t red, green, blue;
    if (!fscanf(source_file, "%hu %hu %hu", &red, &green, &blue))
      goto read_ppm_image_error;
    RgbTriplet rgb =
        (RgbTriplet){.r = ((float)red) / ((float)image->max_value),
                     .g = ((float)green) / ((float)image->max_value),
                     .b = ((float)blue) / ((float)image->max_value)};
    write_at_idx_ppm_image(image, idx, rgb);
  }
  flush_ppm_image(image);
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
  if (!fprintf(output_file, "P3\n"))
    return 0;
  if (!fprintf(output_file, "%lu %lu\n", image->width, image->height))
    return 0;
  if (!fprintf(output_file, "%hu\n", image->max_value))
    return 0;
  size_t image_size = image->width * image->height;
  for (int idx = 0; idx < image_size; idx++) {
    RgbTriplet rgb;
    read_at_idx_ppm_image(image, idx, &rgb);
    uint16_t red, green, blue;
    red = (uint16_t)(rgb.r * (float)image->max_value);
    green = (uint16_t)(rgb.g * (float)image->max_value);
    blue = (uint16_t)(rgb.b * (float)image->max_value);
    if (!fprintf(output_file, "%hu %hu %hu\n", red, green, blue))
      return 0;
  }
  return 1;
}

void free_ppm_image(PpmImage **image) {
  if (image == NULL || *image == NULL)
    return;
  if ((*image)->color_values_write) {
    free(&((*image)->color_values_write));
    (*image)->color_values_write = NULL;
  }
  if ((*image)->color_values_read) {
    free(&((*image)->color_values_read));
    (*image)->color_values_read = NULL;
  }
  free(*image);
  *image = NULL;
}
