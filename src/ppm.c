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
  image->color_values = malloc(image_size * sizeof(RgbTriplet));
  for (int idx = 0; idx < image_size; idx++) {
    uint16_t red, green, blue;
    if (!fscanf(source_file, "%hu %hu %hu", &red, &green, &blue))
      goto read_ppm_image_error;
    image->color_values[idx].r = red;
    image->color_values[idx].g = green;
    image->color_values[idx].b = blue;
  }
  return image;
read_ppm_image_error:
  free_ppm_image(&image);
  return NULL;
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
    uint16_t red, green, blue;
    red = image->color_values[idx].r;
    green = image->color_values[idx].g;
    blue = image->color_values[idx].b;
    if (!fprintf(output_file, "%hu %hu %hu\n", red, green, blue))
      return 0;
  }
  return 1;
}

void free_ppm_image(PpmImage **image) {
  if (image == NULL || *image == NULL)
    return;
  if ((*image)->color_values) {
    free(&((*image)->color_values));
    (*image)->color_values = NULL;
  }
  free(*image);
  *image = NULL;
}
