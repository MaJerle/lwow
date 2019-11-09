/**
 * \file            ow_ll_stm32f429zi_nucleo.c
 * \brief           UART implementation for STm32F429ZI-Nucleo
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
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         $_version_$
 */

/*
 * Default UART configuration is:
 *
 * UART:                USART1
 * STM32 TX:            GPIOA, GPIO_PIN_9
 * STM32 RX:            GPIOA, GPIO_PIN_10
 */

#if !__DOXYGEN__

#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_pwr.h"

/* USART */
#define ONEWIRE_USART                           USART1
#define ONEWIRE_USART_CLK_EN                    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1)

/* USART TX PIN */
#define ONEWIRE_TX_PORT                         GPIOA
#define ONEWIRE_TX_PORT_CLK_EN                  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)
#define ONEWIRE_TX_PIN                          LL_GPIO_PIN_9
#define ONEWIRE_TX_PIN_AF                       LL_GPIO_AF_7

/* USART RX PIN */
#define ONEWIRE_RX_PORT                         GPIOA
#define ONEWIRE_RX_PORT_CLK_EN                  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)
#define ONEWIRE_RX_PIN                          LL_GPIO_PIN_10
#define ONEWIRE_RX_PIN_AF                       LL_GPIO_AF_7

/* Include generic STM32 driver */
#include "../system/ow_ll_stm32.c"

#endif /* !__DOXYGEN__ */
