/**
 * \file            lwow_device_ds18x20.h
 * \brief           DS18x20 driver header
 */

/*
 * Copyright (c) 2024 Tilen MAJERLE
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
 * This file is part of LwOW - Lightweight onewire library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v3.0.2
 */
#ifndef LWOW_DEVICE_DS18x20_HDR_H
#define LWOW_DEVICE_DS18x20_HDR_H

#include "lwow/lwow.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup         LWOW_DEVICES
 * \defgroup        LWOW_DEVICE_DS18x20 DS18x20 temperature sensor
 * \brief           Device driver for DS18x20 temperature sensor
 * \{
 *
 * \note            Functions with `_raw` suffix do no implement
 *                  locking mechanism when used with operating system.
 */

#define LWOW_DS18X20_ALARM_DISABLE      ((int8_t)-128) /*!< Disable alarm temperature */
#define LWOW_DS18X20_ALARM_NOCHANGE     ((int8_t)-127) /*!< Do not modify current alarm settings */
#define LWOW_DS18X20_TEMP_MIN           ((int8_t)-55)  /*!< Minimum temperature */
#define LWOW_DS18X20_TEMP_MAX           ((int8_t)125)  /*!< Maximal temperature */
#define LWOW_DS18X20_CMD_ALARM_SEARCH   0xEC           /*!< Alarm Search Command */
#define LWOW_DS18X20_CMD_CONVERT_T      0x44           /*!< Convert T Command */

uint8_t lwow_ds18x20_start_raw(lwow_t* const owobj, const lwow_rom_t* const rom_id);
uint8_t lwow_ds18x20_start(lwow_t* const owobj, const lwow_rom_t* const rom_id);

uint8_t lwow_ds18x20_read_raw(lwow_t* const owobj, const lwow_rom_t* const rom_id, float* const temp_out);
uint8_t lwow_ds18x20_read(lwow_t* const owobj, const lwow_rom_t* const rom_id, float* const temp_out);

uint8_t lwow_ds18x20_set_resolution_raw(lwow_t* const owobj, const lwow_rom_t* const rom_id, const uint8_t bits);
uint8_t lwow_ds18x20_set_resolution(lwow_t* const owobj, const lwow_rom_t* const rom_id, const uint8_t bits);

uint8_t lwow_ds18x20_get_resolution_raw(lwow_t* const owobj, const lwow_rom_t* const rom_id);
uint8_t lwow_ds18x20_get_resolution(lwow_t* const owobj, const lwow_rom_t* const rom_id);

uint8_t lwow_ds18x20_set_alarm_temp_raw(lwow_t* const owobj, const lwow_rom_t* const rom_id, int8_t temp_l,
                                        int8_t temp_h);
uint8_t lwow_ds18x20_set_alarm_temp(lwow_t* const owobj, const lwow_rom_t* const rom_id, int8_t temp_l, int8_t temp_h);

uint8_t lwow_ds18x20_get_alarm_temp_raw(lwow_t* const owobj, const lwow_rom_t* const rom_id, int8_t* temp_l,
                                        int8_t* temp_h);
uint8_t lwow_ds18x20_get_alarm_temp(lwow_t* const owobj, const lwow_rom_t* const rom_id, int8_t* temp_l,
                                    int8_t* temp_h);

lwowr_t lwow_ds18x20_search_alarm_raw(lwow_t* const owobj, lwow_rom_t* const rom_id);
lwowr_t lwow_ds18x20_search_alarm(lwow_t* const owobj, lwow_rom_t* const rom_id);

uint8_t lwow_ds18x20_is_b(lwow_t* const owobj, const lwow_rom_t* const rom_id);
uint8_t lwow_ds18x20_is_s(lwow_t* const owobj, const lwow_rom_t* const rom_id);

uint16_t lwow_ds18x20_get_temp_conversion_time(uint8_t resolution, uint8_t is_b);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWOW_DEVICE_DS18x20_HDR_H */
