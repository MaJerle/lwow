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
 * This file is part of OneWire-UART.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#include "ow/ow.h"
#include "system/ow_ll.h"
#include "string.h"
#if OW_CFG_OS
#include "system/ow_sys.h"
#endif /* OW_CFG_OS */

#if !__DOXYGEN__

/* Internal macros */
#define OW_FIRST_DEV                    0xFF
#define OW_LAST_DEV                     0x00

/* List of commands */
#define ONEWIRE_CMD_MATCH_ROM           0x55
#define ONEWIRE_CMD_SKIP_ROM            0xCC
#define ONEWIRE_CMD_SEARCH_ROM          0xF0

#define ONEWIRE_BYTE_RESET              0xF0

#endif /* !__DOXYGEN__ */

/**
 * \brief           Send single bit to OneWire port
 * \param[in]       v: Value to send, either `1` or `0`
 * \return          Read byte on 1-wire port, either `1` or `0`
 */
static uint8_t
send_bit(ow_t* ow, uint8_t v) {
    uint8_t b;
	
    /*
     * Sending logical 1 over 1-wire, send 0xFF over UART
     * Sending logical 0 over 1-wire, send 0x00 over UART
     */
    v = v ? 0xFF : 0x00;                        /* Convert to 0 or 1 */
    ow_ll_transmit_receive(&v, &b, 1, ow->arg); /* Exchange data over USART */
    if (b == 0xFF) {                            /* To read bit 1, check of 0xFF sequence */
        return 1;
    }
    return 0;
}

/**
 * \brief           Initialize OneWire library
 * \return          `1` on success, `0` otherwise
 */
owr_t
ow_init(ow_t* ow, void* arg) {
    if (!ow_ll_init(arg)) {                     /* Init low-level directly */
        return owERR;
    }
#if OW_CFG_OS
    if (!ow_sys_mutex_create(&ow->mutex, arg)) {
        ow_ll_deinit(arg);                      /* Deinit low-level */
        return owERR;
    }
#endif /* OW_CFG_OS */
    ow->arg = arg;
    return owOK;
}

/**
 * \brief           Protect 1-wire from concurrent access
 * \note            Used only for OS systems
 * \param[in,out]   ow: 1-Wire handle
 * \return          \ref owOK on success, member of \ref owr_t otherwise
 */
owr_t
ow_protect(ow_t* ow) {
#if OW_USE_RTOS
    if (!ow_sys_mutex_wait(&ow->mutex, arg)) {
        return owERR;
    }
#endif /* OW_USE_RTOS */
    return owOK;
}

/**
 * \brief           Unprotect 1-wire from concurrent access
 * \note            Used only for OS systems
 * \param[in,out]   ow: 1-Wire handle
 * \return          \ref owOK on success, member of \ref owr_t otherwise
 */
owr_t
ow_unprotect(ow_t* ow) {
#if OW_USE_RTOS
    if (!ow_sys_mutex_release(&ow->mutex, arg)) {
        return owERR;
    }
#endif /* OW_USE_RTOS */
    return owOK;
}

/**
 * \brief           Send reset pulse
 * \param[in,out]   ow: 1-Wire handle
 * \return          \ref owOK on success, member of \ref owr_t otherwise
 */
