/**
 * \file            lwow_opt.h
 * \brief           OneWire options
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
 * Version:         v3.0.1
 */
#ifndef LWOW_HDR_OPT_H
#define LWOW_HDR_OPT_H

/* Uncomment to ignore user options (or set macro in compiler flags) */
/* #define LWOW_IGNORE_USER_OPTS */

/* Include application options */
#ifndef LWOW_IGNORE_USER_OPTS
#include "lwow_opts.h"
#endif /* LWOW_IGNORE_USER_OPTS */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup        LWOW_OPT Configuration
 * \brief           OW options
 * \{
 */

/**
 * \brief           Enables `1` or disables `0` operating system support in the library
 *
 * \note            When `LWOW_CFG_OS` is enabled, user must implement functions in \ref LWOW_SYS group.
 */
#ifndef LWOW_CFG_OS
#define LWOW_CFG_OS                               0
#endif

/**
 * \brief           Mutex handle type
 *
 * \note            This value must be set in case \ref LWOW_CFG_OS is set to `1`.
 *                  If data type is not known to compiler, include header file with
 *                  definition before you define handle type
 */
#ifndef LWOW_CFG_OS_MUTEX_HANDLE
#define LWOW_CFG_OS_MUTEX_HANDLE                  void *
#endif

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWOW_HDR_OPT_H */
