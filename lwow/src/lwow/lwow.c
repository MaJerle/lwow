/**
 * \file            lwow.c
 * \brief           OneWire protocol implementation
 */

/*
 * Copyright (c) 2023 Tilen MAJERLE
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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lwow/lwow.h"
#include "system/lwow_sys.h"

#if !__DOXYGEN__

/* Internal macros */
#define OW_FIRST_DEV  0xFF
#define OW_LAST_DEV   0x00

#define OW_RESET_BYTE 0xF0

#endif /* !__DOXYGEN__ */

/* Set value if not NULL */
#define SET_NOT_NULL(p, v)                                                                                             \
    if ((p) != NULL) {                                                                                                 \
        *(p) = (v);                                                                                                    \
    }

/**
 * \brief           Send single bit to OneWire port
 * \param[in]       owobj: OneWire instance
 * \param[in]       btw: Bit to send, either `1` or `0`
 * \param[out]      btr: Pointer to output variable to write read bit
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
static lwowr_t
prv_send_bit(lwow_t* const owobj, uint8_t btw, uint8_t* btr) {
    uint8_t byt = 0;

    SET_NOT_NULL(btr, 0);

    /*
     * To send logical 1 over 1-wire, send 0xFF over UART
     * To send logical 0 over 1-wire, send 0x00 over UART
     */
    btw = btw > 0 ? 0xFFU : 0x00U; /* Convert to 0 or 1 */
    if (!owobj->ll_drv->tx_rx(&btw, &byt, 1U, owobj->arg)) {
        return lwowERRTXRX; /* Transmit error */
    }
    byt = byt == 0xFFU ? 1U : 0U; /* Go to bit values */
    SET_NOT_NULL(btr, byt);       /* Set new byte */
    return lwowOK;
}

/**
 * \brief           Initialize OneWire instance
 * \param[in]       owobj: OneWire instance
 * \param[in]       ll_drv: Low-level driver
 * \param[in]       arg: Custom argument
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_init(lwow_t* const owobj, const lwow_ll_drv_t* const ll_drv, void* arg) {
    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    LWOW_ASSERT("ll_drv != NULL", ll_drv != NULL);
    LWOW_ASSERT("ll_drv->init != NULL", ll_drv->init != NULL);
    LWOW_ASSERT("ll_drv->deinit != NULL", ll_drv->deinit != NULL);
    LWOW_ASSERT("ll_drv->set_baudrate != NULL", ll_drv->set_baudrate != NULL);
    LWOW_ASSERT("ll_drv->tx_rx != NULL", ll_drv->tx_rx != NULL);

    owobj->arg = arg;
    owobj->ll_drv = ll_drv;                 /* Assign low-level driver */
    if (!owobj->ll_drv->init(owobj->arg)) { /* Init low-level directly */
        return lwowERR;
    }
#if LWOW_CFG_OS
    if (!lwow_sys_mutex_create(&owobj->mutex, arg)) {
        owobj->ll_drv->deinit(owobj->arg); /* Deinit low-level */
        return lwowERR;
    }
#endif /* LWOW_CFG_OS */
    return lwowOK;
}

/**
 * \brief           Deinitialize OneWire instance
 * \param[in]       owobj: OneWire instance
 */
void
lwow_deinit(lwow_t* const owobj) {
    if (owobj == NULL || owobj->ll_drv == NULL) {
        return;
    }

#if LWOW_CFG_OS
    lwow_sys_mutex_delete(&owobj->mutex, owobj->arg);
#endif /* LWOW_CFG_OS */
    owobj->ll_drv->deinit(owobj->arg);
}

/**
 * \brief           Protect 1-wire from concurrent access
 * \note            Used only for OS systems
 * \param[in,out]   owobj: 1-Wire handle
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_protect(lwow_t* const owobj, const uint8_t protect) {
    LWOW_ASSERT("owobj != NULL", owobj != NULL);

#if LWOW_CFG_OS
    if (protect && !lwow_sys_mutex_wait(&owobj->mutex, owobj->arg)) {
        return lwowERR;
    }
#else
    (void)ow;
    (void)protect;
#endif /* LWOW_CFG_OS */
    return lwowOK;
}

