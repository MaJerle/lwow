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
    uint8_t ret = 0;
    ow_protect(ow);                             /* Protect 1-Wire */
    if (ow_reset(ow) == owOK) {                 /* Try to reset devices */
        if (rom_id == NULL) {                   /* Check for ROM id */
            ow_skip_rom(ow);                    /* Skip ROM, send to all devices */
        } else {
            ow_match_rom(ow, rom_id);           /* Select exact device by ROM address */
        }
        ow_write_byte(ow, 0x44);                /* Start temperature conversion */
        ret = 1;
    }
    ow_unprotect(ow);                           /* Unprotect 1-Wire */
    return ret;
}

/**
 * \brief           Read temperature previously started with \ref ow_ds18x20_start
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to read data from
 * \param[out]      t: Pointer to output float variable to save temperature
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ds18x20_read(ow_t* ow, uint8_t* rom_id, float* t) {
    uint8_t ret = 0, data[9], i, crc, resolution, m = 0;
    int8_t digit;
    float dec;
    uint16_t temp;
    
    /* Check ROM device */
    if (rom_id != NULL && !ow_ds18x20_is_b(ow, rom_id) && !ow_ds18x20_is_s(ow, rom_id)) {
        return 0;
    }

    ow_protect(ow);                             /* Protect 1-Wire */
    /*
     * First read bit and check if all devices completed with conversion.
     * If everything ready, try to reset the network and continue
     */
    if (ow_read_bit(ow) && ow_reset(ow) == owOK) {
        if (rom_id == NULL) {                   /* Check for ROM id */
            ow_skip_rom(ow);                    /* Skip ROM, send to all devices */
        } else {
            ow_match_rom(ow, rom_id);           /* Select exact device by ROM address */
        }
        ow_write_byte(ow, OW_CMD_RSCRATCHPAD);  /* Send command to read scratchpad */
        
        /* Read plain data from device */
        for (i = 0; i < 9; i++) {
            data[i] = ow_read_byte(ow);         /* Read byte */
        }
        crc = ow_crc(data, 0x09);               /* Calculate CRC */
        if (!crc) {                             /* Result must be 0 to match the CRC */
            temp = (data[1] << 0x08) | data[0]; /* Format data in integer format */
            resolution = ((data[4] & 0x60) >> 0x05) + 0x09; /* Set resolution in units of bits */
            if (temp & 0x8000) {                /* Check for negative temperature */
                temp = ~temp + 1;               /* Perform two's complement */
                m = 1;
            }
            digit = (temp >> 0x04) | (((temp >> 0x08) & 0x07) << 0x04);
            switch (resolution) {               /* Check for resolution settings */
                case 9:  dec = ((temp >> 0x03) & 0x01) * 0.5f; break;
                case 10: dec = ((temp >> 0x02) & 0x03) * 0.25f; break;
                case 11: dec = ((temp >> 0x01) & 0x07) * 0.125f; break;
                case 12: dec = (temp & 0x0F) * 0.0625f; break;
                default: dec = 0xFF, digit = 0;
            }
            dec += digit;
            if (m) {
                dec = -dec;
            }
            *t = dec;
            ret = 1;
        }
    }
    ow_unprotect(ow);                           /* Unprotect 1-Wire */
    return ret;
}


/**
 * \brief           Get resolution for `DS18B20` device
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to get resolution from
 * \return          Resolution in units of bits (`9 - 12`) on success, `0` otherwise
 */
uint8_t
ow_ds18x20_get_resolution(ow_t* ow, uint8_t* rom_id) {
    uint8_t res = 0;

    if (!ow_ds18x20_is_b(ow, rom_id)) {         /* Check if it is B version */
        return 0;
    }

    ow_protect(ow);                             /* Protect 1-Wire */
    if (ow_reset(ow) == owOK) {                 /* Reset bus */
        ow_match_rom(ow, rom_id);               /* Select device */
        ow_write_byte(ow, OW_CMD_RSCRATCHPAD);  /* Read scratchpad command */

        ow_read_byte(ow);                       /* Read byte, ignore it */
        ow_read_byte(ow);                       /* Read byte, ignore it */
        ow_read_byte(ow);
        ow_read_byte(ow);

        res = ((ow_read_byte(ow) & 0x60) >> 0x05) + 9;  /* Read configuration byte and calculate bits */
    }
    ow_unprotect(ow);                           /* Unprotect 1-Wire */

    return res;
}

/**
 * \brief           Set resolution for `DS18B20` sensor
 * \note            `DS18S20` has fixed `9-bit` resolution
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: Address of device to set resolution
 * \param[in]       bits: Number of resolution bits. Possible values are `9 - 12`
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ds18x20_set_resolution(ow_t* ow, uint8_t* rom_id, uint8_t bits) {
    uint8_t th, tl, conf, res = 0;

    if (bits < 9 || bits > 12 ||                /* Check bits range */
        !ow_ds18x20_is_b(ow, rom_id)) {         /* Check if it is B version */
        return 0;
    }

    ow_protect(ow);                             /* Protect 1-Wire */
    if (ow_reset(ow) == owOK) {                 /* Reset bus */
        ow_match_rom(ow, rom_id);               /* Select device */
        ow_write_byte(ow, OW_CMD_RSCRATCHPAD);  /* Read scratchpad command */

        ow_read_byte(ow);                       /* Read byte, ignore it */
        ow_read_byte(ow);                       /* Read byte, ignore it */

        th = ow_read_byte(ow);
        tl = ow_read_byte(ow);
        conf = ow_read_byte(ow) & ~(0x60);      /* Remove configuration bits for temperature resolution */

        switch (bits) {                         /* Check bits configuration */
            case 10: conf |= 0x20; break;       /* 10-bits configuration */
            case 11: conf |= 0x40; break;       /* 11-bits configuration */
            case 12: conf |= 0x60; break;       /* 12-bits configuration */
            default: conf |= 0x00; break;       /* 9-bits configuration */
        }

        /* Write data back to device */
        if (ow_reset(ow) == owOK) {
            ow_match_rom(ow, rom_id);           /* Select device */
            ow_write_byte(ow, OW_CMD_WSCRATCHPAD);  /* Write scratchpad command */

            ow_write_byte(ow, th);
            ow_write_byte(ow, tl);
            ow_write_byte(ow, conf);

            /* Copy scratchpad to non-volatile memory */
            if (ow_reset(ow) == owOK) {         
                ow_match_rom(ow, rom_id);       /* Select device */
                ow_write_byte(ow, OW_CMD_CPYSCRATCHPAD);/* Copy scratchpad */

                res = 1;
            }
        }
    }
    ow_unprotect(ow);                           /* Unprotect 1-Wire */

    return res;
}

/**
 * \brief           Check if ROM address matches `DS18B20` device
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
 * \brief           Check if ROM address matches `DS18S20` device
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to test for DS18S20
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ds18x20_is_s(ow_t* ow, uint8_t* rom_id) {
    OW_UNUSED(ow);                              /* Unused variable */
    return rom_id != NULL && *rom_id == 0x10;   /* Check for correct ROM family code */
}
