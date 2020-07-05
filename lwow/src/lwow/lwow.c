/**
 * \file            lwow.c
 * \brief           OneWire protocol implementation
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
 * Version:         v3.0.0
 */
#include <string.h>
#include "lwow/lwow.h"

#if !__DOXYGEN__

/* Internal macros */
#define OW_FIRST_DEV                    0xFF
#define OW_LAST_DEV                     0x00

#define OW_RESET_BYTE                   0xF0

#endif /* !__DOXYGEN__ */

/* Set value if not NULL */
#define SET_NOT_NULL(p, v)          if ((p) != NULL) { *(p) = (v); }

/**
 * \brief           Send single bit to OneWire port
 * \param[in]       ow: OneWire instance
 * \param[in]       btw: Bit to send, either `1` or `0`
 * \param[out]      btr: Pointer to output variable to write read bit
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
static lwowr_t
prv_send_bit(lwow_t* const ow, uint8_t btw, uint8_t* btr) {
    uint8_t b;

    SET_NOT_NULL(btr, 0);

    /*
     * To send logical 1 over 1-wire, send 0xFF over UART
     * To send logical 0 over 1-wire, send 0x00 over UART
     */
    btw = btw > 0 ? 0xFF : 0x00;                /* Convert to 0 or 1 */
    if (!ow->ll_drv->tx_rx(&btw, &b, 1, ow->arg)) {
        return lwowERRTXRX;                     /* Transmit error */
    }
    b = b == 0xFF ? 1 : 0;                      /* Go to bit values */
    SET_NOT_NULL(btr, b);                       /* Set new byte */
    return lwowOK;
}

/**
 * \brief           Initialize OneWire instance
 * \param[in]       ow: OneWire instance
 * \param[in]       ll_drv: Low-level driver
 * \param[in]       arg: Custom argument
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_init(lwow_t* const ow, const lwow_ll_drv_t* const ll_drv, void* arg) {
    LWOW_ASSERT("ow != NULL", ow != NULL);
    LWOW_ASSERT("ll_drv != NULL", ll_drv != NULL);
    LWOW_ASSERT("ll_drv->init != NULL", ll_drv->init != NULL);
    LWOW_ASSERT("ll_drv->deinit != NULL", ll_drv->deinit != NULL);
    LWOW_ASSERT("ll_drv->set_baudrate != NULL", ll_drv->set_baudrate != NULL);
    LWOW_ASSERT("ll_drv->tx_rx != NULL", ll_drv->tx_rx != NULL);

    ow->arg = arg;
    ow->ll_drv = ll_drv;                        /* Assign low-level driver */
    if (!ow->ll_drv->init(ow->arg)) {           /* Init low-level directly */
        return lwowERR;
    }
#if LWOW_CFG_OS
    if (!lwow_sys_mutex_create(&ow->mutex, arg)) {
        ow->ll_drv->deinit(ow->arg);            /* Deinit low-level */
        return lwowERR;
    }
#endif /* LWOW_CFG_OS */
    return lwowOK;
}

/**
 * \brief           Deinitialize OneWire instance
 * \param[in]       ow: OneWire instance
 */
void
lwow_deinit(lwow_t* const ow) {
    if (ow == NULL || ow->ll_drv == NULL) {
        return;
    }

#if LWOW_CFG_OS
    lwow_sys_mutex_delete(&ow->mutex, ow->arg);
#endif /* LWOW_CFG_OS */
    ow->ll_drv->deinit(ow->arg);
}

