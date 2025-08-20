#!/usr/bin/env sh
# SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
#
# SPDX-License-Identifier: 0BSD OR CC0-1.0

rm -rf target/release/
mkdir -p target/release/

VARIANTS="
sequential
"
for VARIANT in $VARIANTS; do
    if [[ ! -z $VARIANT ]]; then
        echo "Compiling $VARIANT variant with $CC..."
        $CC -xc src/main.c src/ppm.c "src/$VARIANT.c" -lm -O3 \
            -Wall -Wextra -Wdouble-promotion -Wconversion \
            -Wno-sign-conversion \
            -flto -o target/release/$VARIANT
    fi
done
echo "All variants were compiled!"
