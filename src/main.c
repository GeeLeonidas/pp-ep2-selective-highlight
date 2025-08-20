// SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
//
// SPDX-License-Identifier: AGPL-3.0-only

#include "filter.h"
#include "ppm.h"
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(expr, msg, exit_label)                                          \
  if (!(expr)) {                                                               \
    puts(msg);                                                                 \
    goto exit_label;                                                           \
  }

int main(int argc, char **argv) {
  int exit_code = EXIT_FAILURE;
  ASSERT(argc >= 6, "Missing arguments (min.: 5)", exit);
  FILE *source_file = fopen(argv[1], "r");
  ASSERT(source_file != NULL, "Error opening the source file", exit);
  PpmImage *image = read_ppm_image(source_file);
  ASSERT(fclose(source_file) == 0, "Error closing the source file", exit);
  source_file = NULL;
  ASSERT(image != NULL, "Error reading the PPM image", exit);
  size_t m, raw_threshold;
  float sharpen_factor, threshold;
  ASSERT(sscanf(argv[3], "%lu", &m),
         "Error reading variable radius' `m` integer", exit);
  ASSERT(sscanf(argv[4], "%lu", &raw_threshold),
         "Error reading sharpen's `threshold` integer", exit);
  ASSERT(sscanf(argv[5], "%f", &sharpen_factor),
         "Error reading sharpen's `sharpen_factor` float", exit);
  threshold = ((float)raw_threshold) / ((float)image->max_value);
  ASSERT(filter_ppm_image(image, threshold, sharpen_factor, m),
         "Error applying the filter to the PPM image", exit);
  FILE *output_file = fopen(argv[2], "w");
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
