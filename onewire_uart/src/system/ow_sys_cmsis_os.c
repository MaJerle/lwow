/**
 * \file            ow_sys_cmsis_os.c
 * \brief           System functions for CMSIS-OS based operating system
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

#if OW_CFG_OS && !__DOXYGEN__

#include "cmsis_os.h"

uint8_t
ow_sys_mutex_create(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    const osMutexAttr_t attr = {
        .attr_bits = osMutexRecursive
    };

    *mutex = osMutexNew(&attr);                 /* Create new mutex */
    OW_UNUSED(arg);
    return 1;
}

uint8_t
ow_sys_mutex_delete(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    OW_UNUSED(arg);
    osMutexDelete(*mutex);                      /* Delete mutex */
    return 1;
}

uint8_t
ow_sys_mutex_wait(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    if (osMutexAcquire(*mutex, osWaitForever) != osOK) {
        return 0;
    }
    OW_UNUSED(arg);
    return 1;
}

uint8_t
ow_sys_mutex_release(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    if (osMutexRelease(*mutex) != osOK) {
        return 0;
    }
    OW_UNUSED(arg);
    return 1;
}

#endif /* OW_CFG_OS && !__DOXYGEN__ */
