/**
 * \file            ow_device_ds18x20.h
 * \brief           DS18x20 driver header
 */

/*
 * Copyright (c) 2020 Tilen MAJERLE
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
 * This file is part of OneWire-UART library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v2.0.0
 */
#ifndef OW_HDR_DEVICE_DS18x20_H
#define OW_HDR_DEVICE_DS18x20_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ow/ow.h"

/**
 * \ingroup         OW_DEVICES
 * \defgroup        OW_DEVICE_DS18x20 DS18x20 temperature sensor
 * \brief           Device driver for DS18x20 temperature sensor
 * \{
 *
 * \note            Functions with `_raw` suffix do no implement
 *                  locking mechanism when using with operating system.
 */

#define OW_DS18X20_ALARM_DISABLE                ((int8_t)-128)  /*!< Disable alarm temperature */
#define OW_DS18X20_ALARM_NOCHANGE               ((int8_t)-127)  /*!< Do not modify current alarm settings */
#define OW_DS18X20_TEMP_MIN                     ((int8_t)-55)   /*!< Minimum temperature */
#define OW_DS18X20_TEMP_MAX                     ((int8_t)125)   /*!< Maximal temperature */

uint8_t     ow_ds18x20_start_raw(ow_t* const ow, const ow_rom_t* const rom_id);
uint8_t     ow_ds18x20_start(ow_t* const ow, const ow_rom_t* const rom_id);

uint8_t     ow_ds18x20_read_raw(ow_t* const ow, const ow_rom_t* const rom_id, float* const t);
uint8_t     ow_ds18x20_read(ow_t* const ow, const ow_rom_t* const rom_id, float* const t);

uint8_t     ow_ds18x20_set_resolution_raw(ow_t* const ow, const ow_rom_t* const rom_id, const uint8_t bits);
uint8_t     ow_ds18x20_set_resolution(ow_t* const ow, const ow_rom_t* const rom_id, const uint8_t bits);

uint8_t     ow_ds18x20_get_resolution_raw(ow_t* const ow, const ow_rom_t* const rom_id);
uint8_t     ow_ds18x20_get_resolution(ow_t* const ow, const ow_rom_t* const rom_id);

uint8_t     ow_ds18x20_set_alarm_temp_raw(ow_t* const ow, const ow_rom_t* const rom_id, int8_t temp_l, int8_t temp_h);
uint8_t     ow_ds18x20_set_alarm_temp(ow_t* const ow, const ow_rom_t* const rom_id, int8_t temp_l, int8_t temp_h);

owr_t       ow_ds18x20_search_alarm_raw(ow_t* const ow, ow_rom_t* const rom_id);
owr_t       ow_ds18x20_search_alarm(ow_t* const ow, ow_rom_t* const rom_id);

uint8_t     ow_ds18x20_is_b(ow_t* const ow, const ow_rom_t* const rom_id);
uint8_t     ow_ds18x20_is_s(ow_t* const ow, const ow_rom_t* const rom_id);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OW_HDR_DEVICE_DS18x20_H */
