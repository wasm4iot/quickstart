# Getting started with WASM for IoT

## Cloning

Clone with `git clone <...> --recursive`

## Introduction

TODO

## Devcontainer

To set up a development environment with the required toolchain, use the 
[Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) Visual Studio Code plugin.

## Supported Platforms

- [x] NRF52840
  - [x] [Wasm3](./targets/arm-none-eabi/nrf52840/wasm3-coremark/)
  - [x] [WebAssembly Micro Runtime](./targets/arm-none-eabi/nrf52840/wamr-coremark/)
- [x] Hifive1 (Rev B)
  - [x] [Wasm3](./targets/riscv32-unknown-none/hifive1/generic-wasm3/)
- [x] SiPEED MAiX BiT
  - [x] [Wasm3](./targets/riscv64-unknown-none/sipeed/maix-bit/wasm3-embench/)
- [x] SiPEED MAiX GO
  - [x] [Wasm3](./targets/riscv64-unknown-none/sipeed/maix-go/wasm3-embench/)