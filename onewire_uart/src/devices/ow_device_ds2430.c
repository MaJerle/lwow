/**
 * \file            ow_device_ds2430.h
 * \brief           ds2430 driver implementation
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
 * Author:          Marcio Figueira <mvfpoa@gmail.com>
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#include "ow/ow.h"
#include "ow/devices/ow_device_ds2430.h"


/**
 * \brief           Write the storage scratchpad.
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to read data from, accepts NULL
 * \param[in]      	address: the address to be written (<0x20)
 * \param[in]      	reg: stores the data to be written
 * \param[in]      	length: the length of reg
 * \return          `1` on success, `0` otherwise
 */
owr_t
ow_ds2430_storage_write_scratchpad_raw(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, const uint8_t* reg, uint8_t length)
{
	owr_t ret = owOK;

    OW_ASSERT0("ow != NULL", ow != NULL);
    OW_ASSERT0("address < 0x20", address < 0x07);
    OW_ASSERT0("reg != NULL", reg != NULL);
    OW_ASSERT0("length > 0", length > 0);

    ret = ow_reset_raw(ow);
    if (ret != owOK) {
    	return ret;
    }

	if (rom_id == NULL) {                   /* Check for ROM id */
		if(ow_skip_rom_raw(ow) < 1) {               /* Skip ROM, send to all devices */
			return owERR;
		}
	} else {
		if(ow_match_rom_raw(ow, rom_id) < 1) {      /* Select exact device by ROM address */
			return owERRNODEV;
		}
    }
    uint8_t d = ow_write_byte_raw(ow, OW_DS2430_WR_SCRATCHPAD);  /* Send command to read app register */
    if(d != OW_DS2430_WR_SCRATCHPAD)
    	return owERRPRESENCE;

    d = ow_write_byte_raw(ow, address);  /* Send command to read app register */
    if(d != address)
    	return owERR;

    /* Transfer data from array to scratchpad */
    for (int i=0; length > 0; length--, i++) {
    	d = ow_write_byte_raw(ow, reg[i]);     /* Read byte */
        if(d != reg[i])
        	return owERR;
    }

    ow_reset_raw(ow);

	return ret;
}

/**
 * \copydoc         ow_ds2430_storage_write_scratchpad_raw
 * \note            This function is thread-safe
 */
owr_t
ow_ds2430_storage_write_scratchpad(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, const uint8_t* reg, uint8_t length)
{
    uint8_t res;

    OW_ASSERT0("ow != NULL", ow != NULL);

    ow_protect(ow, 1);
    res = ow_ds2430_storage_write_scratchpad_raw(ow, rom_id, address, reg, length);
    ow_unprotect(ow, 1);
    return res;
}


/**
 * \brief           Read the storage scratchpad.
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to read data from, accepts NULL
 * \param[in]      	address: the address to be read (<0x20)
 * \param[out]     	reg: stores the data to be read
 * \param[in]      	length: the length of reg
 * \return          `1` on success, `0` otherwise
 */
owr_t
ow_ds2430_storage_read_scratchpad_raw(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, uint8_t* reg, uint8_t length)
{
	owr_t ret = owOK;

    OW_ASSERT0("ow != NULL", ow != NULL);
    OW_ASSERT0("address < 0x20", address < 0x07);
    OW_ASSERT0("reg != NULL", reg != NULL);
    OW_ASSERT0("length > 0", length > 0);

    ret = ow_reset_raw(ow);
    if (ret != owOK) {
    	return ret;
    }

	if (rom_id == NULL) {                   /* Check for ROM id */
		if(ow_skip_rom_raw(ow) < 1) {               /* Skip ROM, send to all devices */
			return owERR;
		}
	} else {
		if(ow_match_rom_raw(ow, rom_id) < 1) {      /* Select exact device by ROM address */
			return owERRNODEV;
		}
    }
    uint8_t d = ow_write_byte_raw(ow, OW_DS2430_RD_SCRATCHPAD);  /* Send command to read app register */
    if(d != OW_DS2430_RD_SCRATCHPAD)
    	return owERRPRESENCE;

    d = ow_write_byte_raw(ow, address);  /* Send command to read app register */
    if(d != address)
    	return owERR;

    /* Read plain data from device */
    for (int i=0; length > 0; length--, i++) {
    	reg[i] = ow_read_byte_raw(ow);     /* Read byte */
    }

    ow_reset_raw(ow);

	return ret;
}

/**
 * \copydoc         ow_ds2430_storage_read_scratchpad_raw
 * \note            This function is thread-safe
 */