/**
 * \brief           Unprotect 1-wire from concurrent access
 * \note            Used only for OS systems
 * \param[in,out]   owobj: 1-Wire handle
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_unprotect(lwow_t* const owobj, const uint8_t protect) {
    LWOW_ASSERT("owobj != NULL", owobj != NULL);

#if LWOW_CFG_OS
    if (protect && !lwow_sys_mutex_release(&owobj->mutex, owobj->arg)) {
        return lwowERR;
    }
#else
    (void)ow;
    (void)protect;
#endif /* LWOW_CFG_OS */
    return lwowOK;
}

/**
 * \brief           Reset 1-Wire bus and set connected devices to idle state
 * \param[in,out]   owobj: 1-Wire handle
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_reset_raw(lwow_t* const owobj) {
    uint8_t byt = 0;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);

    /* First send reset pulse */
    byt = OW_RESET_BYTE; /* Set reset sequence byte = 0xF0 */
    if (!owobj->ll_drv->set_baudrate(9600U, owobj->arg)) {
        return lwowERRBAUD; /* Error setting baudrate */
    }
    if (!owobj->ll_drv->tx_rx(&byt, &byt, 1U, owobj->arg)) {
        return lwowERRTXRX; /* Error with data exchange */
    }
    if (!owobj->ll_drv->set_baudrate(115200U, owobj->arg)) {
        return lwowERRBAUD; /* Error setting baudrate */
    }

    /* Check if there is reply from any device */
    if (byt == 0 || byt == OW_RESET_BYTE) {
        return lwowERRPRESENCE;
    }
    return lwowOK;
}

/**
 * \copydoc         lwow_reset_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_reset(lwow_t* const owobj) {
    lwowr_t res = lwowERR;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);

    lwow_protect(owobj, 1U);
    res = lwow_reset_raw(owobj);
    lwow_unprotect(owobj, 1U);
    return res;
}

/**
 * \brief           Write byte over OW and read its response
 * \param[in,out]   owobj: 1-Wire handle
 * \param[in]       btw: Byte to write
 * \param[out]      byr: Pointer to read value. Set to `NULL` if not used
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_write_byte_ex_raw(lwow_t* const owobj, const uint8_t btw, uint8_t* const byr) {
    uint8_t trx[8];

    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    SET_NOT_NULL(byr, 0);

    /* Prepare output data */
    for (uint8_t i = 0; i < 8U; ++i) {
        /*
         * If we have to send high bit, set byte as 0xFF,
         * otherwise set it as low bit, 0x00
         */
        trx[i] = (btw & (1U << i)) ? 0xFFU : 0x00U;
    }

    /*
     * Exchange data on UART level,
     * send single byte for each bit = 8 bytes
     */
    if (!owobj->ll_drv->tx_rx(trx, trx, 8U, owobj->arg)) {
        return lwowERRTXRX;
    }

    /* Update output value */
    if (byr != NULL) {
        uint8_t tmp = 0U;

        /*
         * Check received data. If we read 0xFF,
         * our logical write 1 was successful, otherwise it was 0.
         */
        for (uint8_t idx = 0; idx < 8U; ++idx) {
            if (trx[idx] == 0xFFU) {
                tmp |= 0x01U << idx;
            }
        }
        *byr = tmp;
    }
    return lwowOK;
}

