/**
 * \file            ow_device_ds18x20.h
 * \brief           DS18x20 driver implementation
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
#include "ow/ow.h"
#include "ow/devices/ow_device_ds18x20.h"

/**
 * \brief           Start temperature conversion on specific (or all) devices
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to start measurement for.
 *                      Set to `NULL` to start measurement on all devices at the same time
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ds18x20_start_raw(ow_t* const ow, const ow_rom_t* const rom_id) {
    uint8_t ret = 0;

    OW_ASSERT0("ow != NULL", ow != NULL);

    if (ow_reset_raw(ow) == owOK) {
        if (rom_id == NULL) {                   /* Check for ROM id */
            ow_skip_rom_raw(ow);                /* Skip ROM, send to all devices */
        } else {
            ow_match_rom_raw(ow, rom_id);       /* Select exact device by ROM address */
        }
        ow_write_byte_raw(ow, 0x44);            /* Start temperature conversion */
        ret = 1;
    }
    return ret;
}

/**
 * \copydoc         ow_ds18x20_start_raw
 * \note            This function is thread-safe
 */
uint8_t
ow_ds18x20_start(ow_t* const ow, const ow_rom_t* const rom_id) {
    uint8_t res;

    OW_ASSERT0("ow != NULL", ow != NULL);

    ow_protect(ow, 1);
    res = ow_ds18x20_start_raw(ow, rom_id);
    ow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Read temperature previously started with \ref ow_ds18x20_start
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to read data from
 * \param[out]      t: Pointer to output float variable to save temperature
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ds18x20_read_raw(ow_t* const ow, const ow_rom_t* const rom_id, float* const t) {
    float dec;
    uint16_t temp;
    uint8_t ret = 0, data[9], crc, resolution, m = 0;
    int8_t digit;

    OW_ASSERT0("ow != NULL", ow != NULL);
    OW_ASSERT0("t != NULL", t != NULL);
    OW_ASSERT0("ow_ds18x20_is_b(ow, rom_id) || ow_ds18x20_is_s(ow, rom_id)", ow_ds18x20_is_b(ow, rom_id) || ow_ds18x20_is_s(ow, rom_id));

    /*
     * First read bit and check if all devices completed with conversion.
     * If everything ready, try to reset the network and continue
     */
    if (ow_read_bit_raw(ow) && ow_reset_raw(ow) == owOK) {
        if (rom_id == NULL) {                   /* Check for ROM id */
            ow_skip_rom_raw(ow);                /* Skip ROM, send to all devices */
        } else {
            ow_match_rom_raw(ow, rom_id);       /* Select exact device by ROM address */
        }
        ow_write_byte_raw(ow, OW_CMD_RSCRATCHPAD);  /* Send command to read scratchpad */

        /* Read plain data from device */
        for (uint8_t i = 0; i < 9; ++i) {
            data[i] = ow_read_byte_raw(ow);     /* Read byte */
        }
        crc = ow_crc(data, 0x09);               /* Calculate CRC */
        if (crc == 0) {                         /* Result must be 0 to match the CRC */
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

    return ret;
}

/**
 * \copydoc         ow_ds18x20_read_raw
 * \note            This function is thread-safe
 */
uint8_t
ow_ds18x20_read(ow_t* const ow, const ow_rom_t* const rom_id, float* const t) {
    uint8_t res;

    OW_ASSERT0("ow != NULL", ow != NULL);
    OW_ASSERT0("t != NULL", t != NULL);
    OW_ASSERT0("ow_ds18x20_is_b(ow, rom_id) || ow_ds18x20_is_s(ow, rom_id)", ow_ds18x20_is_b(ow, rom_id) || ow_ds18x20_is_s(ow, rom_id));

    ow_protect(ow, 1);
    res = ow_ds18x20_read_raw(ow, rom_id, t);
    ow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Get resolution for `DS18B20` device
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to get resolution from
 * \return          Resolution in units of bits (`9 - 12`) on success, `0` otherwise
 */
uint8_t
ow_ds18x20_get_resolution_raw(ow_t* const ow, const ow_rom_t* const rom_id) {
    uint8_t res = 0;

    OW_ASSERT0("ow != NULL", ow != NULL);
    OW_ASSERT0("rom_id != NULL", rom_id != NULL);
    OW_ASSERT0("ow_ds18x20_is_b(ow, rom_id)", ow_ds18x20_is_b(ow, rom_id));

    if (ow_reset_raw(ow) == owOK) {             /* Reset bus */
        ow_match_rom_raw(ow, rom_id);           /* Select device */
        ow_write_byte_raw(ow, OW_CMD_RSCRATCHPAD);  /* Read scratchpad command */

        /* Read and ignore bytes */
        ow_read_byte_raw(ow);
        ow_read_byte_raw(ow);
        ow_read_byte_raw(ow);
        ow_read_byte_raw(ow);

        res = ((ow_read_byte_raw(ow) & 0x60) >> 0x05) + 9;  /* Read configuration byte and calculate bits */
    }

    return res;
}

/**
 * \copydoc         ow_ds18x20_get_resolution_raw
 * \note            This function is thread-safe
 */
uint8_t
ow_ds18x20_get_resolution(ow_t* const ow, const ow_rom_t* const rom_id) {
    uint8_t res;

    OW_ASSERT0("ow != NULL", ow != NULL);
    OW_ASSERT0("rom_id != NULL", rom_id != NULL);
    OW_ASSERT0("ow_ds18x20_is_b(ow, rom_id)", ow_ds18x20_is_b(ow, rom_id));

    ow_protect(ow, 1);
    res = ow_ds18x20_get_resolution_raw(ow, rom_id);
    ow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Set resolution for `DS18B20` sensor
 * \note            `DS18S20` has fixed `9-bit` resolution
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to set resolution
 * \param[in]       bits: Number of resolution bits. Possible values are `9 - 12`
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ds18x20_set_resolution_raw(ow_t* const ow, const ow_rom_t* const rom_id, const uint8_t bits) {
    uint8_t th, tl, conf, res = 0;

    OW_ASSERT0("ow != NULL", ow != NULL);
    OW_ASSERT0("bits >= 9 && bits <= 12", bits >= 9 && bits <= 12);
    OW_ASSERT0("ow_ds18x20_is_b(ow, rom_id)", ow_ds18x20_is_b(ow, rom_id));

    if (ow_reset_raw(ow) == owOK) {
        if (rom_id == NULL) {
            ow_match_rom_raw(ow, rom_id);
        } else {
            ow_skip_rom_raw(ow);
        }
        ow_write_byte_raw(ow, OW_CMD_RSCRATCHPAD);

        /* Read and ignore bytes */
        ow_read_byte_raw(ow);
        ow_read_byte_raw(ow);

        /* Read important data */
        th = ow_read_byte_raw(ow);
        tl = ow_read_byte_raw(ow);
        conf = ow_read_byte_raw(ow) & ~0x60;    /* Remove configuration bits for temperature resolution */

        switch (bits) {                         /* Check bits configuration */
            case 10: conf |= 0x20; break;       /* 10-bits configuration */
            case 11: conf |= 0x40; break;       /* 11-bits configuration */
            case 12: conf |= 0x60; break;       /* 12-bits configuration */
            default: conf |= 0x00; break;       /* 9-bits configuration */
        }

        /* Write data back to device */
        if (ow_reset_raw(ow) == owOK) {
            ow_match_rom_raw(ow, rom_id);
            ow_write_byte_raw(ow, OW_CMD_WSCRATCHPAD);

            ow_write_byte_raw(ow, th);
            ow_write_byte_raw(ow, tl);
            ow_write_byte_raw(ow, conf);

            /* Copy scratchpad to non-volatile memory */
            if (ow_reset_raw(ow) == owOK) {
                ow_match_rom_raw(ow, rom_id);
                ow_write_byte_raw(ow, OW_CMD_CPYSCRATCHPAD);
                res = 1;
            }
        }
    }
    return res;
}

/**
 * \copydoc         ow_ds18x20_set_resolution_raw
 * \note            This function is thread-safe
 */
uint8_t
ow_ds18x20_set_resolution(ow_t* const ow, const ow_rom_t* const rom_id, const uint8_t bits) {
    uint8_t res;

    OW_ASSERT0("ow != NULL", ow != NULL);
    OW_ASSERT0("bits >= 9 && bits <= 12", bits >= 9 && bits <= 12);
    OW_ASSERT0("ow_ds18x20_is_b(ow, rom_id)", ow_ds18x20_is_b(ow, rom_id));

    ow_protect(ow, 1);
    res = ow_ds18x20_set_resolution_raw(ow, rom_id, bits);
    ow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Set/clear temperature alarm high/low levels in units of degree Celcius
 * \note            `temp_h` and `temp_l` are high and low temperature alarms and can accept different values:
 *                      - `-55 % 125`, valid temperature range
 *                      - \ref OW_DS18X20_ALARM_DISABLE to disable temperature alarm (either high or low)
 *                      - \ref OW_DS18X20_ALARM_NOCHANGE to keep current alarm temperature (either high or low)
 *
 * Example usage would look something similar to:
 * \code{c}
//Set alarm temperature; low = 10°C, high = 30°C
ow_ds18x20_set_alarm_temp(&ow, dev_id, 10, 30);
//Set alarm temperature; low = disable, high = no change
ow_ds18x20_set_alarm_temp(&ow, dev_id, OW_DS18X20_ALARM_DISABLE, OW_DS18X20_ALARM_NOCHANGE);
//Set alarm temperature; low = no change, high = disable
ow_ds18x20_set_alarm_temp(&ow, dev_id, OW_DS18X20_ALARM_NOCHANGE, OW_DS18X20_ALARM_DISABLE);
//Set alarm temperature; low = 10°C, high = 30°C
ow_ds18x20_set_alarm_temp(&ow, dev_id, 10, 30);
\endcode
 *
 *
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address
 * \param[in]       temp_l: Alarm low temperature
 * \param[in]       temp_h: Alarm high temperature
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ds18x20_set_alarm_temp_raw(ow_t* const ow, const ow_rom_t* const rom_id, int8_t temp_l, int8_t temp_h) {
    uint8_t res = 0, conf, th, tl;

    OW_ASSERT0("ow != NULL", ow != NULL);
    OW_ASSERT0("ow_ds18x20_is_b(ow, rom_id)", ow_ds18x20_is_b(ow, rom_id));

    /* Check if there is need to do anything */
    if (temp_l == OW_DS18X20_ALARM_NOCHANGE && temp_h == OW_DS18X20_ALARM_NOCHANGE) {
        return 1;
    }

    /* Check valid temp range for temperatures */
    if (temp_l != OW_DS18X20_ALARM_NOCHANGE) {
        if (temp_l == OW_DS18X20_ALARM_DISABLE || temp_l < OW_DS18X20_TEMP_MIN) {
            temp_l = OW_DS18X20_TEMP_MIN;       /* Set alarm to minumum vlaue */
        } else if (temp_l > OW_DS18X20_TEMP_MAX) {
            temp_l = OW_DS18X20_TEMP_MAX;
        }
    }
    if (temp_h != OW_DS18X20_ALARM_NOCHANGE) {
        if (temp_h == OW_DS18X20_ALARM_DISABLE || temp_h > OW_DS18X20_TEMP_MAX) {
            temp_h = OW_DS18X20_TEMP_MAX;       /* Set alarm to maximal value */
        } else if (temp_h < OW_DS18X20_TEMP_MIN) {
            temp_h = OW_DS18X20_TEMP_MIN;
        }
    }

    if (ow_reset_raw(ow) == owOK) {
        if (rom_id == NULL) {
            ow_skip_rom_raw(ow);
        } else {
            ow_match_rom_raw(ow, rom_id);
        }
        ow_write_byte_raw(ow, OW_CMD_RSCRATCHPAD);

        /* Read and ignore 2 bytes */
        ow_read_byte_raw(ow);
        ow_read_byte_raw(ow);

        /* Read important data */
        th = ow_read_byte_raw(ow);
        tl = ow_read_byte_raw(ow);
        conf = ow_read_byte_raw(ow);

        /* Fill new values */
        th = temp_h == OW_DS18X20_ALARM_NOCHANGE ? (uint8_t)th : (uint8_t)temp_h;
        tl = temp_l == OW_DS18X20_ALARM_NOCHANGE ? (uint8_t)tl : (uint8_t)temp_l;

        /* Write scratchpad */
        if (ow_reset_raw(ow) == owOK) {
            ow_match_rom_raw(ow, rom_id);
            ow_write_byte_raw(ow, OW_CMD_WSCRATCHPAD);

            /* Write configuration register */
            ow_write_byte_raw(ow, th);
            ow_write_byte_raw(ow, tl);
            ow_write_byte_raw(ow, conf);

            /* Copy scratchpad to memory */
            if (ow_reset_raw(ow) == owOK) {
                ow_match_rom_raw(ow, rom_id);
                ow_write_byte_raw(ow, OW_CMD_CPYSCRATCHPAD);

                res = 1;
            }
        }
    }
    return res;
}

/**
 * \copydoc         ow_ds18x20_set_alarm_temp_raw
 * \note            This function is thread-safe
 */
uint8_t
ow_ds18x20_set_alarm_temp(ow_t* const ow, const ow_rom_t* const rom_id, int8_t temp_l, int8_t temp_h) {
    uint8_t res;

    OW_ASSERT0("ow != NULL", ow != NULL);
    OW_ASSERT0("ow_ds18x20_is_b(ow, rom_id)", ow_ds18x20_is_b(ow, rom_id));

    ow_protect(ow, 1);
    res = ow_ds18x20_set_alarm_temp_raw(ow, rom_id, temp_l, temp_h);
    ow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Search for `DS18x20` devices with alarm flag
 * \note            To reset search, use \ref ow_search_reset function
 * \param[in]       ow: 1-Wire handle
 * \param[out]      rom_id: Pointer to 8-byte long variable to save ROM
 * \return          \ref owOK on success, member of \ref owr_t otherwise
 */
owr_t
ow_ds18x20_search_alarm_raw(ow_t* const ow, ow_rom_t* const rom_id) {
    return ow_search_with_command_raw(ow, 0xEC, rom_id);
}

/**
 * \copydoc         ow_ds18x20_search_alarm_raw
 * \note            This function is thread-safe
 */
owr_t
ow_ds18x20_search_alarm(ow_t* const ow, ow_rom_t* const rom_id) {
    owr_t res;

    OW_ASSERT("ow != NULL", ow != NULL);

    ow_protect(ow, 1);
    res = ow_ds18x20_search_alarm_raw(ow, rom_id);
    ow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Check if ROM address matches `DS18B20` device
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to test against `DS18B20`
 * \return          `1` on success, `0` otherwise
 * \note            This function is reentrant
 */
uint8_t
ow_ds18x20_is_b(ow_t* const ow, const ow_rom_t* const rom_id) {
    OW_ASSERT0("ow != NULL", ow != NULL);
    OW_ASSERT0("rom_id != NULL", rom_id != NULL);

    OW_UNUSED(ow);
    return rom_id->rom[0] == 0x28;          /* Check for correct ROM family code */
}

/**
 * \brief           Check if ROM address matches `DS18S20` device
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to test against `DS18S20`
 * \return          `1` on success, `0` otherwise
 * \note            This function is reentrant
 */
uint8_t
ow_ds18x20_is_s(ow_t* const ow, const ow_rom_t* const rom_id) {
    OW_ASSERT0("ow != NULL", ow != NULL);
    OW_ASSERT0("rom_id != NULL", rom_id != NULL);

    OW_UNUSED(ow);
    return rom_id->rom[0] == 0x10;/* Check for correct ROM family code */
}
