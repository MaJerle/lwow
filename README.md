# Lightweight onewire library

LwOW is lightweight, platform independent library for Onewire protocol for embedded systems.
Its primary focus is UART hardware for physical communication for sensors and other slaves.

More about 1-Wire over UART can be found on link below.

https://www.maximintegrated.com/en/app-notes/index.mvp/id/214

<h3>Read first: <a href="http://docs.majerle.eu/projects/lwow/">Documentation</a></h3>

## Features

* Written in C (C11)
* Platform independent, uses custom low-level layer for device drivers
* 1-Wire protocol fits UART specifications at ``9600`` and ``115200`` bauds
* Allows standard one-wire single-gpio manual control (when UARTs are no more available by the system)
* Hardware is responsible for timing characteristics
    * Allows DMA on the high-performance microcontrollers
* Different device drivers included
    * DS18B20 temperature sensor is natively supported
* Works with operating system due to hardware timing management
    * Separate thread-safe API is available
* API for device scan, reading and writing single bits
* User friendly MIT license

## Contribute

Fresh contributions are always welcome. Simple instructions to proceed:

1. Fork Github repository
2. Follow [C style & coding rules](https://github.com/MaJerle/c-code-style) already used in the project
3. Create a pull request to develop branch with new features or bug fixes

Alternatively you may:

1. Report a bug
2. Ask for a feature request