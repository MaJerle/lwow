/**
 * \file            ow.h
 * \brief           OneWire-UART library
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
 * This file is part of OneWire-UART library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#ifndef __ONEWIRE_H
#define __ONEWIRE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stdint.h"
#include "stddef.h"

#include "ow_config.h"

/**
 * \defgroup        OW API functions
 * \brief           OneWire API
 * \{
 *
 * \note            Functions with `_raw` suffix do no implement
 *                  locking mechanism when used with operating system.
 */

/**
 * \brief           1-Wire result enumeration
 */
typedef enum {
    owOK = 0x00,                                /*!< Device returned OK */
    owERRPRESENCE = -1,                         /*!< Presence was not successful */
    owERRNODEV = -2,                            /*!< No device connected, maybe device removed during scan? */
    owERR,                                      /*!< General-Purpose error */
} owr_t;

/**
 * \brief           ROM structure
 */
typedef struct {
    uint8_t rom[8];                             /*!< 8-bytes ROM address */
} ow_rom_t;

/**
 * \brief           1-Wire structure
 */
typedef struct {    
#if OW_CFG_OS || __DOXYGEN__
    OW_CFG_OS_MUTEX_HANDLE mutex;               /*!< Mutex handle */
#endif /* OW_USE_RTOS || __DOXYGEN__ */
    
    ow_rom_t rom;                               /*!< ROM address of last device found.
                                                     When searching for new devices, we always need last found address,
                                                     to be able to decide which way to go next time during scan. */
    uint8_t disrepancy;                         /*!< Disrepancy value on last search */
	void* arg;                                  /*!< User custom argument */
} ow_t;

/**
 * \brief           Search callback function implementation
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: Rom address when new device detected.
 *                     Set to `NULL` when search finished
 * \param[in]       index: Current device index
 *                     When `rom_id = NULL`, value indicates number of total devices found
 * \param[in]       arg: Custom user argument
 * \return          \ref owOK on success, member of \ref owr_t otherwise
 */
typedef owr_t (*ow_search_cb_fn) (ow_t* ow, const ow_rom_t* rom_id, size_t index, void* arg);

#define OW_UNUSED(x)                ((void)(x)) /*!< Unused variable macro */
    
#define OW_CMD_RSCRATCHPAD          0xBE        /*!< Read scratchpad command for 1-Wire devices */
#define OW_CMD_WSCRATCHPAD          0x4E        /*!< Write scratchpad command for 1-Wire devices */
#define OW_CMD_CPYSCRATCHPAD        0x48        /*!< Copy scratchpad command for 1-Wire devices */
#define OW_CMD_RECEEPROM            0xB8
#define OW_CMD_RPWRSUPPLY           0xB4
#define OW_CMD_SEARCHROM            0xF0        /*!< Search ROM command */
#define OW_CMD_READROM              0x33        /*!< Read ROM command */
#define OW_CMD_MATCHROM             0x55        /*!< Match ROM command. Select device with specific ROM */
#define OW_CMD_SKIPROM              0xCC        /*!< Skip ROM, select all devices */

owr_t       ow_init(ow_t* ow, void* arg);

owr_t       ow_protect(ow_t* ow, const uint8_t protect);
owr_t       ow_unprotect(ow_t* ow, const uint8_t protect);


owr_t       ow_reset_raw(ow_t* ow);
owr_t       ow_reset(ow_t* ow);

uint8_t     ow_write_byte_raw(ow_t* ow, uint8_t b);
uint8_t     ow_write_byte(ow_t* ow, uint8_t b);

uint8_t     ow_read_byte_raw(ow_t* ow);
uint8_t     ow_read_byte(ow_t* ow);

uint8_t     ow_read_bit_raw(ow_t* ow);
uint8_t     ow_read_bit(ow_t* ow);


owr_t       ow_search_reset_raw(ow_t* ow);
owr_t       ow_search_reset(ow_t* ow);

owr_t       ow_search_raw(ow_t* ow, ow_rom_t* rom_id);
owr_t       ow_search(ow_t* ow, ow_rom_t* rom_id);

owr_t       ow_search_with_command_raw(ow_t* ow, uint8_t cmd, ow_rom_t* rom_id);
owr_t       ow_search_with_command(ow_t* ow, uint8_t cmd, ow_rom_t* rom_id);

owr_t		ow_search_with_command_callback(ow_t* ow, uint8_t cmd, size_t* found, ow_search_cb_fn func, void* arg);
owr_t		ow_search_with_callback(ow_t* ow, size_t* found, ow_search_cb_fn func, void* arg);

owr_t       ow_search_devices_with_command_raw(ow_t* ow, uint8_t cmd, ow_rom_t* rom_arr, size_t rom_len, size_t* found);
owr_t       ow_search_devices_with_command(ow_t* ow, uint8_t cmd, ow_rom_t* rom_arr, size_t rom_len, size_t* found);

owr_t       ow_search_devices_raw(ow_t* ow, ow_rom_t* rom_arr, size_t rom_len, size_t* found);
owr_t       ow_search_devices(ow_t* ow, ow_rom_t* rom_arr, size_t rom_len, size_t* found);

uint8_t     ow_match_rom_raw(ow_t* ow, const ow_rom_t* rom_id);
uint8_t     ow_match_rom(ow_t* ow, const ow_rom_t* rom_id);

uint8_t     ow_skip_rom_raw(ow_t* ow);
uint8_t     ow_skip_rom(ow_t* ow);

uint8_t     ow_crc_raw(const void* in, size_t len);
uint8_t     ow_crc(const void* in, size_t len);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ONEWIRE_H__ */
