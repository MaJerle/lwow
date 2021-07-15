/**
 * \file            lwow.h
 * \brief           LwOW library
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
 * This file is part of LwOW - Lightweight onewire library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v3.0.2
 */
#ifndef LWOW_HDR_H
#define LWOW_HDR_H

#include <stdint.h>
#include <stddef.h>
#include "lwow/lwow_opt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup        LWOW Lightweight onewire
 * \brief           Lightweight onewire
 * \{
 *
 * \note            Functions with `_raw` suffix do no implement
 *                  locking mechanism when used with operating system.
 */

/**
 * \brief           1-Wire result enumeration
 */
typedef enum {
    lwowOK = 0x00,                              /*!< Device returned OK */
    lwowERRPRESENCE,                            /*!< Presence was not successful */
    lwowERRNODEV,                               /*!< No device connected, maybe device removed during scan? */
    lwowERRTXRX,                                /*!< Error while exchanging data */
    lwowERRBAUD,                                /*!< Error setting baudrate */
    lwowERRPAR,                                 /*!< Parameter error */
    lwowERR,                                    /*!< General-Purpose error */
} lwowr_t;

/**
 * \brief           ROM structure
 */
typedef struct {
    uint8_t rom[8];                             /*!< 8-bytes ROM address */
} lwow_rom_t;

/**
 * \defgroup        LWOW_LL Low-Level functions
 * \brief           Low-level device dependant functions
 * \{
 */

/**
 * \brief           1-Wire low-level driver structure
 */
typedef struct {
    /**
     * \brief       Initialize low-level driver
     * \param[in]   arg: Custom argument passed to \ref lwow_init function
     * \return      `1` on success, `0` otherwise
     */
    uint8_t (*init)(void* arg);

    /**
     * \brief       De-initialize low-level driver
     * \param[in]   arg: Custom argument passed to \ref lwow_init function
     * \return      `1` on success, `0` otherwise
     */
    uint8_t (*deinit)(void* arg);

    /**
     * \brief       Set UART baudrate
     * \param[in]   baud: Baudrate to set in units of bauds, normally `9600` or `115200`
     * \param[in]   arg: Custom argument passed to \ref lwow_init function
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
     * \param[in]   arg: Custom argument passed to \ref lwow_init function
     * \return      `1` on success, `0` otherwise
     */
    uint8_t (*tx_rx)(const uint8_t* tx, uint8_t* rx, size_t len, void* arg);
} lwow_ll_drv_t;

/**
 * \}
 */

/**
 * \brief           1-Wire structure
 */
typedef struct {
    lwow_rom_t rom;                             /*!< ROM address of last device found.
                                                     When searching for new devices, we always need last found address,
                                                     to be able to decide which way to go next time during scan. */
    uint8_t disrepancy;                         /*!< Disrepancy value on last search */
    void* arg;                                  /*!< User custom argument */

    const lwow_ll_drv_t* ll_drv;                /*!< Low-level functions driver */
#if LWOW_CFG_OS || __DOXYGEN__
    LWOW_CFG_OS_MUTEX_HANDLE mutex;             /*!< Mutex handle */
#endif /* LWOW_CFG_OS || __DOXYGEN__ */
} lwow_t;

/**
 * \brief           Search callback function implementation
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: Rom address when new device detected.
 *                     Set to `NULL` when search finished
 * \param[in]       index: Current device index
 *                     When `rom_id = NULL`, value indicates number of total devices found
 * \param[in]       arg: Custom user argument
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
typedef lwowr_t (*lwow_search_cb_fn) (lwow_t* const ow, const lwow_rom_t* const rom_id, size_t index, void* arg);

#define LWOW_UNUSED(x)                ((void)(x))   /*!< Unused variable macro */

/**
 * \brief           Assert check function
 *
 * It returns \ref lwowERRPAR if condition check fails
 *
 * \param[in]       msg: Optional message parameter to print on failure
 * \param[in]       c: Condition to check for
 * \hideinitializer
 */
#define LWOW_ASSERT(msg, c)           do {    \
        if (!(c)) {                             \
            return lwowERRPAR;                    \
        }                                       \
    } while (0)

/**
 * \brief           Assert check function with return `0`
 *
 * It returns `0` if condition check fails
 *
 * \param[in]       msg: Optional message parameter to print on failure
 * \param[in]       c: Condition to check for
 * \hideinitializer
 */
#define LWOW_ASSERT0(msg, c)          do {    \
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
#define LWOW_ARRAYSIZE(x)             (sizeof(x) / sizeof((x)[0]))

