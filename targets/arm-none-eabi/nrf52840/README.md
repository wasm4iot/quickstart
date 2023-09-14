# NRF52840

## Build Instructions

To build the projects, enter a subdirectory and use `platformio run` using the corresponding environment.
See each `platformio.ini` for a list of available boards and environments.

Example:  
```sh
$ cd wasm3-coremark
$ pio run -e nrf52840_dongle -t build
```

## Upload Instructions

Platformio is set up to automatically upload each project to the target device.
Note that you might need to change the `upload_port` or `monitor_port` in a given `platformio.ini` according to what device your
devcontainer enumerates the board as.

Example:
```sh
$ cd wasm3-coremark
$ pio run -e nrf52840_dongle -t upload
```

## Monitor Instructions

Minicom can be used to monitor the console output of each demo project. Make sure you use the correct serial device and baud rate.

Example:  
```sh
# ... build and upload binary ...
$ minicom -D /dev/ttyACM1
```

## Port project to additional board

You can port each project to another board, given that it is supported by platformio and zephyr:

1. Add a new environment to platformio.ini, configuring board-specific settings.
2. Create a new zephyr device overlay (such as [nrf52840dongle_nrf52840.overlay](./wasm3-coremark/zephyr/nrf52840dongle_nrf52840.overlay))
for your board. This is required for board specific bindings, such as console output.
3. Build and upload the project to your new board!
