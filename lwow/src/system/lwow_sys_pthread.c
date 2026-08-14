/**
 * \file            lwow_sys_pthread.c
 * \brief           System functions for GCC native pthread (Linux, macOS)
 */

/*
 * Copyright (c) 2024 Tilen MAJERLE
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
 * Version:         v4.0.0
 */
#include "system/lwow_sys.h"
#include <pthread.h>
#include <stdlib.h>

#if LWOW_CFG_OS && !__DOXYGEN__

uint8_t
lwow_sys_mutex_create(LWOW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    pthread_mutexattr_t attr;
    pthread_mutex_t* m;
    LWOW_UNUSED(arg);

    m = malloc(sizeof(*m));
    if (m == NULL) {
        return 0;
    }
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    if (pthread_mutex_init(m, &attr) != 0) {
        pthread_mutexattr_destroy(&attr);
        free(m);
        return 0;
    }
    pthread_mutexattr_destroy(&attr);
    *mutex = m;
    return 1;
}

uint8_t
lwow_sys_mutex_delete(LWOW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    LWOW_UNUSED(arg);
    pthread_mutex_destroy((pthread_mutex_t*)(*mutex));
    free(*mutex);
    *mutex = NULL;
    return 1;
}

uint8_t
lwow_sys_mutex_wait(LWOW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    LWOW_UNUSED(arg);
    return pthread_mutex_lock((pthread_mutex_t*)(*mutex)) == 0;
}

uint8_t
lwow_sys_mutex_release(LWOW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    LWOW_UNUSED(arg);
    return pthread_mutex_unlock((pthread_mutex_t*)(*mutex)) == 0;
}

#endif /* LWOW_CFG_OS && !__DOXYGEN__ */
