/**
 * \file            ow.h
 * \brief           OneWire-UART library
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
#ifndef OW_HDR_H
#define OW_HDR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stddef.h>

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
    owPARERR = -3,                              /*!< Parameter error */
    owERR,                                      /*!< General-Purpose error */
} owr_t;

/**
 * \brief           ROM structure
 */
typedef struct {
    uint8_t rom[8];                             /*!< 8-bytes ROM address */
} ow_rom_t;

/**
 * \defgroup        OW_LL Low-Level functions
 * \brief           Low-level device dependant functions
 * \{
 */

/**
 * \brief           1-Wire low-level driver structure
 */
typedef struct {
    /**
     * \brief       Initialize low-level driver
     * \param[in]   arg: Custom argument passed to \ref ow_init function
     * \return      `1` on success, `0` otherwise
     */
    uint8_t (*init)(void* arg);

    /**
     * \brief       De-initialize low-level driver
     * \param[in]   arg: Custom argument passed to \ref ow_init function
     * \return      `1` on success, `0` otherwise
     */
    uint8_t (*deinit)(void* arg);

    /**
     * \brief       Set UART baudrate
     * \param[in]   baud: Baudrate to set in units of bauds, normally `9600` or `115200`
     * \param[in]   arg: Custom argument passed to \ref ow_init function
     * \return      `1` on success, `0` otherwise
     */
    uint8_t (*set_baudrate)(uint32_t baud, void* arg);

    /**
     * \brief       Transmit and receive bytes over UART hardware (or custom implementation)
     *
     * Bytes array for `tx` is already prepared to be directly transmitted over UART hardware,
     * no data manipulation is necessary.
     *
     * At the same time, library must read received data on RX port and put it to `rx` data array,
     * one by one, up to `len` number of bytes
     *
     * \param[in]   tx: Data to transmit over UART
     * \param[out]  rx: Array to write received data to
     * \param[in]   len: Number of bytes to exchange
     * \param[in]   arg: Custom argument passed to \ref ow_init function
     * \return      `1` on success, `0` otherwise
     */
    uint8_t (*tx_rx)(const uint8_t* tx, uint8_t* rx, size_t len, void* arg);
} ow_ll_drv_t;

/**
 * \}
 */

/**
 * \defgroup        OW_SYS System functions
 * \brief           System functions when used with operating system
 * \{
 */

uint8_t ow_sys_mutex_create(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg);
uint8_t ow_sys_mutex_delete(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg);
uint8_t ow_sys_mutex_wait(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg);
uint8_t ow_sys_mutex_release(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg);

/**
 * \}
 */

/**
 * \brief           1-Wire structure
 */
typedef struct {
    ow_rom_t rom;                               /*!< ROM address of last device found.
                                                     When searching for new devices, we always need last found address,
                                                     to be able to decide which way to go next time during scan. */
    uint8_t disrepancy;                         /*!< Disrepancy value on last search */
    void* arg;                                  /*!< User custom argument */

    const ow_ll_drv_t* ll_drv;                  /*!< Low-level functions driver */
#if OW_CFG_OS || __DOXYGEN__
    OW_CFG_OS_MUTEX_HANDLE mutex;               /*!< Mutex handle */
#endif /* OW_CFG_OS || __DOXYGEN__ */
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
typedef owr_t (*ow_search_cb_fn) (ow_t* const ow, const ow_rom_t* const rom_id, size_t index, void* arg);

#define OW_UNUSED(x)                ((void)(x)) /*!< Unused variable macro */

/**
 * \brief           Assert check function
 */
#define OW_ASSERT(msg, c)           do {    \
    if (!(c)) {                             \
        return owPARERR;                    \
    }                                       \
} while (0)

/**
 * \brief           Assert check function with return `0`
 */
#define OW_ASSERT0(msg, c)          do {    \
    if (!(c)) {                             \
        return 0;                           \
    }                                       \
} while (0)