owr_t
ow_reset(ow_t* ow) {
    uint8_t b;
    
    /*
     * To send reset pulse, we have to set UART baudrate to "slow" mode, 9600 bauds.
     * At 9600 bauds, every byte takes 10x bit time (start + 8-bits + stop).
     *
     * Bit time = 1/9600 = 0.000104s = 104 us
     *
     * Reset pulse must be low for at least 480us. If we take 5-bits, this is 520us.
     * Since UART starts with start bits (high-to-low transition), our first bit can be used from START seuquence,
     * which mean we need to send additional 4-bits as 0.
     *
     * UART is usually LSB first oriented which means that sending 0xF0 constant will give us expected result:
     *
     * IDLE  S 0 0 0 0 1 1 1 1 S IDLE
     * ----- _ _ _ _ _ - - - - - -----
     *
     *      |         | <- Timing between matches 5 bits = 520us
     *
     * After the 5-bits low-period, master must deselect line and make it available for slave to pull it low as acknowledgment.
     * Timing from master release to expected ack from slave we have to wait at least 70us which takes another bit (6-th bit) in the sequence
     *
     * IDLE  S 0 0 0 0 I 1 1 1 S IDLE
     * ----- _ _ _ _ _ - - - - - -----
     *
     *                | | <- Idle bit
     *
     * For the last 3-data + stop bits we have just enough time to check for ack line low.
     * By 1-wire specs, we need to give 410us to slave to reply with low state.
     *
     * Since we have remaining 3-data bits + stop bit available, we can use them to see the reply from slave.
     *
     * -----------------
     *
     * At the end, if we send 0xF0 over UART on TX line and if we receive 0xF0 back, we do not have any device connected as there was no acknowledgement.
     * 
     */
    
    b = ONEWIRE_BYTE_RESET;                     /* Set reset sequence byte = 0xF0 */
    ow_ll_set_baudrate(9600, ow->arg);          /* Set low baudrate */
    ow_ll_transmit_receive(&b, &b, 1, ow->arg); /* Exchange data over onewire */
    ow_ll_set_baudrate(115200, ow->arg);        /* Go back to high baudrate */
    
    /*
     * Check if any device acknowledged our pulse
     */
    if (b == 0x00 || b == ONEWIRE_BYTE_RESET) {
        return owERRPRESENCE;
    }
    return owOK;
}

/**
 * \brief           Write byte over 1-wire protocol
 * \param[in,out]   ow: 1-Wire handle
 * \param[in]       b: Byte to write
 * \return          Received byte over 1-wire protocol
 */
uint8_t
ow_write_byte(ow_t* ow, uint8_t b) {
    uint8_t i, r = 0, tr[8];
    
    /*
     * Each BIT on 1-wire level represents 1-byte on UART level at 115200 bauds.
     *
     * To transmit entire byte over 1-wire protocol, we have to send 8-bytes over UART
     *
     * Writing logical bit 1 over 1-Wire protocol starts by pulling line low for about 6us.
     * After that, we should release line for at least 64us. Entire sequence takes about 70us in total.
     *
     * From UART point of view, we have to send 0xFF on TX line. Start bit will take care of initial 6us low pulse and 0xFF will take care of high pulse.
     *
     * Writing logical bit 0 over 1-Wire protocol is similar to bit 1, but here we only pull line low for 60us and then release it with STOP bit.
     * To write logical bit 0, we have to send constant 0x00 over UART.
     */
    
    /* Create output data */
    for (i = 0; i < 8; i++) {
        /*
         * If we have to send high bit, set byte as 0xFF,
         * otherwise set it as low bit, 0x00
         */
        tr[i] = (b & (1 << i)) ? 0xFF : 0x00;
    }
    /*
     * Exchange data on UART level,
     * send single byte for each bit = 8 bytes
     */
    ow_ll_transmit_receive(tr, tr, 8, ow->arg); /* Exchange data over UART */
    
    /*
     * Check received data. If we read 0xFF,
     * our logical write 1 was successful, otherwise it was 0.
     */
    for (i = 0; i < 8; i++) {
        if (tr[i] == 0xFF) {
            r |= 0x01 << i;
        }
    }
    return r;
}

/**
 * \brief           Read next byte on 1-Wire
 * \param[in,out]   ow: 1-Wire handle
 * \return          Byte read over 1-Wire
 */
uint8_t
ow_read_byte(ow_t* ow) {
    /*
     * When we want to read byte over 1-Wire,
     * we have to send all bits as 1 and check if slave pulls line down.
     *
     * According to slave reactions, we can later construct received bytes
     */
    return ow_write_byte(ow, 0xFF);
}

/**
 * \brief           Reset search
 * \param[in,out]   ow: 1-Wire handle
 * \return          \ref owOK on success, member of \ref owr_t otherwise
 */
owr_t
ow_search_reset(ow_t* ow) {
    ow->disrepancy = OW_FIRST_DEV;              /* Reset disrepancy to default value */
    return owOK;
}

/**
 * \brief           Search for devices on 1-wire protocol
 * \note            To reset search and to start over, use \ref ow_search_reset function
 * \param[in,out]   ow: 1-Wire handle
 * \param[out]      rom_id: Pointer to 8-byte long variable to save ROM
 * \return          \ref owOK on success, member of \ref owr_t otherwise
 */
