/**
 * \file            ow_ll_stm32l496g_discovery.c
 * \brief           UART implementation for STM32L496G-Discovery
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

/*
 * STM32L496G-Discovery comes with external STMOD+ board with CN4 ESP-01 connector
 *
 * UART configuration is:
 *
 * UART:                USART1
 * STM32 TX:            GPIOB, GPIO_PIN_6
 * STM32 RX:            GPIOG, GPIO_PIN_10; Note: VDDIO2 must be enabled in PWR register
 */

#if !__DOXYGEN__

#include "stm32l4xx_ll_usart.h"
#include "stm32l4xx_ll_lpuart.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_pwr.h"

/* USART */
#define ONEWIRE_USART                           USART1
#define ONEWIRE_USART_CLK_EN                    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1)

/* USART TX PIN */
#define ONEWIRE_TX_PORT                         GPIOB
#define ONEWIRE_TX_PORT_CLK_EN                  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define ONEWIRE_TX_PIN                          LL_GPIO_PIN_6
#define ONEWIRE_TX_PIN_AF                       LL_GPIO_AF_7

/* USART RX PIN */
/* Since GPIOG.10 is on VDDIO2, VDDIO2 domain must be enabled aswell */
#define ONEWIRE_RX_PORT                         GPIOG
#define ONEWIRE_RX_PORT_CLK_EN                  do { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOG); LL_PWR_EnableVddIO2(); } while (0)
#define ONEWIRE_RX_PIN                          LL_GPIO_PIN_10
#define ONEWIRE_RX_PIN_AF                       LL_GPIO_AF_7

/* Include generic STM32 driver */
#include "../system/ow_ll_stm32.c"

#endif /* !__DOXYGEN__ */