#define LWOW_CMD_RSCRATCHPAD          0xBE      /*!< Read scratchpad command for 1-Wire devices */
#define LWOW_CMD_WSCRATCHPAD          0x4E      /*!< Write scratchpad command for 1-Wire devices */
#define LWOW_CMD_CPYSCRATCHPAD        0x48      /*!< Copy scratchpad command for 1-Wire devices */
#define LWOW_CMD_RECEEPROM            0xB8      /*!< Read EEPROM command */
#define LWOW_CMD_RPWRSUPPLY           0xB4      /*!< Read power supply command */
#define LWOW_CMD_SEARCHROM            0xF0      /*!< Search ROM command */
#define LWOW_CMD_READROM              0x33      /*!< Read ROM command */
#define LWOW_CMD_MATCHROM             0x55      /*!< Match ROM command. Select device with specific ROM */
#define LWOW_CMD_SKIPROM              0xCC      /*!< Skip ROM, select all devices */


lwowr_t     lwow_init(lwow_t* const ow, const lwow_ll_drv_t* const ll_drv, void* arg);
void        lwow_deinit(lwow_t* const ow);

lwowr_t     lwow_protect(lwow_t* const ow, const uint8_t protect);
lwowr_t     lwow_unprotect(lwow_t* const ow, const uint8_t protect);

lwowr_t     lwow_reset_raw(lwow_t* const ow);
lwowr_t     lwow_reset(lwow_t* const ow);

lwowr_t     lwow_write_byte_ex_raw(lwow_t* const ow, const uint8_t btw, uint8_t* const br);
lwowr_t     lwow_write_byte_ex(lwow_t* const ow, const uint8_t btw, uint8_t* const br);

lwowr_t     lwow_read_byte_ex_raw(lwow_t* const ow, uint8_t* const br);
lwowr_t     lwow_read_byte_ex(lwow_t* const ow, uint8_t* const br);

lwowr_t     lwow_read_bit_ex_raw(lwow_t* const ow, uint8_t* const br);
lwowr_t     lwow_read_bit_ex(lwow_t* const ow, uint8_t* const br);

lwowr_t     lwow_search_reset_raw(lwow_t* const ow);
lwowr_t     lwow_search_reset(lwow_t* const ow);

lwowr_t     lwow_search_raw(lwow_t* const ow, lwow_rom_t* const rom_id);
lwowr_t     lwow_search(lwow_t* const ow, lwow_rom_t* const rom_id);

lwowr_t     lwow_search_with_command_raw(lwow_t* const ow, const uint8_t cmd, lwow_rom_t* const rom_id);
lwowr_t     lwow_search_with_command(lwow_t* const ow, const uint8_t cmd, lwow_rom_t* const rom_id);

lwowr_t     lwow_search_with_command_callback(lwow_t* const ow, const uint8_t cmd, size_t* const roms_found, const lwow_search_cb_fn func, void* const arg);
lwowr_t     lwow_search_with_callback(lwow_t* const ow, size_t* const roms_found, const lwow_search_cb_fn func, void* const arg);

lwowr_t     lwow_search_devices_with_command_raw(lwow_t* const ow, const uint8_t cmd, lwow_rom_t* const rom_id_arr, const size_t rom_len, size_t* const roms_found);
lwowr_t     lwow_search_devices_with_command(lwow_t* const ow, const uint8_t cmd, lwow_rom_t* const rom_id_arr, const size_t rom_len, size_t* const roms_found);

lwowr_t     lwow_search_devices_raw(lwow_t* const ow, lwow_rom_t* const rom_id_arr, const size_t rom_len, size_t* const roms_found);
lwowr_t     lwow_search_devices(lwow_t* const ow, lwow_rom_t* const rom_id_arr, const size_t rom_len, size_t* const roms_found);

lwowr_t     lwow_match_rom_raw(lwow_t* const ow, const lwow_rom_t* const rom_id);
lwowr_t     lwow_match_rom(lwow_t* const ow, const lwow_rom_t* const rom_id);

lwowr_t     lwow_skip_rom_raw(lwow_t* const ow);
lwowr_t     lwow_skip_rom(lwow_t* const ow);

uint8_t     lwow_crc(const void* const in, const size_t len);

/* Legacy functions, deprecated, to be removed in next major release */
uint8_t     lwow_write_byte_raw(lwow_t* const ow, const uint8_t b);
uint8_t     lwow_write_byte(lwow_t* const ow, const uint8_t b);
uint8_t     lwow_read_byte_raw(lwow_t* const ow);
uint8_t     lwow_read_byte(lwow_t* const ow);
uint8_t     lwow_read_bit_raw(lwow_t* const ow);
uint8_t     lwow_read_bit(lwow_t* const ow);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWOW_HDR_H */
