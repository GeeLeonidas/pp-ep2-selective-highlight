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
  PpmImage *image = NULL;
  FILE *source_file = NULL, *output_file = NULL;
  ASSERT(argc >= 6, "Missing arguments (min.: 5)", exit);
  // Reads the runtime parameters
  size_t m, raw_threshold;
  float sharpen_factor, threshold;
  ASSERT(sscanf(argv[3], "%lu", &m),
         "Error reading variable radius' `m` integer", exit);
  ASSERT(sscanf(argv[4], "%lu", &raw_threshold),
         "Error reading sharpen's `threshold` integer", exit);
  threshold = ((float)raw_threshold) / 255.0f;
  ASSERT(threshold >= 0.0f && threshold <= 1.0f,
         "Sharpen's `threshold` integer isn't inside 0..255 interval", exit);
  ASSERT(sscanf(argv[5], "%f", &sharpen_factor),
         "Error reading sharpen's `sharpen_factor` float", exit);
  ASSERT(sharpen_factor >= 0.0f && sharpen_factor <= 2.0f,
         "Sharpen's `sharpen_factor` float isn't inside 0..2 interval", exit);
  int thread_count = 6;
  if (argc >= 7)
    ASSERT(sscanf(argv[6], "%d", &thread_count),
           "Error reading `thread_count` integer", exit);
  // Tries to open/close the output file in append-mode just to test if it's possible
  output_file = fopen(argv[2], "a");
  ASSERT(output_file != NULL, "Error opening the output file", exit);
  ASSERT(fclose(output_file) == 0, "Error closing the output file", exit);
  output_file = NULL;
  // Opens the source file and reads the PPM image
  source_file = fopen(argv[1], "r");
  ASSERT(source_file != NULL, "Error opening the source file", exit);
  image = read_ppm_image(source_file);
  ASSERT(fclose(source_file) == 0, "Error closing the source file", exit);
  source_file = NULL;
  ASSERT(image != NULL, "Error reading the PPM image", exit);
  // Apply the PPM image filter
  ASSERT(filter_ppm_image(image, threshold, sharpen_factor, m, thread_count),
         "Error applying the filter to the PPM image", exit);
  // Saves the PPM image to the output file
  output_file = fopen(argv[2], "w");
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
