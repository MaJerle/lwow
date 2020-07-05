/**
 * \file            lwow_device_ds18x20.h
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
 * This file is part of LwOW - Lightweight onewire library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v3.0.0
 */
#include "lwow/lwow.h"
#include "lwow/devices/lwow_device_ds18x20.h"

/**
 * \brief           Start temperature conversion on specific (or all) devices
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to start measurement for.
 *                      Set to `NULL` to start measurement on all devices at the same time
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwow_ds18x20_start_raw(lwow_t* const ow, const lwow_rom_t* const rom_id) {
    uint8_t ret = 0;

    LWOW_ASSERT0("ow != NULL", ow != NULL);

    if (lwow_reset_raw(ow) == lwowOK) {
        if (rom_id == NULL) {                   /* Check for ROM id */
            lwow_skip_rom_raw(ow);              /* Skip ROM, send to all devices */
        } else {
            lwow_match_rom_raw(ow, rom_id);     /* Select exact device by ROM address */
        }
        lwow_write_byte_ex_raw(ow, 0x44, NULL); /* Start temperature conversion */
        ret = 1;
    }
    return ret;
}

/**
 * \copydoc         lwow_ds18x20_start_raw
 * \note            This function is thread-safe
 */
uint8_t
lwow_ds18x20_start(lwow_t* const ow, const lwow_rom_t* const rom_id) {
    uint8_t res;

    LWOW_ASSERT0("ow != NULL", ow != NULL);

    lwow_protect(ow, 1);
    res = lwow_ds18x20_start_raw(ow, rom_id);
    lwow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Read temperature previously started with \ref lwow_ds18x20_start
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to read data from
 * \param[out]      t: Pointer to output float variable to save temperature
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwow_ds18x20_read_raw(lwow_t* const ow, const lwow_rom_t* const rom_id, float* const t) {
    float dec;
    uint16_t temp;
    uint8_t ret = 0, data[9], crc, resolution, m = 0, bit_val;
    int8_t digit;

    LWOW_ASSERT0("ow != NULL", ow != NULL);
    LWOW_ASSERT0("t != NULL", t != NULL);
    LWOW_ASSERT0("lwow_ds18x20_is_b(ow, rom_id) || lwow_ds18x20_is_s(ow, rom_id)", lwow_ds18x20_is_b(ow, rom_id) || lwow_ds18x20_is_s(ow, rom_id));

    /*
     * First read bit and check if all devices completed with conversion.
     * If everything ready, try to reset the network and continue
     */
    if (lwow_read_bit_ex_raw(ow, &bit_val) == lwowOK && bit_val != 0 && lwow_reset_raw(ow) == lwowOK) {
        if (rom_id == NULL) {
            lwow_skip_rom_raw(ow);
        } else {
            lwow_match_rom_raw(ow, rom_id);
        }
        lwow_write_byte_ex_raw(ow, LWOW_CMD_RSCRATCHPAD, NULL);

        /* Read plain data from device */
        for (uint8_t i = 0; i < 9; ++i) {
            lwow_read_byte_ex_raw(ow, &data[i]);
        }
        crc = lwow_crc(data, 0x09);             /* Calculate CRC */
        if (crc == 0) {                         /* Result must be 0 to match the CRC */
            temp = (data[1] << 0x08) | data[0]; /* Format data in integer format */
            resolution = ((data[4] & 0x60) >> 0x05) + 0x09; /* Set resolution in units of bits */
            if (temp & 0x8000) {                /* Check for negative temperature */
                temp = ~temp + 1;               /* Perform two's complement */
                m = 1;
            }
            digit = (temp >> 0x04) | (((temp >> 0x08) & 0x07) << 0x04);
            switch (resolution) {               /* Check for resolution settings */
                case 9:
                    dec = ((temp >> 0x03) & 0x01) * 0.5f;
                    break;
                case 10:
                    dec = ((temp >> 0x02) & 0x03) * 0.25f;
                    break;
                case 11:
                    dec = ((temp >> 0x01) & 0x07) * 0.125f;
                    break;
                case 12:
                    dec = (temp & 0x0F) * 0.0625f;
                    break;
                default:
                    dec = 0xFF, digit = 0;
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
 * \copydoc         lwow_ds18x20_read_raw
 * \note            This function is thread-safe
 */
uint8_t
lwow_ds18x20_read(lwow_t* const ow, const lwow_rom_t* const rom_id, float* const t) {
    uint8_t res;

    LWOW_ASSERT0("ow != NULL", ow != NULL);
    LWOW_ASSERT0("t != NULL", t != NULL);
    LWOW_ASSERT0("lwow_ds18x20_is_b(ow, rom_id) || lwow_ds18x20_is_s(ow, rom_id)", lwow_ds18x20_is_b(ow, rom_id) || lwow_ds18x20_is_s(ow, rom_id));

    lwow_protect(ow, 1);
    res = lwow_ds18x20_read_raw(ow, rom_id, t);
    lwow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Get resolution for `DS18B20` device
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to get resolution from
 * \return          Resolution in units of bits (`9 - 12`) on success, `0` otherwise
 */
uint8_t
lwow_ds18x20_get_resolution_raw(lwow_t* const ow, const lwow_rom_t* const rom_id) {
    uint8_t res = 0, br;

    LWOW_ASSERT0("ow != NULL", ow != NULL);
    LWOW_ASSERT0("rom_id != NULL", rom_id != NULL);
    LWOW_ASSERT0("lwow_ds18x20_is_b(ow, rom_id)", lwow_ds18x20_is_b(ow, rom_id));

    if (lwow_reset_raw(ow) == lwowOK) {         /* Reset bus */
        lwow_match_rom_raw(ow, rom_id);         /* Select device */
        lwow_write_byte_ex_raw(ow, LWOW_CMD_RSCRATCHPAD, NULL);

        /* Read and ignore bytes */
        lwow_read_byte_ex_raw(ow, &br);
        lwow_read_byte_ex_raw(ow, &br);
        lwow_read_byte_ex_raw(ow, &br);
        lwow_read_byte_ex_raw(ow, &br);

        lwow_read_byte_ex_raw(ow, &br);
        res = ((br & 0x60) >> 0x05) + 9;  /* Read configuration byte and calculate bits */
    }

    return res;
}

/**
 * \copydoc         lwow_ds18x20_get_resolution_raw
 * \note            This function is thread-safe
 */
uint8_t
lwow_ds18x20_get_resolution(lwow_t* const ow, const lwow_rom_t* const rom_id) {
    uint8_t res;

    LWOW_ASSERT0("ow != NULL", ow != NULL);
    LWOW_ASSERT0("rom_id != NULL", rom_id != NULL);
    LWOW_ASSERT0("lwow_ds18x20_is_b(ow, rom_id)", lwow_ds18x20_is_b(ow, rom_id));

    lwow_protect(ow, 1);
    res = lwow_ds18x20_get_resolution_raw(ow, rom_id);
    lwow_unprotect(ow, 1);
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
lwow_ds18x20_set_resolution_raw(lwow_t* const ow, const lwow_rom_t* const rom_id, const uint8_t bits) {
    uint8_t th, tl, conf, res = 0;

    LWOW_ASSERT0("ow != NULL", ow != NULL);
    LWOW_ASSERT0("bits >= 9 && bits <= 12", bits >= 9 && bits <= 12);
    LWOW_ASSERT0("lwow_ds18x20_is_b(ow, rom_id)", lwow_ds18x20_is_b(ow, rom_id));

    if (lwow_reset_raw(ow) == lwowOK) {
        if (rom_id == NULL) {
            lwow_match_rom_raw(ow, rom_id);
        } else {
            lwow_skip_rom_raw(ow);
        }
        lwow_write_byte_ex_raw(ow, LWOW_CMD_RSCRATCHPAD, NULL);

        /* Read and ignore bytes */
        lwow_read_byte_ex_raw(ow, &th);
        lwow_read_byte_ex_raw(ow, &th);

        /* Read important data */
        lwow_read_byte_ex_raw(ow, &th);
        lwow_read_byte_ex_raw(ow, &tl);
        lwow_read_byte_ex_raw(ow, &conf);

        conf &= ~0x60;                          /* Remove configuration bits for temperature resolution */
        switch (bits) {
            case 12:
                conf |= 0x60;
                break;
            case 11:
                conf |= 0x40;
                break;
            case 10:
                conf |= 0x20;
                break;
            case 9:
            default:
                break;
        }

        /* Write data back to device */
        if (lwow_reset_raw(ow) == lwowOK) {
            lwow_match_rom_raw(ow, rom_id);
            lwow_write_byte_ex_raw(ow, LWOW_CMD_WSCRATCHPAD, NULL);

            lwow_write_byte_ex_raw(ow, th, NULL);
            lwow_write_byte_ex_raw(ow, tl, NULL);
            lwow_write_byte_ex_raw(ow, conf, NULL);

            /* Copy scratchpad to non-volatile memory */
            if (lwow_reset_raw(ow) == lwowOK) {
                lwow_match_rom_raw(ow, rom_id);
                lwow_write_byte_ex_raw(ow, LWOW_CMD_CPYSCRATCHPAD, NULL);
                res = 1;
            }
        }
    }
    return res;
}

/**
 * \copydoc         lwow_ds18x20_set_resolution_raw
 * \note            This function is thread-safe
 */
uint8_t
lwow_ds18x20_set_resolution(lwow_t* const ow, const lwow_rom_t* const rom_id, const uint8_t bits) {
    uint8_t res;

    LWOW_ASSERT0("ow != NULL", ow != NULL);
    LWOW_ASSERT0("bits >= 9 && bits <= 12", bits >= 9 && bits <= 12);
    LWOW_ASSERT0("lwow_ds18x20_is_b(ow, rom_id)", lwow_ds18x20_is_b(ow, rom_id));

    lwow_protect(ow, 1);
    res = lwow_ds18x20_set_resolution_raw(ow, rom_id, bits);
    lwow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Set/clear temperature alarm high/low levels in units of degree Celcius
 * \note            `temp_h` and `temp_l` are high and low temperature alarms and can accept different values:
 *                      - `-55 % 125`, valid temperature range
 *                      - \ref LWOW_DS18X20_ALARM_DISABLE to disable temperature alarm (either high or low)
 *                      - \ref LWOW_DS18X20_ALARM_NOCHANGE to keep current alarm temperature (either high or low)
 *
 * Example usage would look something similar to:
 * \code{c}
//Set alarm temperature; low = 10°C, high = 30°C
lwow_ds18x20_set_alarm_temp(&ow, dev_id, 10, 30);
//Set alarm temperature; low = disable, high = no change
lwow_ds18x20_set_alarm_temp(&ow, dev_id, LWOW_DS18X20_ALARM_DISABLE, LWOW_DS18X20_ALARM_NOCHANGE);
//Set alarm temperature; low = no change, high = disable
lwow_ds18x20_set_alarm_temp(&ow, dev_id, LWOW_DS18X20_ALARM_NOCHANGE, LWOW_DS18X20_ALARM_DISABLE);
//Set alarm temperature; low = 10°C, high = 30°C
lwow_ds18x20_set_alarm_temp(&ow, dev_id, 10, 30);
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
lwow_ds18x20_set_alarm_temp_raw(lwow_t* const ow, const lwow_rom_t* const rom_id, int8_t temp_l, int8_t temp_h) {
    uint8_t res = 0, conf, th, tl;

    LWOW_ASSERT0("ow != NULL", ow != NULL);
    LWOW_ASSERT0("lwow_ds18x20_is_b(ow, rom_id)", lwow_ds18x20_is_b(ow, rom_id));

    /* Check if there is need to do anything */
    if (temp_l == LWOW_DS18X20_ALARM_NOCHANGE && temp_h == LWOW_DS18X20_ALARM_NOCHANGE) {
        return 1;
    }

    /* Check valid temp range for temperatures */
    if (temp_l != LWOW_DS18X20_ALARM_NOCHANGE) {
        if (temp_l == LWOW_DS18X20_ALARM_DISABLE || temp_l < LWOW_DS18X20_TEMP_MIN) {
            temp_l = LWOW_DS18X20_TEMP_MIN;
        } else if (temp_l > LWOW_DS18X20_TEMP_MAX) {
            temp_l = LWOW_DS18X20_TEMP_MAX;
        }
    }
    if (temp_h != LWOW_DS18X20_ALARM_NOCHANGE) {
        if (temp_h == LWOW_DS18X20_ALARM_DISABLE || temp_h > LWOW_DS18X20_TEMP_MAX) {
            temp_h = LWOW_DS18X20_TEMP_MAX;
        } else if (temp_h < LWOW_DS18X20_TEMP_MIN) {
            temp_h = LWOW_DS18X20_TEMP_MIN;
        }
    }

    if (lwow_reset_raw(ow) == lwowOK) {
        if (rom_id == NULL) {
            lwow_skip_rom_raw(ow);
        } else {
            lwow_match_rom_raw(ow, rom_id);
        }
        lwow_write_byte_ex_raw(ow, LWOW_CMD_RSCRATCHPAD, NULL);

        /* Read and ignore 2 bytes */
        lwow_read_byte_ex_raw(ow, &th);
        lwow_read_byte_ex_raw(ow, &th);

        /* Read important data */
        lwow_read_byte_ex_raw(ow, &th);
        lwow_read_byte_ex_raw(ow, &tl);
        lwow_read_byte_ex_raw(ow, &conf);

        /* Fill new values */
        th = temp_h == LWOW_DS18X20_ALARM_NOCHANGE ? (uint8_t)th : (uint8_t)temp_h;
        tl = temp_l == LWOW_DS18X20_ALARM_NOCHANGE ? (uint8_t)tl : (uint8_t)temp_l;

        /* Write scratchpad */
        if (lwow_reset_raw(ow) == lwowOK) {
            lwow_match_rom_raw(ow, rom_id);
            lwow_write_byte_ex_raw(ow, LWOW_CMD_WSCRATCHPAD, NULL);

            /* Write configuration register */
            lwow_write_byte_ex_raw(ow, th, NULL);
            lwow_write_byte_ex_raw(ow, tl, NULL);
            lwow_write_byte_ex_raw(ow, conf, NULL);

            /* Copy scratchpad to memory */
            if (lwow_reset_raw(ow) == lwowOK) {
                lwow_match_rom_raw(ow, rom_id);
                lwow_write_byte_ex_raw(ow, LWOW_CMD_CPYSCRATCHPAD, NULL);

                res = 1;
            }
        }
    }
    return res;
}

/**
 * \copydoc         lwow_ds18x20_set_alarm_temp_raw
 * \note            This function is thread-safe
 */
uint8_t
lwow_ds18x20_set_alarm_temp(lwow_t* const ow, const lwow_rom_t* const rom_id, int8_t temp_l, int8_t temp_h) {
    uint8_t res;

    LWOW_ASSERT0("ow != NULL", ow != NULL);
    LWOW_ASSERT0("lwow_ds18x20_is_b(ow, rom_id)", lwow_ds18x20_is_b(ow, rom_id));

    lwow_protect(ow, 1);
    res = lwow_ds18x20_set_alarm_temp_raw(ow, rom_id, temp_l, temp_h);
    lwow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Search for `DS18x20` devices with alarm flag
 * \note            To reset search, use \ref lwow_search_reset function
 * \param[in]       ow: 1-Wire handle
 * \param[out]      rom_id: Pointer to 8-byte long variable to save ROM
 * \return          \ref lwowOK on success, member of \ref lwowr_t otherwise
 */
lwowr_t
lwow_ds18x20_search_alarm_raw(lwow_t* const ow, lwow_rom_t* const rom_id) {
    return lwow_search_with_command_raw(ow, 0xEC, rom_id);
}

/**
 * \copydoc         lwow_ds18x20_search_alarm_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_ds18x20_search_alarm(lwow_t* const ow, lwow_rom_t* const rom_id) {
    lwowr_t res;

    LWOW_ASSERT("ow != NULL", ow != NULL);

    lwow_protect(ow, 1);
    res = lwow_ds18x20_search_alarm_raw(ow, rom_id);
    lwow_unprotect(ow, 1);
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
lwow_ds18x20_is_b(lwow_t* const ow, const lwow_rom_t* const rom_id) {
    LWOW_ASSERT0("ow != NULL", ow != NULL);
    LWOW_ASSERT0("rom_id != NULL", rom_id != NULL);

    LWOW_UNUSED(ow);
    return rom_id->rom[0] == 0x28;
}

/**
 * \brief           Check if ROM address matches `DS18S20` device
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to test against `DS18S20`
 * \return          `1` on success, `0` otherwise
 * \note            This function is reentrant
 */
uint8_t
lwow_ds18x20_is_s(lwow_t* const ow, const lwow_rom_t* const rom_id) {
    LWOW_ASSERT0("ow != NULL", ow != NULL);
    LWOW_ASSERT0("rom_id != NULL", rom_id != NULL);

    LWOW_UNUSED(ow);
    return rom_id->rom[0] == 0x10;
}
