// SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
//
// SPDX-License-Identifier: AGPL-3.0-only

#include "ppm.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(expr, msg, exit_label)                                          \
  if (!(expr)) {                                                               \
    puts(msg);                                                                 \
    goto exit_label;                                                           \
  }

#define LIMIT 0.01f
#define SQUARED_LIMIT (LIMIT * LIMIT)

int main(int argc, char **argv) {
  int exit_code = EXIT_FAILURE;
  size_t another_image_count = (size_t)(argc - 2);
  PpmImage *baseline_image = NULL;
  PpmImage **another_images = NULL;
  ASSERT(argc > 2, "Need at least two file paths to be checked", exit);
  FILE *baseline_file = fopen(argv[argc - 1], "r");
  ASSERT(baseline_file != NULL, "Error opening the baseline file", exit);
  baseline_image = read_ppm_image(baseline_file);
  ASSERT(fclose(baseline_file) == 0, "Error closing the baseline file", exit);
  baseline_file = NULL;
  ASSERT(baseline_image != NULL, "Error reading the baseline PPM image", exit);
  another_images = malloc(another_image_count * sizeof(PpmImage *));
  ASSERT(another_images != NULL, "Could not allocate another images", exit);
  for (size_t idx = 0; idx < another_image_count; idx++)
    another_images[idx] = NULL;
  for (size_t idx = 0; idx < another_image_count; idx++) {
    FILE *another_file = fopen(argv[idx + 1], "r");
    ASSERT(another_file != NULL, "Error opening another file", exit);
    another_images[idx] = read_ppm_image(another_file);
    ASSERT(fclose(another_file) == 0, "Error closing another file", exit);
    another_file = NULL;
    ASSERT(another_images[idx] != NULL, "Error reading another PPM image",
           exit);
  }
  size_t surpassed_count = 0;
  for (size_t image_idx = 0; image_idx < another_image_count; image_idx++) {
    printf("\"%s\" versus \"%s\"\n", argv[image_idx + 1], argv[argc - 1]);
    PpmImage *another_image = another_images[image_idx];
    size_t image_size = another_image->width * another_image->height;
    size_t old_surpassed_count = surpassed_count;
    for (size_t triplet_idx = 0; triplet_idx < image_size; triplet_idx++) {
      RgbTriplet baseline_rgb, another_rgb, squared_diff;
      ASSERT(read_at_idx_ppm_image(baseline_image, triplet_idx, &baseline_rgb),
             "Error reading PPM baseline image at index", exit);
      ASSERT(read_at_idx_ppm_image(another_image, triplet_idx, &another_rgb),
             "Error reading PPM image at index", exit);
      squared_diff.r = baseline_rgb.r - another_rgb.r;
      squared_diff.r = squared_diff.r * squared_diff.r;
      squared_diff.g = baseline_rgb.g - another_rgb.g;
      squared_diff.g = squared_diff.g * squared_diff.g;
      squared_diff.b = baseline_rgb.b - another_rgb.b;
      squared_diff.b = squared_diff.b * squared_diff.b;
      int has_surpassed = squared_diff.r > SQUARED_LIMIT ||
                          squared_diff.g > SQUARED_LIMIT ||
                          squared_diff.b > SQUARED_LIMIT;
      if (has_surpassed) {
        size_t x = triplet_idx % another_image->width;
        size_t y = triplet_idx / another_image->width;
        printf("Surpassed the limit (i.e. %f) at XY(%lu, %lu): "
               "RGB(%f, %f, %f) versus RGB(%f, %f, %f)",
               (double)LIMIT, x, y, (double)another_rgb.r,
               (double)another_rgb.g, (double)another_rgb.b,
               (double)baseline_rgb.r, (double)baseline_rgb.g,
               (double)baseline_rgb.b);
        surpassed_count++;
      }
    }
    if (old_surpassed_count == surpassed_count)
      puts("No significant difference between the images was found");
  }
  exit_code = (surpassed_count == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
exit:
  free_ppm_image(&baseline_image);
  if (another_images != NULL)
    for (size_t idx = 0; idx < another_image_count; idx++)
      free_ppm_image(&another_images[idx]);
  return exit_code;
}
