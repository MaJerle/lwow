/**
 * \file            ow_device_ds18x20.h
 * \brief           DS18x20 library based on 1-Wire
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
 * This file is part of OneWire-UART.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#ifndef __ONEWIRE_DEVICE_DS18x20_H
#define __ONEWIRE_DEVICE_DS18x20_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ow/ow.h"

/**
 * \ingroup         OW_DEVICES
 * \defgroup        OW_DEVICE_DS18x20 DS18x20
 * \brief           Device driver for DS18x20
 * \{
 */

uint8_t     ow_ds18x20_start(ow_t* ow, uint8_t* rom_id);
uint8_t     ow_ds18x20_read(ow_t* ow, uint8_t* rom_id, float* t);
uint8_t     ow_ds18x20_is_b(ow_t* ow, uint8_t* rom_id);
uint8_t     ow_ds18x20_is_s(ow_t* ow, uint8_t* rom_id);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ONEWIRE_DEVICE_DS18x20_H */
