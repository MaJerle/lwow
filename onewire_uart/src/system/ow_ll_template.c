/**
 * \file            ow_ll_template.c
 * \brief           Low-level UART communication
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
#include "system/ow_ll.h"

/**
 * \brief           Initialize low-level communication
 * \param[in]       arg: User argument passed on \ref ow_init function
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ll_init(void* arg) {
    /* Configure and enable UART at 115200 bauds */

    return 1;
}

/**
 * \brief           Deinit low-level
 * \param[in]       arg: User argument passed on \ref ow_init function
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ll_deinit(void* arg) {
    /* Disable UART peripheral */

    return 1;
}

/**
 * \brief           Set baudrate for UART hardware
 * \param[in]       baud: Expected baudrate for UART. Values can be either `9600` or `15200` bauds
 * \param[in]       arg: User argument passed on \ref ow_init function
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ll_set_baudrate(uint32_t baud, void* arg) {
    /* Configure UART to selected baudrate */

    return 1;
}

/**
 * \brief           Transmit-Receive data over UART
 * \param[in]       tx: Array of data to send
 * \param[out]      rx: Array to save receive data
 * \param[in]       len: Number of bytes to send
 * \param[in]       arg: User argument passed on \ref ow_init function
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ll_transmit_receive(const uint8_t* tx, uint8_t* rx, size_t len, void* arg) {
    /* Perform data exchange */

    return 1;
}