/**
 * \copydoc         lwow_write_byte_ex_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_write_byte_ex(lwow_t* const owobj, const uint8_t btw, uint8_t* const byr) {
    lwowr_t res = lwowERR;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);

    lwow_protect(owobj, 1U);
    res = lwow_write_byte_ex_raw(owobj, btw, byr);
    lwow_unprotect(owobj, 1U);
    return res;
}

/**
 * \brief           Read byte from OW device
 * \param[in,out]   owobj: 1-Wire handle
 * \param[out]      byr: Pointer to save read value
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_read_byte_ex_raw(lwow_t* const owobj, uint8_t* const byr) {
    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    LWOW_ASSERT("byr != NULL", byr != NULL);

    /*
     * When we want to read byte over 1-Wire,
     * we have to send all bits as 1 and check if slave pulls line down.
     *
     * According to slave reactions, we can later construct received bytes
     */
    return lwow_write_byte_ex_raw(owobj, 0xFFU, byr);
}

/**
 * \copydoc         lwow_read_byte_ex_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_read_byte_ex(lwow_t* const owobj, uint8_t* const byr) {
    lwowr_t res = lwowERR;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    LWOW_ASSERT("byr != NULL", byr != NULL);

    lwow_protect(owobj, 1U);
    res = lwow_read_byte_ex_raw(owobj, byr);
    lwow_unprotect(owobj, 1U);
    return res;
}

/**
 * \brief           Read sinle bit from OW device
 * \param[in,out]   owobj: 1-Wire handle
 * \param[out]      br: Pointer to save read value, either `1` or `0`
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_read_bit_ex_raw(lwow_t* const owobj, uint8_t* const byr) {
    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    LWOW_ASSERT("byr != NULL", byr != NULL);

    /* Send bit as `1` and read the response */
    return prv_send_bit(owobj, 1, byr);
}

/**
 * \copydoc         lwow_read_bit_ex_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_read_bit_ex(lwow_t* const owobj, uint8_t* const byr) {
    lwowr_t res = lwowERR;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    LWOW_ASSERT("byr != NULL", byr != NULL);

    lwow_protect(owobj, 1U);
    res = lwow_read_bit_ex_raw(owobj, byr);
    lwow_unprotect(owobj, 1U);
    return res;
}

/**
 * \brief           Reset search
 * \param[in,out]   owobj: 1-Wire handle
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_search_reset_raw(lwow_t* const owobj) {
    LWOW_ASSERT("owobj != NULL", owobj != NULL);

    owobj->disrepancy = OW_FIRST_DEV; /* Reset disrepancy to default value */
    return lwowOK;
}

