# Raspberry Pi Zero bare-metal

## Build

```sh
$ cd build
$ export TOOLCHAIN_PREFIX=<path to arm-none-eabi parent>
$ cmake -DTOOLCHAIN_PREFIX=${TOOLCHAIN_PREFIX} -DCMAKE_TOOLCHAIN_FILE=../../../toolchain.cmake -DCMAKE_BUILD_TYPE=Release ..
$ make
```

## Execute 

### Qemu

```sh
$ qemu-system-arm -M raspi0 -semihosting -kernel Kernel.elf -nographic
```

### Physical Hardware

TODO