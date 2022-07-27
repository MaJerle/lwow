/**
 * \file            lwow_ll_win32.c
 * \brief           UART implementation for WIN32
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
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
#include <stdio.h>
#include "lwow/lwow.h"
#include "windows.h"

#if !__DOXYGEN__

/* Function prototypes */
static uint8_t init(void* arg);
static uint8_t deinit(void* arg);
static uint8_t set_baudrate(uint32_t baud, void* arg);
static uint8_t transmit_receive(const uint8_t* tx, uint8_t* rx, size_t len, void* arg);

/* Win 32 LL driver for OW */
const lwow_ll_drv_t
lwow_ll_drv_win32 = {
    .init = init,
    .deinit = deinit,
    .set_baudrate = set_baudrate,
    .tx_rx = transmit_receive,
};

static HANDLE com_port;
static DCB dcb = { 0 };

static uint8_t
init(void* arg) {
    dcb.DCBlength = sizeof(dcb);
    LWOW_UNUSED(arg);

    /* Open virtual file as read/write */
    com_port = CreateFileA("\\\\.\\COM4",
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          0,
                          OPEN_EXISTING,
                          0,
                          NULL
                         );

    /* First read current values */
    if (GetCommState(com_port, &dcb)) {
        COMMTIMEOUTS timeouts;

        dcb.BaudRate = 115200;
        dcb.ByteSize = 8;
        dcb.Parity = NOPARITY;
        dcb.StopBits = ONESTOPBIT;

        /* Try to set com port data */
        if (!SetCommState(com_port, &dcb)) {
            printf("Cannot get COM port..\r\n");
            return 0;
        }

        if (GetCommTimeouts(com_port, &timeouts)) {
            /* Set timeout to return immediatelly from ReadFile function */
            timeouts.ReadIntervalTimeout = MAXDWORD;
            timeouts.ReadTotalTimeoutConstant = 0;
            timeouts.ReadTotalTimeoutMultiplier = 0;
            if (!SetCommTimeouts(com_port, &timeouts)) {
                printf("Cannot set COM PORT timeouts..\r\n");
            }
            GetCommTimeouts(com_port, &timeouts);
        } else {
            printf("Cannot get communication timeouts..\r\n");
            return 0;
        }
    } else {
        printf("Cannot get COM port info..\r\n");
        return 0;
    }

    return 1;
}

uint8_t
deinit(void* arg) {
    /* Disable UART peripheral */
    LWOW_UNUSED(arg);

    return 1;
}

uint8_t
set_baudrate(uint32_t baud, void* arg) {
    LWOW_UNUSED(arg);
    /* Configure UART to selected baudrate */
    dcb.BaudRate = baud;

    /* Try to set com port data */
    if (!SetCommState(com_port, &dcb)) {
        printf("Cannot set COM port baudrate to %u bauds\r\n", (unsigned)baud);
        return 0;
    }

    return 1;
}

uint8_t
transmit_receive(const uint8_t* tx, uint8_t* rx, size_t len, void* arg) {
    /* Perform data exchange */
    size_t read = 0;
    DWORD br;
    LWOW_UNUSED(arg);

    if (com_port != NULL) {
        /*
         * Flush any data in RX buffer.
         * This helps to reset communication in case of on-the-fly device management
         * if one-or-more device(s) are added or removed.
         *
         * Any noise on UART level could start byte and put it to Win buffer,
         * preventing to read aligned data from it
         */
        PurgeComm(com_port, PURGE_RXCLEAR | PURGE_RXABORT);

        /* Write file and send data */
        WriteFile(com_port, tx, len, &br, NULL);
        FlushFileBuffers(com_port);

        /* Read same amount of data as sent previously (loopback) */
        do {
            if (ReadFile(com_port, rx, (DWORD)(len - read), &br, NULL)) {
                read += (size_t)br;
                rx += (size_t)br;
            }
        } while (read < len);
    }

    return 1;
}

#endif /* !__DOXYGEN__ */