/**
 * \copydoc         lwow_search_reset_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_search_reset(lwow_t* const owobj) {
    lwowr_t res = lwowERR;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);

    lwow_protect(owobj, 1U);
    res = lwow_search_reset_raw(owobj);
    lwow_unprotect(owobj, 1U);
    return res;
}

/**
 * \brief           Search for devices on 1-wire bus
 * \note            To reset search and to start over, use \ref lwow_search_reset function
 * \param[in,out]   owobj: 1-Wire handle
 * \param[out]      rom_id: Pointer to ROM structure to save ROM
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_search_raw(lwow_t* const owobj, lwow_rom_t* const rom_id) {
    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    return lwow_search_with_command_raw(owobj, LWOW_CMD_SEARCHROM, rom_id);
}

/**
 * \copydoc         lwow_search_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_search(lwow_t* const owobj, lwow_rom_t* const rom_id) {
    lwowr_t res = lwowERR;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);

    lwow_protect(owobj, 1U);
    res = lwow_search_raw(owobj, rom_id);
    lwow_unprotect(owobj, 1U);
    return res;
}

/**
 * \brief           Search for devices on 1-wire bus with custom search command
 * \note            To reset search and to start over, use \ref lwow_search_reset function
 * \param[in,out]   owobj: 1-Wire handle
 * \param[in]       cmd: command to use for search operation
 * \param[out]      rom_id: Pointer to ROM structure to store address
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_search_with_command_raw(lwow_t* const owobj, const uint8_t cmd, lwow_rom_t* const rom_id) {
    lwowr_t res = lwowERR;
    uint8_t id_bit_number = 0, next_disrepancy = 0, *idd = NULL;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    LWOW_ASSERT("rom_id != NULL", rom_id != NULL);

    idd = owobj->rom.rom;

    /* Check for last device */
    if (owobj->disrepancy == 0) {
        lwow_search_reset_raw(owobj); /* Reset search for next search */
        return lwowERRNODEV;          /* No devices anymore */
    }

    /* Step 1: Reset all devices on 1-Wire line to be able to listen for new command */
    res = lwow_reset_raw(owobj);
    if (res != lwowOK) {
        return res;
    }

    /* Step 2: Send search rom command for all devices on 1-Wire */
    lwow_write_byte_ex_raw(owobj, cmd, NULL); /* Start with search ROM command */
    next_disrepancy = OW_LAST_DEV;            /* This is currently last device */

    for (id_bit_number = 64U; id_bit_number > 0U;) {
        uint8_t bit = 0, b_cpl = 0;

        for (uint8_t jdx = 8; jdx > 0; --jdx, --id_bit_number) {
            /* Read first bit and its complimentary one */
            if (prv_send_bit(owobj, 1, &bit) != lwowOK || prv_send_bit(owobj, 1U, &b_cpl) != lwowOK) {
                return lwowERRTXRX;
            }

            /*
             * If we have connected many devices on 1-Wire port, b and b_cpl are ANDed between all devices.
             *
             * We have to react if b and b_cpl are the same:
             *
             *  - Both 1: No devices on 1-Wire line responded
             *      - No device connected at all
             *      - All devices were put to block state due to search
             *  - Both 0: We have "collision" as device with bit 0 and bit 1 are connected
             *
             * If b and b_cpl are different, it means we have:
             *
             *  - Single device connected on 1-Wire or
             *  - All devices on 1-Wire have the same bit value at current position
             *      - In this case, we move to direction of b value
             */
            if (bit && b_cpl) {
                goto out; /* We do not have device connected */
            } else if (!bit && !b_cpl) {
                /*
                 * Decide which way to go for next scan
                 *
                 * Force move to "1" in case of:
                 *
                 *  - known diff position is larger than current bit reading
                 *  - Previous ROM address bit 0 was 1 and known diff is different than reading
                 *
                 * Because we shift *id variable down by 1 bit every iteration,
                 * *id & 0x01 always returns 1 if bit on previous ROM is the same as current bit
                 */
                if (id_bit_number < owobj->disrepancy || ((*idd & 0x01U) && owobj->disrepancy != id_bit_number)) {
                    bit = 1;
                    next_disrepancy = id_bit_number;
                }
            }

            /*
             * Devices are expecting master will send bit value back.
             * All devices which do not have this bit value
             * will go to blocked state and will wait for next reset sequence
             *
             * In case of "collision", we decide here which devices we will
             * continue to scan (binary tree)
             */
            prv_send_bit(owobj, bit, NULL);

            /*
             * Because we shift down *id each iteration, we have to position bit value to the MSB position
             * and it will be automatically positioned correct way.
             */
            *idd = (uint8_t)(*idd >> 0x01U) | (uint8_t)(bit << 0x07U);
        }
        ++idd; /* Go to next byte */
    }
out:
    owobj->disrepancy = next_disrepancy;                              /* Save disrepancy value */
    LWOW_MEMCPY(rom_id->rom, owobj->rom.rom, sizeof(owobj->rom.rom)); /* Copy ROM to user memory */
    return id_bit_number == 0 ? lwowOK : lwowERRNODEV;                /* Return search result status */
}

