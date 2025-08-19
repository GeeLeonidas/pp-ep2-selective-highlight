# SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>
#
# SPDX-License-Identifier: 0BSD OR CC0-1.0

{
  description = "C/C++ Template";

  inputs = {
    pre-commit-hooks.url = "github:cachix/git-hooks.nix";
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  };

  outputs =
    {
      self,
      nixpkgs,
      ...
    }@inputs:
    let
      supportedSystems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
    in
    {
      checks = forAllSystems (
        system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          sequential-build = pkgs.callPackage ./nix/sequential.nix { };

          pre-commit-check = inputs.pre-commit-hooks.lib.${system}.run {
            src = ./.;
            hooks = {
              # Markdown
              lychee = {
                enable = true;
                settings.flags = "--exclude-path LICENSES/";
              };
              markdownlint.enable = true;

              # C
              clang-format = rec {
                enable = true;
                types_or = pkgs.lib.mkForce [
                  "c"
                  "c++"
                ];
                package = pkgs.clang-tools;
                entry = "${package}/bin/clang-format -style=file -i";
              };

              # Nix
              nixfmt-rfc-style.enable = true;
              flake-checker.enable = true;
              statix.enable = true;

              # Git
              convco.enable = true;

              # Licensing
              reuse.enable = true;

              # Security
              ripsecrets.enable = true;

              # EOFs and newlines
              end-of-file-fixer.enable = true;
              mixed-line-endings.enable = true;
              trim-trailing-whitespace.enable = true;

              # File system
              check-symlinks.enable = true;
              check-case-conflicts.enable = true;
              check-added-large-files.enable = true;
            };
          };
        }
      );
      packages = forAllSystems (
        system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          default = pkgs.callPackage ./nix/sequential.nix { };
        }
      );
      devShells = forAllSystems (
        system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          default = nixpkgs.legacyPackages.${system}.mkShell {
            inherit (self.checks.${system}.pre-commit-check) shellHook;
            buildInputs =
              (with pkgs; [
                clang

                lldb
                valgrind-light
                linuxPackages_latest.perf
              ])
              ++ self.checks.${system}.pre-commit-check.enabledPackages;
          };
        }
      );
    };
}
