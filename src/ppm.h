// SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
//
// SPDX-License-Identifier: AGPL-3.0-only

#ifndef PPM_HEADER
#define PPM_HEADER

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct rgb_triplet {
  float r, g, b;
} RgbTriplet;

typedef struct ppm_image {
  size_t width, height;
  uint16_t max_value;
  RgbTriplet *color_values_write;
  RgbTriplet *color_values_read;
  uint8_t needs_flushing;
} PpmImage;

PpmImage *read_ppm_image(FILE *source_file);
int write_at_idx_ppm_image(PpmImage *image, size_t idx, RgbTriplet rgb);
int write_at_xy_ppm_image(PpmImage *image, size_t x, size_t y, RgbTriplet rgb);
int read_at_idx_ppm_image(PpmImage *image, size_t idx, RgbTriplet *rgb);
int read_at_xy_ppm_image(PpmImage *image, size_t x, size_t y, RgbTriplet *rgb);
int flush_ppm_image(PpmImage *image);
int save_ppm_image(PpmImage *image, FILE *output_file);
void free_ppm_image(PpmImage **image);

#endif // PPM_HEADER