/**
 * \copydoc         lwow_search_with_command_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_search_with_command(lwow_t* const owobj, const uint8_t cmd, lwow_rom_t* const rom_id) {
    lwowr_t res = lwowERR;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    LWOW_ASSERT("rom_id != NULL", rom_id != NULL);

    lwow_protect(owobj, 1U);
    res = lwow_search_with_command_raw(owobj, cmd, rom_id);
    lwow_unprotect(owobj, 1U);
    return res;
}

/**
 * \brief           Select device on 1-wire network with exact ROM number
 * \param[in]       owobj: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to match device
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_match_rom_raw(lwow_t* const owobj, const lwow_rom_t* const rom_id) {
    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    LWOW_ASSERT("rom_id != NULL", rom_id != NULL);

    /* Write byte to match rom exactly */
    if (lwow_write_byte_ex_raw(owobj, LWOW_CMD_MATCHROM, NULL) != lwowOK) {
        return lwowERR;
    }
    for (uint8_t idx = 0; idx < 8U; ++idx) { /* Send 8 bytes representing ROM address */
        if (lwow_write_byte_ex_raw(owobj, rom_id->rom[idx], NULL) != lwowOK) { /* Send ROM bytes */
            return lwowERR;
        }
    }

    return lwowOK;
}

/**
 * \copydoc         lwow_match_rom_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_match_rom(lwow_t* const owobj, const lwow_rom_t* const rom_id) {
    lwowr_t res = lwowERR;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    LWOW_ASSERT("rom_id != NULL", rom_id != NULL);

    lwow_protect(owobj, 1U);
    res = lwow_match_rom_raw(owobj, rom_id);
    lwow_unprotect(owobj, 1U);
    return res;
}

/**
 * \brief           Skip ROM address and select all devices on the network
 * \param[in]       owobj: 1-Wire handle
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_skip_rom_raw(lwow_t* const owobj) {
    LWOW_ASSERT("owobj != NULL", owobj != NULL);

    return lwow_write_byte_ex_raw(owobj, LWOW_CMD_SKIPROM, NULL);
}

/**
 * \copydoc         lwow_skip_rom_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_skip_rom(lwow_t* const owobj) {
    lwowr_t res = lwowERR;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);

    lwow_protect(owobj, 1U);
    res = lwow_skip_rom_raw(owobj);
    lwow_unprotect(owobj, 1U);
    return res;
}

/**
 * \brief           Calculate CRC-8 of input data
 * \param[in]       inp: Input data
 * \param[in]       len: Number of bytes
 * \return          Calculated CRC
 * \note            This function is reentrant
 */
uint8_t
lwow_crc(const void* inp, const size_t len) {
    uint8_t crc = 0;
    const uint8_t* p_data = inp;

    if (p_data == NULL || len == 0) {
        return 0;
    }

    for (size_t i = 0; i < len; ++i, ++p_data) {
        uint8_t inbyte = *p_data;
        for (uint8_t j = 8U; j > 0; --j) {
            uint8_t mix = (uint8_t)(crc ^ inbyte) & 0x01U;
            crc >>= 1U;
            if (mix > 0) {
                crc ^= 0x8CU;
            }
            inbyte >>= 0x01U;
        }
    }
    return crc;
}

/**
 * \brief           Search devices on 1-wire network by using callback function and custom search command
 *
 * When new device is detected, callback function `func` is called to notify user
 *
 * \param[in]       owobj: 1-Wire handle
 * \param[in]       cmd: 1-Wire search command
 * \param[out]      roms_found: Output variable to save number of found devices. Set to `NULL` if not used
 * \param[in]       func: Callback function to call for each device
 * \param[in]       arg: Custom user argument, used in callback function
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 * \note            This function is thread-safe
 */
lwowr_t
lwow_search_with_command_callback(lwow_t* const owobj, const uint8_t cmd, size_t* const roms_found,
                                  const lwow_search_cb_fn func, void* arg) {
    lwowr_t res = lwowERR;
    lwow_rom_t rom_id = {0};
    size_t idx = 0;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    LWOW_ASSERT("func != NULL", func != NULL);

    lwow_protect(owobj, 1U);
    /* Search device-by-device until all found */
    for (idx = 0, res = lwow_search_reset_raw(owobj);
         res == lwowOK && (res = lwow_search_with_command_raw(owobj, cmd, &rom_id)) == lwowOK; ++idx) {
        res = func(owobj, &rom_id, idx, arg);
        if (res != lwowOK) {
            break;
        }
    }
    func(owobj, NULL, idx, arg); /* Call with NULL rom_id parameter */
    lwow_unprotect(owobj, 1U);
    SET_NOT_NULL(roms_found, idx); /* Set number of roms found */
    if (res == lwowERRNODEV) {     /* `No device` might not be an error, but simply no devices on bus */
        res = lwowOK;
    }
    return res;
}

