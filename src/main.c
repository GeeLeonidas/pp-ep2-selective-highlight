// SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
//
// SPDX-License-Identifier: AGPL-3.0-only

#include "filter.h"
#include "ppm.h"
#include <stdlib.h>

int main(int argc, char **argv) {
  int exit_code = EXIT_FAILURE;
  PpmImage *image = read_ppm_image(stdin);
  if (!filter_ppm_image(image))
    goto exit;
  if (!save_ppm_image(image, stdout))
    goto exit;
  exit_code = EXIT_SUCCESS;
exit:
  free_ppm_image(&image);
  return exit_code;
}