/**
 * \brief           Protect 1-wire from concurrent access
 * \note            Used only for OS systems
 * \param[in,out]   ow: 1-Wire handle
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_protect(lwow_t* const ow, const uint8_t protect) {
    LWOW_ASSERT("ow != NULL", ow != NULL);

#if LWOW_CFG_OS
    if (protect && !lwow_sys_mutex_wait(&ow->mutex, ow->arg)) {
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
 * \param[in,out]   ow: 1-Wire handle
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_unprotect(lwow_t* const ow, const uint8_t protect) {
    LWOW_ASSERT("ow != NULL", ow != NULL);

#if LWOW_CFG_OS
    if (protect && !lwow_sys_mutex_release(&ow->mutex, ow->arg)) {
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
 * \param[in,out]   ow: 1-Wire handle
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_reset_raw(lwow_t* const ow) {
    uint8_t b;

    LWOW_ASSERT("ow != NULL", ow != NULL);

    /* First send reset pulse */
    b = OW_RESET_BYTE;                          /* Set reset sequence byte = 0xF0 */
    if (!ow->ll_drv->set_baudrate(9600, ow->arg)) {
        return lwowERRBAUD;                       /* Error setting baudrate */
    }
    if (!ow->ll_drv->tx_rx(&b, &b, 1, ow->arg)) {
        return lwowERRTXRX;                       /* Error with data exchange */
    }
    if (!ow->ll_drv->set_baudrate(115200, ow->arg)) {
        return lwowERRBAUD;                       /* Error setting baudrate */
    }

    /* Check if there is reply from any device */
    if (b == 0 || b == OW_RESET_BYTE) {
        return lwowERRPRESENCE;
    }
    return lwowOK;
}

/**
 * \copydoc         lwow_reset_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_reset(lwow_t* const ow) {
    lwowr_t res;

    LWOW_ASSERT("ow != NULL", ow != NULL);

    lwow_protect(ow, 1);
    res = lwow_reset_raw(ow);
    lwow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Write byte over OW and read its response
 * \param[in,out]   ow: 1-Wire handle
 * \param[in]       btw: Byte to write
 * \param[out]      br: Pointer to read value. Set to `NULL` if not used
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_write_byte_ex_raw(lwow_t* const ow, const uint8_t btw, uint8_t* const br) {
    uint8_t tr[8];

    LWOW_ASSERT("ow != NULL", ow != NULL);
    SET_NOT_NULL(br, 0);

    /* Prepare output data */
    for (uint8_t i = 0; i < 8; ++i) {
        /*
         * If we have to send high bit, set byte as 0xFF,
         * otherwise set it as low bit, 0x00
         */
        tr[i] = (btw & (1 << i)) ? 0xFF : 0x00;
    }

    /*
     * Exchange data on UART level,
     * send single byte for each bit = 8 bytes
     */
    if (!ow->ll_drv->tx_rx(tr, tr, 8, ow->arg)) {
        return lwowERRTXRX;
    }

    /* Update output value */
    if (br != NULL) {
        uint8_t r = 0;
        /*
         * Check received data. If we read 0xFF,
         * our logical write 1 was successful, otherwise it was 0.
         */
        for (uint8_t i = 0; i < 8; ++i) {
            if (tr[i] == 0xFF) {
                r |= 0x01 << i;
            }
        }
        *br = r;
    }
    return lwowOK;
}

/**
 * \copydoc         lwow_write_byte_ex_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_write_byte_ex(lwow_t* const ow, const uint8_t btw, uint8_t* const br) {
    lwowr_t res;

    LWOW_ASSERT("ow != NULL", ow != NULL);

    lwow_protect(ow, 1);
    res = lwow_write_byte_ex_raw(ow, btw, br);
    lwow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Read byte from OW device
 * \param[in,out]   ow: 1-Wire handle
 * \param[out]      br: Pointer to save read value
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_read_byte_ex_raw(lwow_t* const ow, uint8_t* const br) {
    LWOW_ASSERT("ow != NULL", ow != NULL);
    LWOW_ASSERT("br != NULL", br != NULL);

    /*
     * When we want to read byte over 1-Wire,
     * we have to send all bits as 1 and check if slave pulls line down.
     *
     * According to slave reactions, we can later construct received bytes
     */
    return lwow_write_byte_ex_raw(ow, 0xFF, br);
}

/**
 * \copydoc         lwow_read_byte_ex_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_read_byte_ex(lwow_t* const ow, uint8_t* const br) {
    lwowr_t res;

    LWOW_ASSERT("ow != NULL", ow != NULL);
    LWOW_ASSERT("br != NULL", br != NULL);

    lwow_protect(ow, 1);
    res = lwow_read_byte_ex_raw(ow, br);
    lwow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Read sinle bit from OW device
 * \param[in,out]   ow: 1-Wire handle
 * \param[out]      br: Pointer to save read value, either `1` or `0`
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_read_bit_ex_raw(lwow_t* const ow, uint8_t* const br) {
    LWOW_ASSERT("ow != NULL", ow != NULL);
    LWOW_ASSERT("br != NULL", br != NULL);

    return prv_send_bit(ow, 1, br);             /* Send bit as `1` and read the response */
}