/**
 * \brief           Search devices on 1-wire network by using callback function and `SEARCH_ROM` 1-Wire command
 *
 * When new device is detected, callback function `func` is called to notify user
 *
 * \param[in]       owobj: 1-Wire handle
 * \param[out]      roms_found: Output variable to save number of found devices. Set to `NULL` if not used
 * \param[in]       func: Callback function to call for each device
 * \param[in]       arg: Custom user argument, used in callback function
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 * \note            This function is thread-safe
 */
lwowr_t
lwow_search_with_callback(lwow_t* const owobj, size_t* const roms_found, const lwow_search_cb_fn func, void* arg) {
    return lwow_search_with_command_callback(owobj, LWOW_CMD_SEARCHROM, roms_found, func, arg);
}

/**
 * \brief           Search for devices on 1-Wire network with command and store ROM IDs to input array
 * \param[in]       owobj: 1-Wire handle
 * \param[in]       cmd: 1-Wire search command
 * \param[in]       rom_id_arr: Pointer to output array to store found ROM IDs into
 * \param[in]       rom_len: Length of input ROM array
 * \param[out]      roms_found: Output variable to save number of found devices. Set to `NULL` if not used
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_search_devices_with_command_raw(lwow_t* const owobj, const uint8_t cmd, lwow_rom_t* const rom_id_arr,
                                     const size_t rom_len, size_t* const roms_found) {
    lwowr_t res = lwowERR;
    size_t cnt = 0;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    LWOW_ASSERT("rom_id_arr != NULL", rom_id_arr != NULL);
    LWOW_ASSERT("rom_len > 0", rom_len > 0);

    for (cnt = 0, res = lwow_search_reset_raw(owobj); cnt < rom_len; ++cnt) {
        res = lwow_search_with_command_raw(owobj, cmd, &rom_id_arr[cnt]);
        if (res != lwowOK) {
            break;
        }
    }
    SET_NOT_NULL(roms_found, cnt); /* Set number of roms found */
    if (res == lwowERRNODEV && cnt > 0) {
        res = lwowOK;
    }
    return res;
}

/**
 * \copydoc         lwow_search_devices_with_command_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_search_devices_with_command(lwow_t* const owobj, const uint8_t cmd, lwow_rom_t* const rom_id_arr,
                                 const size_t rom_len, size_t* const roms_found) {
    lwowr_t res = lwowERR;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    LWOW_ASSERT("rom_id_arr != NULL", rom_id_arr != NULL);
    LWOW_ASSERT("rom_len > 0", rom_len > 0);

    lwow_protect(owobj, 1U);
    res = lwow_search_devices_with_command_raw(owobj, cmd, rom_id_arr, rom_len, roms_found);
    lwow_unprotect(owobj, 1U);
    return res;
}

/**
 * \brief           Search for devices on 1-Wire network with default command and store ROM IDs to input array
 * \param[in]       owobj: 1-Wire handle
 * \param[in]       rom_id_arr: Pointer to output array to store found ROM IDs into
 * \param[in]       rom_len: Length of input ROM array
 * \param[out]      roms_found: Output variable to save number of found devices. Set to `NULL` if not used
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_search_devices_raw(lwow_t* const owobj, lwow_rom_t* const rom_id_arr, const size_t rom_len,
                        size_t* const roms_found) {
    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    LWOW_ASSERT("rom_id_arr != NULL", rom_id_arr != NULL);
    LWOW_ASSERT("rom_len > 0", rom_len > 0);

    return lwow_search_devices_with_command_raw(owobj, LWOW_CMD_SEARCHROM, rom_id_arr, rom_len, roms_found);
}

/**
 * \copydoc         lwow_search_devices_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_search_devices(lwow_t* const owobj, lwow_rom_t* const rom_id_arr, const size_t rom_len, size_t* const roms_found) {
    lwowr_t res = lwowERR;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);
    LWOW_ASSERT("rom_id_arr != NULL", rom_id_arr != NULL);
    LWOW_ASSERT("rom_len > 0", rom_len > 0);

    lwow_protect(owobj, 1U);
    res = lwow_search_devices_raw(owobj, rom_id_arr, rom_len, roms_found);
    lwow_unprotect(owobj, 1U);
    return res;
}

/* Deprecated functions list */