/**
 * \brief           Get size of statically declared array
 * \param[in]       x: Input array
 * \return          Number of array elements
 * \hideinitializer
 */
#define OW_ARRAYSIZE(x)             (sizeof(x) / sizeof((x)[0]))

#define OW_CMD_RSCRATCHPAD          0xBE        /*!< Read scratchpad command for 1-Wire devices */
#define OW_CMD_WSCRATCHPAD          0x4E        /*!< Write scratchpad command for 1-Wire devices */
#define OW_CMD_CPYSCRATCHPAD        0x48        /*!< Copy scratchpad command for 1-Wire devices */
#define OW_CMD_RECEEPROM            0xB8        /*!< Read EEPROM command */
#define OW_CMD_RPWRSUPPLY           0xB4        /*!< Read power supply command */
#define OW_CMD_SEARCHROM            0xF0        /*!< Search ROM command */
#define OW_CMD_READROM              0x33        /*!< Read ROM command */
#define OW_CMD_MATCHROM             0x55        /*!< Match ROM command. Select device with specific ROM */
#define OW_CMD_SKIPROM              0xCC        /*!< Skip ROM, select all devices */


owr_t       ow_init(ow_t* const ow, const ow_ll_drv_t* const ll_drv, void* arg);
void        ow_deinit(ow_t* const ow);

owr_t       ow_protect(ow_t* const ow, const uint8_t protect);
owr_t       ow_unprotect(ow_t* const ow, const uint8_t protect);


owr_t       ow_reset_raw(ow_t* const ow);
owr_t       ow_reset(ow_t* const ow);

uint8_t     ow_write_byte_raw(ow_t* const ow, const uint8_t b);
uint8_t     ow_write_byte(ow_t* const ow, const uint8_t b);

uint8_t     ow_read_byte_raw(ow_t* const ow);
uint8_t     ow_read_byte(ow_t* const ow);

uint8_t     ow_read_bit_raw(ow_t* const ow);
uint8_t     ow_read_bit(ow_t* const ow);


owr_t       ow_search_reset_raw(ow_t* const ow);
owr_t       ow_search_reset(ow_t* const ow);

owr_t       ow_search_raw(ow_t* const ow, ow_rom_t* const rom_id);
owr_t       ow_search(ow_t* const ow, ow_rom_t* const rom_id);

owr_t       ow_search_with_command_raw(ow_t* const ow, const uint8_t cmd, ow_rom_t* const rom_id);
owr_t       ow_search_with_command(ow_t* const ow, const uint8_t cmd, ow_rom_t* const rom_id);

owr_t       ow_search_with_command_callback(ow_t* const ow, const uint8_t cmd, size_t* const roms_found, const ow_search_cb_fn func, void* const arg);
owr_t       ow_search_with_callback(ow_t* const ow, size_t* const roms_found, const ow_search_cb_fn func, void* const arg);

owr_t       ow_search_devices_with_command_raw(ow_t* const ow, const uint8_t cmd, ow_rom_t* const rom_id_arr, const size_t rom_len, size_t* const roms_found);
owr_t       ow_search_devices_with_command(ow_t* const ow, const uint8_t cmd, ow_rom_t* const rom_id_arr, const size_t rom_len, size_t* const roms_found);

owr_t       ow_search_devices_raw(ow_t* const ow, ow_rom_t* const rom_id_arr, const size_t rom_len, size_t* const roms_found);
owr_t       ow_search_devices(ow_t* const ow, ow_rom_t* const rom_id_arr, const size_t rom_len, size_t* const roms_found);

uint8_t     ow_match_rom_raw(ow_t* const ow, const ow_rom_t* const rom_id);
uint8_t     ow_match_rom(ow_t* const ow, const ow_rom_t* const rom_id);

uint8_t     ow_skip_rom_raw(ow_t* const ow);
uint8_t     ow_skip_rom(ow_t* const ow);

uint8_t     ow_crc(const void* const in, const size_t len);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OW_HDR_H */
