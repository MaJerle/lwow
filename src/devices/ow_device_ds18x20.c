/**
 * \file            ow.c
 * \brief           OneWire protocol implementation
 */
 
/*
 * Copyright (c) 2018 Tilen Majerle
 *  
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of OneWire library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#include "ow/ow.h"
#include "ow/devices/ow_device_ds18x20.h"

/**
 * \brief           Start temperature conversion on specific (or al) devices
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: ROM id of device to start measurement.
 *                  Set to `NULL` to start measurement on all devices
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ds18x20_start(ow_t* ow, uint8_t* rom_id) {
    if (rom_id == NULL) {
        ow_skip_rom(ow);                        /* Skip ROM, send to all devices */
    } else {
        ow_match_rom(ow, rom_id);               /* Select exact device by ROM address */
    }
    return 1;
}

/**
 * \brief           Read temperature previously started with \ref ow_ds18x20_start
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to read data from
 * \param[out]      t: Temperature handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ds18x20_read(ow_t* ow, uint8_t* rom_id, float* t) {
    return 1;
}

/**
 * \brief           Check if ROM address matches DS18B20 device
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to test for DS18B20
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ds18x20_is_b(ow_t* ow, uint8_t* rom_id) {
    OW_UNUSED(ow);                              /* Unused variable */
    return rom_id != NULL && *rom_id == 0x28;   /* Check for correct ROM family code */
}

/**
 * \brief           Check if ROM address matches DS18S20 device
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to test for DS18S20
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ds18x20_is_s(ow_t* ow, uint8_t* rom_id) {
    OW_UNUSED(ow);                              /* Unused variable */
    return rom_id != NULL && *rom_id == 0x10;   /* Check for correct ROM family code */
}