/**
 * \copydoc         lwow_read_bit_ex_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_read_bit_ex(lwow_t* const ow, uint8_t* const br) {
    lwowr_t res;

    LWOW_ASSERT("ow != NULL", ow != NULL);
    LWOW_ASSERT("br != NULL", br != NULL);

    lwow_protect(ow, 1);
    res = lwow_read_bit_ex_raw(ow, br);
    lwow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Reset search
 * \param[in,out]   ow: 1-Wire handle
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_search_reset_raw(lwow_t* const ow) {
    LWOW_ASSERT("ow != NULL", ow != NULL);

    ow->disrepancy = OW_FIRST_DEV;              /* Reset disrepancy to default value */
    return lwowOK;
}

/**
 * \copydoc         lwow_search_reset_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_search_reset(lwow_t* const ow) {
    lwowr_t res;

    LWOW_ASSERT("ow != NULL", ow != NULL);

    lwow_protect(ow, 1);
    res = lwow_search_reset_raw(ow);
    lwow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Search for devices on 1-wire bus
 * \note            To reset search and to start over, use \ref lwow_search_reset function
 * \param[in,out]   ow: 1-Wire handle
 * \param[out]      rom_id: Pointer to ROM structure to save ROM
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_search_raw(lwow_t* const ow, lwow_rom_t* const rom_id) {
    LWOW_ASSERT("ow != NULL", ow != NULL);
    return lwow_search_with_command_raw(ow, LWOW_CMD_SEARCHROM, rom_id);
}

/**
 * \copydoc         lwow_search_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_search(lwow_t* const ow, lwow_rom_t* const rom_id) {
    lwowr_t res;

    LWOW_ASSERT("ow != NULL", ow != NULL);

    lwow_protect(ow, 1);
    res = lwow_search_raw(ow, rom_id);
    lwow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Search for devices on 1-wire bus with custom search command
 * \note            To reset search and to start over, use \ref lwow_search_reset function
 * \param[in,out]   ow: 1-Wire handle
 * \param[in]       cmd: command to use for search operation
 * \param[out]      rom_id: Pointer to ROM structure to store address
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_search_with_command_raw(lwow_t* const ow, const uint8_t cmd, lwow_rom_t* const rom_id) {
    lwowr_t res;
    uint8_t id_bit_number, next_disrepancy, *id;

    LWOW_ASSERT("ow != NULL", ow != NULL);
    LWOW_ASSERT("rom_id != NULL", rom_id != NULL);

    id = ow->rom.rom;

    /* Check for last device */
    if (ow->disrepancy == 0) {
        lwow_search_reset_raw(ow);              /* Reset search for next search */
        return lwowERRNODEV;                    /* No devices anymore */
    }

    /* Step 1: Reset all devices on 1-Wire line to be able to listen for new command */
    if ((res = lwow_reset_raw(ow)) != lwowOK) {
        return res;
    }

    /* Step 2: Send search rom command for all devices on 1-Wire */
    lwow_write_byte_ex_raw(ow, cmd, NULL);      /* Start with search ROM command */
    next_disrepancy = OW_LAST_DEV;              /* This is currently last device */

    for (id_bit_number = 64; id_bit_number > 0;) {
        uint8_t b, b_cpl;
        for (uint8_t j = 8; j > 0; --j, --id_bit_number) {
            /* Read first bit and its complimentary one */
            if (prv_send_bit(ow, 1, &b) != lwowOK || prv_send_bit(ow, 1, &b_cpl) != lwowOK) {
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
            if (b && b_cpl) {
                goto out;                       /* We do not have device connected */
            } else if (!b && !b_cpl) {
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
                if (id_bit_number < ow->disrepancy || ((*id & 0x01) && ow->disrepancy != id_bit_number)) {
                    b = 1;
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
            prv_send_bit(ow, b, NULL);

            /*
             * Because we shift down *id each iteration, we have to position bit value to the MSB position
             * and it will be automatically positioned correct way.
             */
            *id = (*id >> 0x01) | (b << 0x07);  /* Shift ROM byte down and add next, protocol is LSB first */
        }
        ++id;                                   /* Go to next byte */
    }
out:
    ow->disrepancy = next_disrepancy;           /* Save disrepancy value */
    memcpy(rom_id->rom, ow->rom.rom, sizeof(ow->rom.rom));  /* Copy ROM to user memory */
    return id_bit_number == 0 ? lwowOK : lwowERRNODEV;  /* Return search result status */
}

/**
 * \copydoc         lwow_search_with_command_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_search_with_command(lwow_t* const ow, const uint8_t cmd, lwow_rom_t* const rom_id) {
    lwowr_t res;

    LWOW_ASSERT("ow != NULL", ow != NULL);
    LWOW_ASSERT("rom_id != NULL", rom_id != NULL);

    lwow_protect(ow, 1);
    res = lwow_search_with_command_raw(ow, cmd, rom_id);
    lwow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Select device on 1-wire network with exact ROM number
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to match device
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_match_rom_raw(lwow_t* const ow, const lwow_rom_t* const rom_id) {
    LWOW_ASSERT("ow != NULL", ow != NULL);
    LWOW_ASSERT("rom_id != NULL", rom_id != NULL);

    /* Write byte to match rom exactly */
    if (lwow_write_byte_ex_raw(ow, LWOW_CMD_MATCHROM, NULL) != lwowOK) {
        return lwowERR;
    }
    for (uint8_t i = 0; i < 8; ++i) {           /* Send 8 bytes representing ROM address */
        if (lwow_write_byte_ex_raw(ow, rom_id->rom[i], NULL) != lwowOK) {   /* Send ROM bytes */
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
lwow_match_rom(lwow_t* const ow, const lwow_rom_t* const rom_id) {
    uint8_t res;

    LWOW_ASSERT("ow != NULL", ow != NULL);
    LWOW_ASSERT("rom_id != NULL", rom_id != NULL);

    lwow_protect(ow, 1);
    res = lwow_match_rom_raw(ow, rom_id);
    lwow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Skip ROM address and select all devices on the network
 * \param[in]       ow: 1-Wire handle
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_skip_rom_raw(lwow_t* const ow) {
    LWOW_ASSERT("ow != NULL", ow != NULL);

    return lwow_write_byte_ex_raw(ow, LWOW_CMD_SKIPROM, NULL);
}

/**
 * \copydoc         lwow_skip_rom_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_skip_rom(lwow_t* const ow) {
    uint8_t res;

    LWOW_ASSERT("ow != NULL", ow != NULL);

    lwow_protect(ow, 1);
    res = lwow_skip_rom_raw(ow);
    lwow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Calculate CRC-8 of input data
 * \param[in]       in: Input data
 * \param[in]       len: Number of bytes
 * \return          Calculated CRC
 * \note            This function is reentrant
 */
uint8_t
lwow_crc(const void* in, const size_t len) {
    uint8_t crc = 0;
    const uint8_t* d = in;

    if (in == NULL || len == 0) {
        return 0;
    }

    for (size_t i = 0; i < len; ++i, ++d) {
        uint8_t inbyte = *d;
        for (uint8_t j = 8; j > 0; --j) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix > 0) {
                crc ^= 0x8C;
            }
            inbyte >>= 0x01;
        }
    }
    return crc;
}

/**
 * \brief           Search devices on 1-wire network by using callback function and custom search command
 *
 * When new device is detected, callback function `func` is called to notify user
 *
 * \param[in]       ow: 1-Wire handle
 * \param[in]       cmd: 1-Wire search command
 * \param[out]      roms_found: Output variable to save number of found devices. Set to `NULL` if not used
 * \param[in]       func: Callback function to call for each device
 * \param[in]       arg: Custom user argument, used in callback function
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 * \note            This function is thread-safe
 */
lwowr_t
lwow_search_with_command_callback(lwow_t* const ow, const uint8_t cmd, size_t* const roms_found,
                                const lwow_search_cb_fn func, void* arg) {
    lwowr_t res;
    lwow_rom_t rom_id;
    size_t i;

    LWOW_ASSERT("ow != NULL", ow != NULL);
    LWOW_ASSERT("func != NULL", func != NULL);

    lwow_protect(ow, 1);
    /* Search device-by-device until all found */
    for (i = 0, res = lwow_search_reset_raw(ow);
         res == lwowOK && (res = lwow_search_with_command_raw(ow, cmd, &rom_id)) == lwowOK; ++i) {
        if ((res = func(ow, &rom_id, i, arg)) != lwowOK) {
            break;
        }
    }
    func(ow, NULL, i, arg);                     /* Call with NULL rom_id parameter */
    lwow_unprotect(ow, 1);

    if (roms_found != NULL) {
        *roms_found = i;
    }
    if (res == lwowERRNODEV) {                  /* `No device` might not be an error, but simply no devices on bus */
        res = lwowOK;
    }
    return res;
}

/**
 * \brief           Search devices on 1-wire network by using callback function and `SEARCH_ROM` 1-Wire command
 *
 * When new device is detected, callback function `func` is called to notify user
 *
 * \param[in]       ow: 1-Wire handle
 * \param[out]      roms_found: Output variable to save number of found devices. Set to `NULL` if not used
 * \param[in]       func: Callback function to call for each device
 * \param[in]       arg: Custom user argument, used in callback function
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 * \note            This function is thread-safe
 */
lwowr_t
lwow_search_with_callback(lwow_t* const ow, size_t* const roms_found, const lwow_search_cb_fn func, void* arg) {
    return lwow_search_with_command_callback(ow, LWOW_CMD_SEARCHROM, roms_found, func, arg);
}

/**
 * \brief           Search for devices on 1-Wire network with command and store ROM IDs to input array
 * \param[in]       ow: 1-Wire handle
 * \param[in]       cmd: 1-Wire search command
 * \param[in]       rom_id_arr: Pointer to output array to store found ROM IDs into
 * \param[in]       rom_len: Length of input ROM array
 * \param[out]      roms_found: Output variable to save number of found devices. Set to `NULL` if not used
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_search_devices_with_command_raw(lwow_t* const ow, const uint8_t cmd, lwow_rom_t* const rom_id_arr,
                                   const size_t rom_len, size_t* const roms_found) {
    lwowr_t res;
    size_t cnt = 0;

    LWOW_ASSERT("ow != NULL", ow != NULL);
    LWOW_ASSERT("rom_id_arr != NULL", rom_id_arr != NULL);
    LWOW_ASSERT("rom_len > 0", rom_len > 0);

    for (cnt = 0, res = lwow_search_reset_raw(ow); cnt < rom_len; ++cnt) {
        if ((res = lwow_search_with_command_raw(ow, cmd, &rom_id_arr[cnt])) != lwowOK) {
            break;
        }
    }
    if (roms_found != NULL) {
        *roms_found = cnt;
    }
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
lwow_search_devices_with_command(lwow_t* const ow, const uint8_t cmd, lwow_rom_t* const rom_id_arr,
                               const size_t rom_len, size_t* const roms_found) {
    lwowr_t res;

    LWOW_ASSERT("ow != NULL", ow != NULL);
    LWOW_ASSERT("rom_id_arr != NULL", rom_id_arr != NULL);
    LWOW_ASSERT("rom_len > 0", rom_len > 0);

    lwow_protect(ow, 1);
    res = lwow_search_devices_with_command_raw(ow, cmd, rom_id_arr, rom_len, roms_found);
    lwow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Search for devices on 1-Wire network with default command and store ROM IDs to input array
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id_arr: Pointer to output array to store found ROM IDs into
 * \param[in]       rom_len: Length of input ROM array
 * \param[out]      roms_found: Output variable to save number of found devices. Set to `NULL` if not used
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_search_devices_raw(lwow_t* const ow, lwow_rom_t* const rom_id_arr, const size_t rom_len, size_t* const roms_found) {
    LWOW_ASSERT("ow != NULL", ow != NULL);
    LWOW_ASSERT("rom_id_arr != NULL", rom_id_arr != NULL);
    LWOW_ASSERT("rom_len > 0", rom_len > 0);

    return lwow_search_devices_with_command_raw(ow, LWOW_CMD_SEARCHROM, rom_id_arr, rom_len, roms_found);
}

/**
 * \copydoc         lwow_search_devices_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_search_devices(lwow_t* const ow, lwow_rom_t* const rom_id_arr, const size_t rom_len, size_t* const roms_found) {
    lwowr_t res;

    LWOW_ASSERT("ow != NULL", ow != NULL);
    LWOW_ASSERT("rom_id_arr != NULL", rom_id_arr != NULL);
    LWOW_ASSERT("rom_len > 0", rom_len > 0);

    lwow_protect(ow, 1);
    res = lwow_search_devices_raw(ow, rom_id_arr, rom_len, roms_found);
    lwow_unprotect(ow, 1);
    return res;
}

/* Deprecated functions list */

/**
 * \brief           Write byte over 1-wire protocol
 * \note            This function is deprecated. Use \ref lwow_write_byte_ex_raw instead
 * \deprecated      This function is deprecated. Use \ref lwow_write_byte_ex_raw instead
 * \param[in,out]   ow: 1-Wire handle
 * \param[in]       b: Byte to write
 * \return          Received byte over 1-wire protocol
 */
uint8_t
lwow_write_byte_raw(lwow_t* const ow, const uint8_t b) {
    uint8_t r;
    return lwow_write_byte_ex_raw(ow, b, &r) == lwowOK ? r : 0x00;
}

/**
 * \copydoc         lwow_write_byte_raw
 * \note            This function is deprecated. Use \ref lwow_write_byte_ex instead
 * \deprecated      This function is deprecated. Use \ref lwow_write_byte_ex instead
 * \note            This function is thread-safe
 */
uint8_t
lwow_write_byte(lwow_t* const ow, const uint8_t b) {
    uint8_t r;
    return lwow_write_byte_ex(ow, b, &r) == lwowOK ? r : 0x00;
}

/**
 * \brief           Read next byte on 1-Wire
 * \note            This function is deprecated. Use \ref lwow_read_byte_ex_raw instead
 * \deprecated      This function is deprecated. Use \ref lwow_read_byte_ex_raw instead
 * \param[in,out]   ow: 1-Wire handle
 * \return          Byte read over 1-Wire
 */
uint8_t
lwow_read_byte_raw(lwow_t* const ow) {
    uint8_t br;
    return lwow_read_byte_ex_raw(ow, &br) == lwowOK ? br : 0x00;
}

/**
 * \copydoc         lwow_read_byte_raw
 * \note            This function is deprecated. Use \ref lwow_read_byte_ex instead
 * \deprecated      This function is deprecated. Use \ref lwow_read_byte_ex instead
 * \note            This function is thread-safe
 */
uint8_t
lwow_read_byte(lwow_t* const ow) {
    uint8_t br;
    return lwow_read_byte_ex(ow, &br) == lwowOK ? br : 0x00;
}

/**
 * \brief           Read single bit on 1-Wire network
 * \note            This function is deprecated. Use \ref lwow_read_bit_ex_raw instead
 * \deprecated      This function is deprecated. Use \ref lwow_read_bit_ex_raw instead
 * \param[in,out]   ow: 1-Wire handle
 * \return          Bit value
 */
uint8_t
lwow_read_bit_raw(lwow_t* const ow) {
    uint8_t br;
    return lwow_read_bit_ex_raw(ow, &br) == lwowOK ? br : 0x00;
}

/**
 * \copydoc         lwow_read_bit_raw
 * \note            This function is deprecated. Use \ref lwow_read_bit_ex instead
 * \deprecated      This function is deprecated. Use \ref lwow_read_bit_ex instead
 * \note            This function is thread-safe
 */
uint8_t
lwow_read_bit(lwow_t* const ow) {
    uint8_t br;
    return lwow_read_bit_ex(ow, &br) == lwowOK ? br : 0x00;
}
