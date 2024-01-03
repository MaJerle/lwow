/**
 * \file            lwow_device_ds18x20.h
 * \brief           DS18x20 driver implementation
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
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "lwow/devices/lwow_device_ds18x20.h"
#include "lwow/lwow.h"

#define LWOW_DS18B20_FAMILY_CODE 0x28U
#define LWOW_DS18S20_FAMILY_CODE 0x10U

/**
 * \brief           Start temperature conversion on specific (or all) devices
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to start measurement for.
 *                      Set to `NULL` to start measurement on all devices at the same time
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwow_ds18x20_start_raw(lwow_t* const owobj, const lwow_rom_t* const rom_id) {
    uint8_t res = 0;

    LWOW_ASSERT0("owobj != NULL", owobj != NULL);

    if (lwow_reset_raw(owobj) == lwowOK && lwow_match_or_skip_rom_raw(owobj, rom_id) == lwowOK) {
        lwow_write_byte_ex_raw(owobj, LWOW_DS18X20_CMD_CONVERT_T, NULL); /* Start temperature conversion */
        res = 1;
    }
    return res;
}

/**
 * \copydoc         lwow_ds18x20_start_raw
 * \note            This function is thread-safe
 */
uint8_t
lwow_ds18x20_start(lwow_t* const owobj, const lwow_rom_t* const rom_id) {
    uint8_t res = 0;

    LWOW_ASSERT0("owobj != NULL", owobj != NULL);

    lwow_protect(owobj, 1);
    res = lwow_ds18x20_start_raw(owobj, rom_id);
    lwow_unprotect(owobj, 1);
    return res;
}