owr_t
ow_ds2430_storage_read_scratchpad(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, uint8_t* reg, uint8_t length)
{
    uint8_t res;

    OW_ASSERT0("ow != NULL", ow != NULL);

    ow_protect(ow, 1);
    res = ow_ds2430_storage_read_scratchpad_raw(ow, rom_id, address, reg, length);
    ow_unprotect(ow, 1);
    return res;
}


/**
 * \brief           Write the storage memory from scratchpad
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to read data from, accepts NULL
 * \param[in]      	delay10ms: a pointer to a function to call to wait for 10ms
 * \return          `0` on success, `1` otherwise
 */
owr_t
ow_ds2430_storage_copy_from_scratchpad_raw(ow_t* ow, const ow_rom_t* rom_id, void (*delay10ms)()) {
	owr_t ret = owOK;

    OW_ASSERT0("ow != NULL", ow != NULL);

    ret = ow_reset_raw(ow);
    if (ret != owOK) {
    	return ret;
    }

	if (rom_id == NULL) {                   /* Check for ROM id */
		if(ow_skip_rom_raw(ow) < 1) {               /* Skip ROM, send to all devices */
			return owERR;
		}
	} else {
		if(ow_match_rom_raw(ow, rom_id) < 1) {      /* Select exact device by ROM address */
			return owERRNODEV;
		}
    }
    uint8_t d = ow_write_byte_raw(ow, OW_DS2430_CP_SCRATCHPAD);  /* Send command to read app register */
    if(d != OW_DS2430_CP_SCRATCHPAD)
    	return owERRPRESENCE;

    d = ow_write_byte_raw(ow, OW_DS2430_CP_VALIDATION_KEY);  /* Send command to read app register */
    if(d != OW_DS2430_CP_VALIDATION_KEY)
    	return owERR;

    if(delay10ms != NULL) {
    	delay10ms();
    }

	return ret;
}

/**
 * \copydoc         ow_ds2430_storage_read_scratchpad_raw
 * \note            This function is thread-safe
 */