/**
 * \brief           Write byte over 1-wire protocol
 * \note            This function is deprecated. Use \ref lwow_write_byte_ex_raw instead
 * \deprecated      This function is deprecated. Use \ref lwow_write_byte_ex_raw instead
 * \param[in,out]   owobj: 1-Wire handle
 * \param[in]       byt: Byte to write
 * \return          Received byte over 1-wire protocol
 */
uint8_t
lwow_write_byte_raw(lwow_t* const owobj, const uint8_t byt) {
    uint8_t brt = 0;
    return lwow_write_byte_ex_raw(owobj, byt, &brt) == lwowOK ? brt : 0x00;
}

/**
 * \copydoc         lwow_write_byte_raw
 * \note            This function is deprecated. Use \ref lwow_write_byte_ex instead
 * \deprecated      This function is deprecated. Use \ref lwow_write_byte_ex instead
 * \note            This function is thread-safe
 */
uint8_t
lwow_write_byte(lwow_t* const owobj, const uint8_t byt) {
    uint8_t brt = 0;
    return lwow_write_byte_ex(owobj, byt, &brt) == lwowOK ? brt : 0x00;
}

/**
 * \brief           Read next byte on 1-Wire
 * \note            This function is deprecated. Use \ref lwow_read_byte_ex_raw instead
 * \deprecated      This function is deprecated. Use \ref lwow_read_byte_ex_raw instead
 * \param[in,out]   owobj: 1-Wire handle
 * \return          Byte read over 1-Wire
 */
uint8_t
lwow_read_byte_raw(lwow_t* const owobj) {
    uint8_t brt = 0;
    return lwow_read_byte_ex_raw(owobj, &brt) == lwowOK ? brt : 0x00;
}

/**
 * \copydoc         lwow_read_byte_raw
 * \note            This function is deprecated. Use \ref lwow_read_byte_ex instead
 * \deprecated      This function is deprecated. Use \ref lwow_read_byte_ex instead
 * \note            This function is thread-safe
 */
uint8_t
lwow_read_byte(lwow_t* const owobj) {
    uint8_t brt = 0;
    return lwow_read_byte_ex(owobj, &brt) == lwowOK ? brt : 0x00;
}

/**
 * \brief           Read single bit on 1-Wire network
 * \note            This function is deprecated. Use \ref lwow_read_bit_ex_raw instead
 * \deprecated      This function is deprecated. Use \ref lwow_read_bit_ex_raw instead
 * \param[in,out]   owobj: 1-Wire handle
 * \return          Bit value
 */
uint8_t
lwow_read_bit_raw(lwow_t* const owobj) {
    uint8_t brt = 0;
    return lwow_read_bit_ex_raw(owobj, &brt) == lwowOK ? brt : 0x00;
}

/**
 * \copydoc         lwow_read_bit_raw
 * \note            This function is deprecated. Use \ref lwow_read_bit_ex instead
 * \deprecated      This function is deprecated. Use \ref lwow_read_bit_ex instead
 * \note            This function is thread-safe
 */
uint8_t
lwow_read_bit(lwow_t* const owobj) {
    uint8_t brt = 0;
    return lwow_read_bit_ex(owobj, &brt) == lwowOK ? brt : 0x00;
}
