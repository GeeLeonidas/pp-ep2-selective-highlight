#!/usr/bin/env sh
# SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
#
# SPDX-License-Identifier: 0BSD OR CC0-1.0

if [ ! -z CC ]; then
    CC=gcc-14
fi

rm -f target/release/cuda
mkdir -p target/release/

CUDA_PATH="$(find /usr/local -regex '.+/cuda-[0-9]+\.[0-9]+$')"
PATH="$CUDA_PATH/bin:$PATH"
LD_LIBRARY_PATH="$CUDA_PATH/lib64:$LD_LIBRARY_PATH"

echo "Compiling cuda variant with nvcc..."
nvcc src/cuda.cu -O3 \
    -arch=compute_61 -code=sm_61 \
    -Wno-deprecated-gpu-targets \
    -o target/release/cuda
echo "Variant was compiled!"
