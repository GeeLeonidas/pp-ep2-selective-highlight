#!/usr/bin/env sh
# SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
#
# SPDX-License-Identifier: 0BSD OR CC0-1.0

if [ ! -z CC ]; then
    CC=gcc-14
fi

OLD_CC=$CC

rm -rf target/release/
mkdir -p target/release/

SEQ_VARIANT="$CC,sequential,"
OMP_VARIANT="$CC,openmp,-fopenmp"
PTHREADS_VARIANT="$CC,pthreads,"

LOOP_PARAMETERS="$SEQ_VARIANT;$OMP_VARIANT;$PTHREADS_VARIANT;"
while IFS=',' read -d';' -r CC VARIANT EXTRA_ARGS; do
    echo "Compiling $VARIANT variant with $CC..."
    $CC -xc src/main.c "src/ppm.c" "src/$VARIANT.c" -lm -O3 \
        -Wall -Wextra -Wdouble-promotion -Wconversion \
        -Wno-sign-conversion $EXTRA_ARGS \
        -flto -o target/release/$VARIANT
done <<< "$LOOP_PARAMETERS"
echo "All CPU variants were compiled!"

CC=$OLD_CC

echo "Compiling checker with $CC..."
$CC -xc src/checker.c src/ppm.c -lm -O3 \
    -Wall -Wextra -Wdouble-promotion -Wconversion \
    -Wno-sign-conversion \
    -flto -o target/release/checker
echo "Checker was compiled!"
