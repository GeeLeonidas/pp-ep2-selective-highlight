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
  RgbTriplet *color_values;
} PpmImage;

PpmImage *read_ppm_image(FILE *source_file);
int save_ppm_image(PpmImage *image, FILE *output_file);
void free_ppm_image(PpmImage **image);

#endif // PPM_HEADER
