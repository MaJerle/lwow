/**
 * \file            ow_device_ds2430.h
 * \brief           DS2430 driver header
 */

/*
 * Copyright (c) 2019 Tilen MAJERLE
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
 * Author:          Márcio Figueira <mvfpoa@gmail.com>
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
*/
#ifndef OW_HDR_DEVICE_DS2430_H
#define OW_HDR_DEVICE_DS2430_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ow/ow.h"

/**
 * \ingroup         OW_DEVICES
 * \defgroup        OW_DEVICE_DS2430 256bit EEPROM
 * \brief           Device driver for DS2430 256bit EEPROM
 * \{
 *
 * \note            Functions with `_raw` suffix do no implement
 *                  locking mechanism when using with operating system.
 */

#define OW_DS2430_WR_SCRATCHPAD             ((uint8_t)0x0F)  /*!< Data Memory Write 256bit scratchpad */
#define OW_DS2430_RD_SCRATCHPAD             ((uint8_t)0xAA)  /*!< Data Memory Read  256bit scratchpad */
#define OW_DS2430_CP_SCRATCHPAD             ((uint8_t)0x55)  /*!< Data Memory Copy  256bit scratchpad to EEPROM */
#define OW_DS2430_RD_MEMORY                 ((uint8_t)0xF0)  /*!< Data Memory Read  256bit EEPROM and copy to scratchpad */
#define OW_DS2430_RD_STATUS                 ((uint8_t)0x66)  /*!< Data Memory Read  8bit status register */
#define OW_DS2430_WR_APP_REGISTER           ((uint8_t)0x99)  /*!< Data Memory Write 64bit app register scratchpad */
#define OW_DS2430_RD_APP_REGISTER           ((uint8_t)0xC3)  /*!< Data Memory read  64bit app register memory/scratchpad */
#define OW_DS2430_CPLK_APP_REGISTER         ((uint8_t)0x5A)  /*!< Data Memory copy  64bit scratchpad to memory and perpetually lock memory */

#define OW_DS2430_CP_VALIDATION_KEY			((uint8_t)0xA5)  /*!< Data Memory Copy operations requires validation key */
#define OW_DS2430_RD_STS_VALIDATION_KEY     ((uint8_t)0x00)  /*!< Data Memory Read status requires validation key */

#define OW_DS2430_STORAGE_LENGTH	 	    ((uint8_t)0x20)  /*!< Data Memory Read scratchpad memory length */
#define OW_DS2430_APP_REG_LENGTH	 	    ((uint8_t)0x08)  /*!< Data Memory Read app register length */

owr_t   ow_ds2430_storage_write_scratchpad_raw(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, const uint8_t* reg, uint8_t length);
owr_t   ow_ds2430_storage_write_scratchpad(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, const uint8_t* reg, uint8_t length);

owr_t   ow_ds2430_storage_read_scratchpad_raw(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, uint8_t* reg, uint8_t length);
owr_t   ow_ds2430_storage_read_scratchpad(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, uint8_t* reg, uint8_t length);

owr_t   ow_ds2430_storage_copy_from_scratchpad_raw(ow_t* ow, const ow_rom_t* rom_id, void (*delay10ms_fn)());
owr_t   ow_ds2430_storage_copy_from_scratchpad(ow_t* ow, const ow_rom_t* rom_id, void (*delay10ms_fn)());

owr_t   ow_ds2430_storage_read_raw(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, uint8_t* reg, uint8_t length);
owr_t   ow_ds2430_storage_read(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, uint8_t* reg, uint8_t length);

owr_t   ow_ds2430_app_register_write_scratchpad_raw(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, const uint8_t* reg, uint8_t length);
owr_t   ow_ds2430_app_register_write_scratchpad(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, const uint8_t* reg, uint8_t length);

owr_t   ow_ds2430_app_register_read_raw(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, uint8_t* reg, uint8_t length);
owr_t   ow_ds2430_app_register_read(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, uint8_t* reg, uint8_t length);

owr_t   ow_ds2430_app_register_copylock_from_scratchpad_raw(ow_t* ow, const ow_rom_t* rom_id, void (*delay10ms_fn)());
owr_t   ow_ds2430_app_register_copylock_from_scratchpad(ow_t* ow, const ow_rom_t* rom_id, void (*delay10ms_fn)());

owr_t	ow_ds2430_status_read_raw(ow_t* ow, const ow_rom_t* rom_id, uint8_t* status);
owr_t	ow_ds2430_status_read(ow_t* ow, const ow_rom_t* rom_id, uint8_t* status);

owr_t	ow_ds2430_app_register_is_locked_raw(ow_t* ow, const ow_rom_t* rom_id, uint8_t* locked);
owr_t	ow_ds2430_app_register_is_locked(ow_t* ow, const ow_rom_t* rom_id, uint8_t* locked);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OW_HDR_DEVICE_DS2430_H */
