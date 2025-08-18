// SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
//
// SPDX-License-Identifier: AGPL-3.0-only

#include "ppm.h"
#include <stdlib.h>

#ifdef PTHREADS
#include "pthreads.h"
#else
#ifdef OPENMP
#include "openmp.h"
#else
#ifdef CUDA
#include "cuda.h"
#else // If nothing was defined, assume sequential
#include "sequential.h"
#endif // CUDA
#endif // OPENMP
#endif // PTHREADS

int main(int argc, char **argv) {
  int exit_code = EXIT_FAILURE;
  PpmImage *image = read_ppm_image(stdin);
  if (!process_ppm_image(image))
    goto exit;
  if (!save_ppm_image(image, stdout))
    goto exit;
  exit_code = EXIT_SUCCESS;
exit:
  free_ppm_image(&image);
  return exit_code;
}