owr_t
ow_search(ow_t* ow, uint8_t *rom_id) {
    uint8_t id_bit_number, j, next_disrepancy;
    uint8_t b, b_cpl;
    owr_t res;
    uint8_t* id = ow->rom;
    
    /*
     * Check for last device
     */
    if (ow->disrepancy == 0) {
        ow_search_reset(ow);
        return owERRNODEV;                      /* No devices anymore */
    }

    /*
     * Step 1: Reset all devices on 1-Wire line to be able to listen for new command
     */
    if ((res = ow_reset(ow)) != owOK) {
        return res;
    }
    
    /*
     * Step 2: Send search rom command for all devices on 1-Wire
     */
    ow_write_byte(ow, ONEWIRE_CMD_SEARCH_ROM);  /* Start with search ROM command */
    next_disrepancy = OW_LAST_DEV;              /* This is currently last device */

    id_bit_number = 64;                         /* We have to read 8 bytes, each 8 bits */
    while (id_bit_number) {
        j = 8;
        while (j--) {
            b       = send_bit(ow, 1);          /* Read first bit = next address bit */
            b_cpl   = send_bit(ow, 1);          /* Read second bit = complementary bit of next address bit */
            
            /*
             * If we have connected many devices on 1-Wire port b and b_cpl are ANDed between all devices.
             *
             * We have to react if b and b_cpl are the same:
             *
             * - Both 1: No devices on 1-Wire line responded
             *      - No device connected at all
             *      - All devices were put to block state due to search
             * - Both 0: We have "collision" as device with bit 0 and bit 1 are connected
             *
             * If b and b_cpl are different, it means we have:
             *  - Single device connected on 1-Wire or
             *  - All devices on 1-Wire have the same bit value at current position
             *  - In this case, we move to direction of b value
             */
            if (b && b_cpl) {
                goto out;                       /* We do not have device connected */
            } else if (!b && !b_cpl) {
                /*
                 * Decide which way to go for next scan
                 *
                 * Force move to "1" in case of:
                 *
                 * - known diff position is larger than current bit reading
                 * - Previous ROM address bit 0 was 1 and known diff is different than reading
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
             * will go on blocked state and will wait for next reset sequence
             *
             * In case of "collision", we can decide here which devices we will 
             * continue to scan (binary tree)
             */
            send_bit(ow, b);                    /* Send bit you want to continue with */
            
            /*
             * Because we shift down *id each iteration, we have to position bit value to the MSB position
             * and it will be automatically positioned correct way.
             */
            *id = (*id >> 0x01) | (b << 0x07);  /* Shift ROM byte down and add next, protocol is LSB first */
            id_bit_number--;
        }
        id++;                                   /* Go to next byte */
    }
out:
    ow->disrepancy = next_disrepancy;           /* Save disrepancy value */
    memcpy(rom_id, ow->rom, sizeof(ow->rom));   /* Copy ROM to user memory */
    return id_bit_number == 0 ? owOK : owERRNODEV;  /* Return search result status */
}

/**
 * \brief           Select device on 1-wire network with exact ROM number
 * \param[in]       ow: 1-Wire handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_match_rom(ow_t* ow, uint8_t* rom_id) {
    uint8_t i;

    ow_write_byte(ow, ONEWIRE_CMD_MATCH_ROM);   /* Write byte to match rom exactly */

    /*
     * Send 8 bytes representing ROM address
     */
    for (i = 0; i < 8; i++) {
        ow_write_byte(ow, rom_id[i]);           /* Send ROM bytes */
    }
    return 1;
}

/**
 * \brief           Skip ROM address and select all devices on the network
 * \param[in]       ow: 1-Wire handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_skip_rom(ow_t* ow) {
    ow_write_byte(ow, ONEWIRE_CMD_SKIP_ROM);    /* Write byte to match rom exactly */
    return 1;
}

/**
 * \brief           Calculate CRC-8 of input data
 * \param[in]       in: Input data
 * \param[in]       len: Number of bytes
 * \return          Calculated CRC
 */
uint8_t
ow_crc(const void *in, size_t len) {
    uint8_t crc = 0, inbyte, i, mix;
    const uint8_t* d = in;

    while (len--) {
        inbyte = *d++;
        for (i = 8; i; i--) {
            mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) {
                crc ^= 0x8C;
            }
            inbyte >>= 1;
        }
    }
    return crc;
}