/**
 * \brief           Read temperature previously started with \ref lwow_ds18x20_start
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to read data from
 * \param[out]      temp_out: Pointer to output float variable to save temperature
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwow_ds18x20_read_raw(lwow_t* const owobj, const lwow_rom_t* const rom_id, float* const temp_out) {
    float dec = 0.0f;
    uint16_t temp = 0;
    uint8_t ret = 0, data[9] = {0}, crc = 0, resolution = 0, m = 0, bit_val = 0;
    int8_t digit = 0;

    LWOW_ASSERT0("owobj != NULL", owobj != NULL);
    LWOW_ASSERT0("temp_out != NULL", temp_out != NULL);
    if (rom_id != NULL) {
        LWOW_ASSERT0("lwow_ds18x20_is_b(owobj, rom_id) || lwow_ds18x20_is_s(owobj, rom_id)",
                     lwow_ds18x20_is_b(owobj, rom_id) || lwow_ds18x20_is_s(owobj, rom_id));
    }

    /*
     * First read bit and check if all devices completed with conversion.
     * If everything ready, try to reset the network and continue
     */
    if (lwow_read_bit_ex_raw(owobj, &bit_val) == lwowOK && bit_val != 0 && lwow_reset_raw(owobj) == lwowOK
        && lwow_match_or_skip_rom_raw(owobj, rom_id) == lwowOK) {
        lwow_write_byte_ex_raw(owobj, LWOW_CMD_RSCRATCHPAD, NULL);

        /* Read plain data from device */
        for (uint8_t idx = 0; idx < LWOW_ARRAYSIZE(data); ++idx) {
            lwow_read_byte_ex_raw(owobj, &data[idx]);
        }
        crc = lwow_crc(data, LWOW_ARRAYSIZE(data));            /* Calculate CRC */
        if (crc == 0) {                                        /* Result must be 0 to match the CRC */
            temp = (data[1] << 0x08U) | data[0];               /* Format data in integer format */
            resolution = ((data[4] & 0x60U) >> 0x05U) + 0x09U; /* Set resolution in units of bits */
            if (temp & 0x8000U) {                              /* Check for negative temperature */
                temp = ~temp + 1;                              /* Perform two's complement */
                m = 1;
            }
            digit = (temp >> 0x04U) | (((temp >> 0x08U) & 0x07U) << 0x04U);
            switch (resolution) { /* Check for resolution settings */
                case 9U: dec = ((temp >> 0x03U) & 0x01U) * 0.5f; break;
                case 10U: dec = ((temp >> 0x02U) & 0x03U) * 0.25f; break;
                case 11U: dec = ((temp >> 0x01U) & 0x07U) * 0.125f; break;
                case 12U: dec = (temp & 0x0FU) * 0.0625f; break;
                default: dec = 0xFFU, digit = 0;
            }
            dec += digit;
            if (m) {
                dec = -dec;
            }
            *temp_out = dec;
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
lwow_ds18x20_read(lwow_t* const owobj, const lwow_rom_t* const rom_id, float* const t) {
    uint8_t res = 0;

    LWOW_ASSERT0("owobj != NULL", owobj != NULL);
    LWOW_ASSERT0("t != NULL", t != NULL);
    if (rom_id != NULL) {
        LWOW_ASSERT0("lwow_ds18x20_is_b(owobj, rom_id) || lwow_ds18x20_is_s(owobj, rom_id)",
                     lwow_ds18x20_is_b(owobj, rom_id) || lwow_ds18x20_is_s(owobj, rom_id));
    }

    lwow_protect(owobj, 1);
    res = lwow_ds18x20_read_raw(owobj, rom_id, t);
    lwow_unprotect(owobj, 1);
    return res;
}

/**
 * \brief           Get resolution for `DS18B20` device
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to get resolution from
 * \return          Resolution in units of bits (`9 - 12`) on success, `0` otherwise
 */
uint8_t
lwow_ds18x20_get_resolution_raw(lwow_t* const owobj, const lwow_rom_t* const rom_id) {
    uint8_t res = 0, bresol = 0;

    LWOW_ASSERT0("owobj != NULL", owobj != NULL);
    LWOW_ASSERT0("rom_id != NULL", rom_id != NULL);
    LWOW_ASSERT0("lwow_ds18x20_is_b(owobj, rom_id)", lwow_ds18x20_is_b(owobj, rom_id));

    if (lwow_reset_raw(owobj) == lwowOK && lwow_match_or_skip_rom_raw(owobj, rom_id) == lwowOK) {
        lwow_write_byte_ex_raw(owobj, LWOW_CMD_RSCRATCHPAD, NULL);

        /* Read and ignore bytes */
        lwow_read_byte_ex_raw(owobj, &bresol);
        lwow_read_byte_ex_raw(owobj, &bresol);
        lwow_read_byte_ex_raw(owobj, &bresol);
        lwow_read_byte_ex_raw(owobj, &bresol);

        lwow_read_byte_ex_raw(owobj, &bresol);
        res = ((bresol & 0x60U) >> 0x05U) + 9U; /* Read configuration byte and calculate bits */
    }

    return res;
}

/**
 * \copydoc         lwow_ds18x20_get_resolution_raw
 * \note            This function is thread-safe
 */
uint8_t
lwow_ds18x20_get_resolution(lwow_t* const owobj, const lwow_rom_t* const rom_id) {
    uint8_t res = 0;

    LWOW_ASSERT0("owobj != NULL", owobj != NULL);
    LWOW_ASSERT0("rom_id != NULL", rom_id != NULL);
    LWOW_ASSERT0("lwow_ds18x20_is_b(owobj, rom_id)", lwow_ds18x20_is_b(owobj, rom_id));

    lwow_protect(owobj, 1);
    res = lwow_ds18x20_get_resolution_raw(owobj, rom_id);
    lwow_unprotect(owobj, 1);
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
lwow_ds18x20_set_resolution_raw(lwow_t* const owobj, const lwow_rom_t* const rom_id, const uint8_t bits) {
    uint8_t thigh = 0, tlow = 0, conf = 0, res = 0;

    LWOW_ASSERT0("owobj != NULL", owobj != NULL);
    LWOW_ASSERT0("bits >= 9U && bits <= 12U", bits >= 9U && bits <= 12U);
    LWOW_ASSERT0("lwow_ds18x20_is_b(owobj, rom_id)", lwow_ds18x20_is_b(owobj, rom_id));

    if (lwow_reset_raw(owobj) == lwowOK && lwow_match_or_skip_rom_raw(owobj, rom_id) == lwowOK) {
        lwow_write_byte_ex_raw(owobj, LWOW_CMD_RSCRATCHPAD, NULL);

        /* Read and ignore bytes */
        lwow_read_byte_ex_raw(owobj, &thigh);
        lwow_read_byte_ex_raw(owobj, &thigh);

        /* Read important data */
        lwow_read_byte_ex_raw(owobj, &thigh);
        lwow_read_byte_ex_raw(owobj, &tlow);
        lwow_read_byte_ex_raw(owobj, &conf);

        conf &= ~0x60U; /* Remove configuration bits for temperature resolution */
        switch (bits) {
            case 12U: conf |= 0x60U; break;
            case 11U: conf |= 0x40U; break;
            case 10U: conf |= 0x20U; break;
            case 9U:
            default: break;
        }

        /* Write data back to device */
        if (lwow_reset_raw(owobj) == lwowOK && lwow_match_or_skip_rom_raw(owobj, rom_id) == lwowOK) {
            lwow_write_byte_ex_raw(owobj, LWOW_CMD_WSCRATCHPAD, NULL);

            lwow_write_byte_ex_raw(owobj, thigh, NULL);
            lwow_write_byte_ex_raw(owobj, tlow, NULL);
            lwow_write_byte_ex_raw(owobj, conf, NULL);

            /* Copy scratchpad to non-volatile memory */
            if (lwow_reset_raw(owobj) == lwowOK && lwow_match_or_skip_rom_raw(owobj, rom_id) == lwowOK) {
                lwow_write_byte_ex_raw(owobj, LWOW_CMD_CPYSCRATCHPAD, NULL);
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
lwow_ds18x20_set_resolution(lwow_t* const owobj, const lwow_rom_t* const rom_id, const uint8_t bits) {
    uint8_t res = 0;

    LWOW_ASSERT0("owobj != NULL", owobj != NULL);
    LWOW_ASSERT0("bits >= 9U && bits <= 12U", bits >= 9U && bits <= 12U);
    LWOW_ASSERT0("lwow_ds18x20_is_b(owobj, rom_id)", lwow_ds18x20_is_b(owobj, rom_id));

    lwow_protect(owobj, 1);
    res = lwow_ds18x20_set_resolution_raw(owobj, rom_id, bits);
    lwow_unprotect(owobj, 1);
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
lwow_ds18x20_set_alarm_temp_raw(lwow_t* const owobj, const lwow_rom_t* const rom_id, int8_t temp_l, int8_t temp_h) {
    uint8_t res = 0, conf = 0, thigh = 0, tlow = 0;

    LWOW_ASSERT0("owobj != NULL", owobj != NULL);
    LWOW_ASSERT0("lwow_ds18x20_is_b(owobj, rom_id)", lwow_ds18x20_is_b(owobj, rom_id));

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

    if (lwow_reset_raw(owobj) == lwowOK && lwow_match_or_skip_rom_raw(owobj, rom_id) == lwowOK) {
        lwow_write_byte_ex_raw(owobj, LWOW_CMD_RSCRATCHPAD, NULL);

        /* Read and ignore 2 bytes */
        lwow_read_byte_ex_raw(owobj, &thigh);
        lwow_read_byte_ex_raw(owobj, &thigh);

        /* Read important data */
        lwow_read_byte_ex_raw(owobj, &thigh);
        lwow_read_byte_ex_raw(owobj, &tlow);
        lwow_read_byte_ex_raw(owobj, &conf);

        /* Fill new values */
        thigh = temp_h == LWOW_DS18X20_ALARM_NOCHANGE ? (uint8_t)thigh : (uint8_t)temp_h;
        tlow = temp_l == LWOW_DS18X20_ALARM_NOCHANGE ? (uint8_t)tlow : (uint8_t)temp_l;

        /* Write scratchpad */
        if (lwow_reset_raw(owobj) == lwowOK && lwow_match_or_skip_rom_raw(owobj, rom_id) == lwowOK) {
            lwow_write_byte_ex_raw(owobj, LWOW_CMD_WSCRATCHPAD, NULL);

            /* Write configuration register */
            lwow_write_byte_ex_raw(owobj, thigh, NULL);
            lwow_write_byte_ex_raw(owobj, tlow, NULL);
            lwow_write_byte_ex_raw(owobj, conf, NULL);

            /* Copy scratchpad to memory */
            if (lwow_reset_raw(owobj) == lwowOK && lwow_match_or_skip_rom_raw(owobj, rom_id) == lwowOK) {
                lwow_write_byte_ex_raw(owobj, LWOW_CMD_CPYSCRATCHPAD, NULL);

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
lwow_ds18x20_set_alarm_temp(lwow_t* const owobj, const lwow_rom_t* const rom_id, int8_t temp_l, int8_t temp_h) {
    uint8_t res = 0;

    LWOW_ASSERT0("owobj != NULL", owobj != NULL);
    LWOW_ASSERT0("lwow_ds18x20_is_b(owobj, rom_id)", lwow_ds18x20_is_b(owobj, rom_id));

    lwow_protect(owobj, 1);
    res = lwow_ds18x20_set_alarm_temp_raw(owobj, rom_id, temp_l, temp_h);
    lwow_unprotect(owobj, 1);
    return res;
}

/**
 * \brief           Get the low and high temperature triggers for the alarm configuration
 * 
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address
 * \param[out]      temp_l: Pointer to output variable to write low temperature alarm trigger
 * \param[out]      temp_h: Pointer to output variable to write high temperature alarm trigger
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwow_ds18x20_get_alarm_temp_raw(lwow_t* const owobj, const lwow_rom_t* const rom_id, int8_t* temp_l, int8_t* temp_h) {
    uint8_t res = 0, thigh = 0, tlow = 0;

    LWOW_ASSERT0("owobj != NULL", owobj != NULL);
    LWOW_ASSERT0("lwow_ds18x20_is_b(owobj, rom_id)", lwow_ds18x20_is_b(owobj, rom_id));
    LWOW_ASSERT0("temp_l != NULL || temp_h != NULL", temp_l != NULL || temp_h != NULL);

    if (lwow_reset_raw(owobj) == lwowOK && lwow_match_or_skip_rom_raw(owobj, rom_id) == lwowOK) {
        lwow_write_byte_ex_raw(owobj, LWOW_CMD_RSCRATCHPAD, NULL);

        /* Read and ignore 2 bytes */
        lwow_read_byte_ex_raw(owobj, &thigh);
        lwow_read_byte_ex_raw(owobj, &thigh);

        /* Read important data */
        lwow_read_byte_ex_raw(owobj, &thigh);
        lwow_read_byte_ex_raw(owobj, &tlow);

        if (temp_l != NULL) {
            *temp_l = (int8_t)tlow;
        }
        if (temp_h != NULL) {
            *temp_h = (int8_t)thigh;
        }
        res = 1;
    }
    return res;
}

/**
 * \copydoc         lwow_ds18x20_get_alarm_temp_raw
 * \note            This function is thread-safe
 */
uint8_t
lwow_ds18x20_get_alarm_temp(lwow_t* const owobj, const lwow_rom_t* const rom_id, int8_t* temp_l, int8_t* temp_h) {
    uint8_t res = 0;

    LWOW_ASSERT0("owobj != NULL", owobj != NULL);
    LWOW_ASSERT0("lwow_ds18x20_is_b(owobj, rom_id)", lwow_ds18x20_is_b(owobj, rom_id));
    LWOW_ASSERT0("temp_l != NULL || temp_h != NULL", temp_l != NULL || temp_h != NULL);

    lwow_protect(owobj, 1);
    res = lwow_ds18x20_get_alarm_temp_raw(owobj, rom_id, temp_l, temp_h);
    lwow_unprotect(owobj, 1);
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
lwow_ds18x20_search_alarm_raw(lwow_t* const owobj, lwow_rom_t* const rom_id) {
    return lwow_search_with_command_raw(owobj, LWOW_DS18X20_CMD_ALARM_SEARCH, rom_id);
}

/**
 * \copydoc         lwow_ds18x20_search_alarm_raw
 * \note            This function is thread-safe
 */
lwowr_t
lwow_ds18x20_search_alarm(lwow_t* const owobj, lwow_rom_t* const rom_id) {
    lwowr_t res = lwowERR;

    LWOW_ASSERT("owobj != NULL", owobj != NULL);

    lwow_protect(owobj, 1);
    res = lwow_ds18x20_search_alarm_raw(owobj, rom_id);
    lwow_unprotect(owobj, 1);
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
lwow_ds18x20_is_b(lwow_t* const owobj, const lwow_rom_t* const rom_id) {
    LWOW_ASSERT0("owobj != NULL", owobj != NULL);
    LWOW_ASSERT0("rom_id != NULL", rom_id != NULL);

    LWOW_UNUSED(owobj);
    return rom_id->rom[0] == LWOW_DS18B20_FAMILY_CODE;
}

/**
 * \brief           Check if ROM address matches `DS18S20` device
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to test against `DS18S20`
 * \return          `1` on success, `0` otherwise
 * \note            This function is reentrant
 */
uint8_t
lwow_ds18x20_is_s(lwow_t* const owobj, const lwow_rom_t* const rom_id) {
    LWOW_ASSERT0("owobj != NULL", owobj != NULL);
    LWOW_ASSERT0("rom_id != NULL", rom_id != NULL);

    LWOW_UNUSED(owobj);
    return rom_id->rom[0] == LWOW_DS18S20_FAMILY_CODE;
}

/**
 * \brief           Get temperature conversion time in units of milliseconds for a specific resolution
 * 
 * \param           resolution: Resolution in bits
 * \param[in]       is_b: Set to `1` for DS18B20, `0` otherwise
 * \return          uint16_t 
 */
uint16_t
lwow_ds18x20_get_temp_conversion_time(uint8_t resolution, uint8_t is_b) {
    if (is_b) {

        switch (resolution) {
            case 9U: return 94U;
            case 10U: return 188U;
            case 11U: return 375U;
            case 12U: return 750U;
            default: return 0U;
        }
    } else {
        return 750U;
    }
}
