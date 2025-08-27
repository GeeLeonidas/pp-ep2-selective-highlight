#!/usr/bin/env sh
# SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
#
# SPDX-License-Identifier: 0BSD OR CC0-1.0

rm -rf target/debug/
mkdir -p target/debug/

SEQ_VARIANT="$CC,sequential,"
OMP_VARIANT="$CC,openmp,-fopenmp"
PTHREADS_VARIANT="$CC,pthreads,"

LOOP_PARAMETERS="$SEQ_VARIANT;$OMP_VARIANT;$PTHREADS_VARIANT;"
while IFS=',' read -d';' -r CC VARIANT EXTRA_ARGS; do
    echo "Compiling $VARIANT variant with $CC..."
    $CC -xc src/main.c "src/ppm.c" "src/$VARIANT.c" -lm -g3 \
        -Wall -Wextra -Wdouble-promotion -Wconversion \
        -Wno-sign-conversion $EXTRA_ARGS \
        -o target/debug/$VARIANT
done <<< "$LOOP_PARAMETERS"
echo "All variants were compiled!"
