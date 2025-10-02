# Programação Paralela: EP 2 - Realce Seletivo

<!--
SPDX-FileCopyrightText: 2025 Guilherme Leoi <leoi.guilherme@aluno.ufabc.edu.br>

SPDX-License-Identifier: CC-BY-SA-4.0
-->

## Ambiente para Sequencial/OpenMP/Pthreads/Checker

[Com Nix instalado](https://zero-to-nix.com/start/install/), apenas execute
`nix develop` dentro da pasta do projeto.

## Ambiente para CUDA

Como o CUDA depende do driver de vídeo presente no sistema operacional, é
mais simples configurar o ambiente em um contêiner com acesso à GPU ou direto
no sistema.

[Os drivers CUDA instalados no SO foram da versão 12.8](https://docs.nvidia.com/cuda/archive/12.8.0/cuda-installation-guide-linux/index.html),
pois a versão 13.0 (mais recente até o momento) retirou suporte às placas Pascal.

Utilizou-se [distrobox](https://github.com/89luca89/distrobox) para a
criação de um contêiner Fedora 41 possuindo ponte para a GPU com
`distrobox create --image fedora:41 --nvidia -n cuda`.

Depois de entrar no contêiner com `distrobox enter cuda`, instalou-se
gcc versão 14 com `sudo dnf install -y gcc` e [CUDA Toolkit versão 12.8](https://developer.nvidia.com/cuda-12-8-0-download-archive).

## Comandos utilizados neste projeto

Os scripts deste projeto utilizam a variável de ambiente `CC` como referência
para qual compilador utilizar.

Caso ela não esteja presente, assume-se `gcc-14` como valor padrão.

### Build para Sequencial/OpenMP/Pthreads/Checker

`./tools/build_release.sh` ou `./tools/build_debug.sh`

### Build para CUDA

`./tools/build_cuda.sh`.

### Execução dos programas

`./target/<debug ou release>/<variante> <imagem de entrada> <imagem de saída>
<M> <threshold> <sharpen factor> <opcional: nº de threads>`

Obs.: [As imagens PPM no diretório inputs](./inputs) foram armazenadas com
[Git LFS](https://git-lfs.com/), para baixá-las é necessário executar
`git lfs fetch --all` e `git lfs pull`.

### Checagem de similaridade entre as imagens

`./target/<debug ou release>/checker <imagens 1> <imagem 2> ... <imagem N>`

Obs.: A última imagem listada será utilizada como referencial.

### Benchmark

`./tools/benchmark.sh`