owr_t
ow_ds2430_storage_copy_from_scratchpad(ow_t* ow, const ow_rom_t* rom_id, void (*delay10ms)()) {
    uint8_t res;

    OW_ASSERT0("ow != NULL", ow != NULL);

    ow_protect(ow, 1);
    res = ow_ds2430_storage_copy_from_scratchpad_raw(ow, rom_id, delay10ms);
    ow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Read the application register. If locked, reads from application register, else from scratchpad
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to read data from
 * \param[out]      reg: stores the 8 byte application register
 * \return          `1` on success, `0` otherwise
 */
owr_t
ow_ds2430_storage_read_raw(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, uint8_t* reg, uint8_t length) {
	owr_t ret = owOK;

    OW_ASSERT0("ow != NULL", ow != NULL);

    ret = ow_reset_raw(ow);
    if (ret != owOK) {
    	return ret;
    }

	if (rom_id == NULL) {                   /* Check for ROM id */
		if(ow_skip_rom_raw(ow) < 1) {               /* Skip ROM, send to all devices */
			return owERR;
		}
	} else {
		if(ow_match_rom_raw(ow, rom_id) < 1) {      /* Select exact device by ROM address */
			return owERRNODEV;
		}
    }

    uint8_t d = ow_write_byte_raw(ow, OW_DS2430_RD_MEMORY);  /* Send command to read app register */
    if(d != OW_DS2430_RD_MEMORY)
    	return owERRPRESENCE;

    if(address > 0x1F) {
        ow_reset_raw(ow);
        return owOK;
    }

    OW_ASSERT0("address < 0x07", address < 0x07);
    OW_ASSERT0("reg != NULL", reg != NULL);

    d = ow_write_byte_raw(ow, address);  /* Send command to read app register */
    if(d != address)
    	return owERR;

    /* Read plain data from device */
    for (int i=0; length > 0; length--, i++) {
    	reg[i] = ow_read_byte_raw(ow);     /* Read byte */
    }

    ow_reset_raw(ow);

	return ret;
}

/**
 * \copydoc         ow_ds2430_storage_read_raw
 * \note            This function is thread-safe
 */
owr_t
ow_ds2430_storage_read(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, uint8_t* reg, uint8_t length) {
    uint8_t res;

    OW_ASSERT0("ow != NULL", ow != NULL);

    ow_protect(ow, 1);
    res = ow_ds2430_storage_read_raw(ow, rom_id, address, reg, length);
    ow_unprotect(ow, 1);
    return res;
}


/**
 * \brief           Read the application register. If locked, reads from application register, else from scratchpad
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to read data from
 * \param[out]      reg: stores the 8 byte application register
 * \return          `1` on success, `0` otherwise
 */
owr_t
ow_ds2430_app_register_read_raw(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, uint8_t* reg, uint8_t length) {
    owr_t ret = owOK;

    OW_ASSERT0("ow != NULL", ow != NULL);
    OW_ASSERT0("reg != NULL", reg != NULL);
    OW_ASSERT0("address < 0x07", address < 0x07);

    ret = ow_reset_raw(ow);
    if (ret != owOK) {
    	return ret;
    }

	if (rom_id == NULL) {                   /* Check for ROM id */
		if(ow_skip_rom_raw(ow) < 1)                /* Skip ROM, send to all devices */
			return owERR;
	} else {
		if(ow_match_rom_raw(ow, rom_id) < 1)       /* Select exact device by ROM address */
			return owERRNODEV;
    }
	uint8_t d = ow_write_byte_raw(ow, OW_DS2430_RD_APP_REGISTER);  /* Send command to read app register */
	if(d != OW_DS2430_RD_APP_REGISTER)
		return owERRPRESENCE;

	d = ow_write_byte_raw(ow, address);  /* Send command to read app register */
	if(d != 0)
		return owERRPRESENCE;

	/* Read plain data from device */
	for (int i=0; length > 0; length--,i++) {
		reg[i] = ow_read_byte_raw(ow);     /* Read byte */
	}
	ow_reset_raw(ow);

    return ret;
}

/**
 * \copydoc         ow_ds2430_app_register_read_raw
 * \note            This function is thread-safe
 */
owr_t
ow_ds2430_app_register_read(ow_t* ow, const ow_rom_t* rom_id, const uint8_t address, uint8_t* reg, uint8_t length) {
    uint8_t res;

    OW_ASSERT0("ow != NULL", ow != NULL);

    ow_protect(ow, 1);
    res = ow_ds2430_app_register_read_raw(ow, rom_id, address, reg, length);
    ow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Read status register
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to read data from
 * \return          `1` on success, `0` otherwise
 */
owr_t
ow_ds2430_status_read_raw(ow_t* ow, const ow_rom_t* rom_id, uint8_t* status) {
	owr_t ret = owOK;

    OW_ASSERT0("ow != NULL", ow != NULL);

    ret = ow_reset_raw(ow);
    if (ret != owOK) {
    	return ret;
    }

	if (rom_id == NULL) {                   /* Check for ROM id */
		if(ow_skip_rom_raw(ow) < 1)                /* Skip ROM, send to all devices */
			return owERR;
	} else {
		if(ow_match_rom_raw(ow, rom_id) < 1)       /* Select exact device by ROM address */
			return owERRNODEV;
    }
	uint8_t d = ow_write_byte_raw(ow, OW_DS2430_RD_STATUS);  /* Send command to read app register */
	if(d != OW_DS2430_RD_STATUS)
		return owERR;

	d = ow_write_byte_raw(ow, OW_DS2430_RD_STS_VALIDATION_KEY);  /* Send command to read app register */
	if(d != OW_DS2430_RD_STS_VALIDATION_KEY)
		return owERR;

	/* Read plain data from device */
	*status = ow_read_byte_raw(ow);     /* Read byte */
	// 0xFC indicates it is locked
	ow_reset_raw(ow);

    return ret;
}

/**
 * \copydoc         ow_ds2430_status_read
 * \note            This function is thread-safe
 */
owr_t
ow_ds2430_status_read(ow_t* ow, const ow_rom_t* rom_id, uint8_t* status) {
	owr_t res;

    OW_ASSERT0("ow != NULL", ow != NULL);

    ow_protect(ow, 1);
    res = ow_ds2430_status_read_raw(ow, rom_id, status);
    ow_unprotect(ow, 1);
    return res;
}

/**
 * \brief           Read status register
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_id: 1-Wire device address to read data from
 * \return          `1` on success, `0` otherwise
 */
owr_t
ow_ds2430_app_register_is_locked_raw(ow_t* ow, const ow_rom_t* rom_id, uint8_t* locked) {
	owr_t ret;

	uint8_t status;
    ret = ow_ds2430_status_read_raw(ow, rom_id, &status);
    if(ret != owOK)
    	return ret;
    if(status == 0xFC)
    	*locked = 1;
    else if(status == 0xFF)
    	*locked = 0;
    else return owERR;
    return owOK;
}

/**
 * \copydoc         ow_ds2430_app_register_is_locked_raw
 * \note            This function is thread-safe
 */
owr_t
ow_ds2430_app_register_is_locked(ow_t* ow, const ow_rom_t* rom_id, uint8_t* locked) {
    uint8_t res;

    OW_ASSERT0("ow != NULL", ow != NULL);

    ow_protect(ow, 1);
    res = ow_ds2430_app_register_is_locked_raw(ow, rom_id, locked);
    ow_unprotect(ow, 1);
    return res;
}


