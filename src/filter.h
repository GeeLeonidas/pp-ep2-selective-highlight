// SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
//
// SPDX-License-Identifier: AGPL-3.0-only

#ifndef FILTER_HEADER
#define FILTER_HEADER

#include "ppm.h"

int filter_ppm_image(PpmImage *image, float threshold, float sharpen_factor,
                     size_t m, int thread_count);

#endif // FILTER_HEADER
