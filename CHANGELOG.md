# Changelog

## Develop

- Move system functions to `system/lwow_sys.h` file
- Add support for Platform.IO
- Add CMakeLists.txt file for develop in VSCode

## v3.0.2

- Update CMSIS OS driver to support FreeRTOS aware kernel

## v3.0.1

- Change configuration options from _CONFIG_ to _OPT
- Apply code style settings with Artistic style options
- Update docs

## v3.0.0

- Break compatibility vs `2.0.0`
- New name of library is now LwOW - Lightweight onewire
- Added `_ex` functions to read/write bytes or bits
- Added drivers features as set of callback functions with drv pointer
- Rename private functions to `prv_` prefix

## v2.0.0

- LL drivers are now passed as custom structure to allow multiple blocks as separate drivers
- Added first sphinx documentation
- Updated examples

## v1.2.0

- Automatically set result to OK when search function finds at least 1 connected device
- Upgrade examples to CMSIS OS V2
- Use pre-increment/decrement
- Other C code style fixes

## v1.1.0

- Added assert for all API functions
- Added new owPARERR enumerator for wrong parameters
- Added `ow_deinit` function implementation

## v1.0.0

- Use separate ROM structure instead of byte array
- Add option to search devices with command and save result directly to input array
- Make all variables as local instead of using user pointers

## v0.2.0

- Removed `protect` parameter in API functions
- Added `_raw` functions to provide non-thread-safe implementation for operating systems
- Separate API functions with operating system protection

## v0.1.0

- Support for UART interface instead of software emulation on microcontroller or PC application
- Support for operating systems (including non-real-time)
- Added API driver for *DS18x20* temperature sensor