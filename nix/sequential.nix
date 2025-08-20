# SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
#
# SPDX-License-Identifier: 0BSD OR CC0-1.0

{ pkgs, ... }:
pkgs.stdenv.mkDerivation {
  name = "pp-ep2";

  src = ../.;

  buildPhase = ''
    $CC src/main.c src/ppm.c src/sequential.c -lm -o pp-ep2
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp pp-ep2 $out/bin/pp-ep2
  '';
}
