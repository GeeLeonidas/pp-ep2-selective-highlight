#!/usr/bin/env sh
# SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
#
# SPDX-License-Identifier: 0BSD OR CC0-1.0

rm -f target/debug/cuda
mkdir -p target/debug/

echo "Compiling cuda variant with nvcc..."
nvcc src/main.c src/ppm.c src/cuda.cu -lm -O3 \
    -o target/debug/cuda
echo "Variant was compiled!"
