/**
 * \file            ow_sys_template.c
 * \brief           System functions template file
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
#include "system/ow_sys.h"

#if OW_CFG_OS || __DOXYGEN__

#include "cmsis_os.h"

/**
 * \brief           Create a new mutex and assign value to handle
 * \param[out]      mutex: Output variable to save mutex handle
 * \param[in]       arg: User argument passed on \ref ow_init function
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_sys_mutex_create(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    return 1;
}

/**
 * \brief           Delete existing mutex and invalidate mutex variable
 * \param[in]       mutex: Mutex handle to remove and invalidate
 * \param[in]       arg: User argument passed on \ref ow_init function
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_sys_mutex_delete(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    return 1;
}

/**
 * \brief           Wait for a mutex until ready (unlimited time)
 * \param[in]       mutex: Mutex handle to wait for
 * \param[in]       arg: User argument passed on \ref ow_init function
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_sys_mutex_wait(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    return 1;
}

/**
 * \brief           Release already locked mutex
 * \param[in]       mutex: Mutex handle to release
 * \param[in]       arg: User argument passed on \ref ow_init function
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_sys_mutex_release(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    return 1;
}

#endif /* OW_CFG_OS || __DOXYGEN__ */
