/**
 * \file            ow_config_default.h
 * \brief           OneWire default config
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
#ifndef __ONEWIRE_CONFIG_DEFAULT_H
#define __ONEWIRE_CONFIG_DEFAULT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup         OW
 * \defgroup        OW_CONFIG OneWire config
 * \brief           Configuration for OneWire library
 * \{
 */
 
/**
 * \brief           OS configuration
 */
#ifndef OW_CFG_OS
#define OW_CFG_OS                               1
#endif

/**
 * \brief           Mutex handle type
 * \note            This value must be set in case \ref OW_CFG_OS is set to 1
 */
#ifndef OW_CFG_OS_MUTEX_HANDLE
#define OW_CFG_OS_MUTEX_HANDLE                  void *
#endif
 
/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ONEWIRE_CONFIG_DEFAULT_H */
