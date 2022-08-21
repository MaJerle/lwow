/**
 * \file            lwow_ll_stm32_single_gpio_driver.c
 * \brief           Driver for non-UART use, with single GPIO
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
#include "stm32l4xx_hal.h"
/* And all other includes */

#if !__DOXYGEN__

/* Pin setting */
#define GPIO_CLK_EN                     LL_AHB2_GRP1_PERIPH_GPIOB
#define GPIO_PORT                       GPIOB
#define GPIO_PIN                        LL_GPIO_PIN_13
#define OW_PIN_LOW                      LL_GPIO_ResetOutputPin(GPIO_PORT, GPIO_PIN)
#define OW_PIN_HIGH                     LL_GPIO_SetOutputPin(GPIO_PORT, GPIO_PIN)
#define OW_PIN_INPUT                    LL_GPIO_SetPinMode(GPIO_PORT, GPIO_PIN, LL_GPIO_MODE_INPUT)
#define OW_PIN_OUTPUT                   LL_GPIO_SetPinMode(GPIO_PORT, GPIO_PIN, LL_GPIO_MODE_OUTPUT)

/* Macros for irq lock */
#define IRQ_LOCK_DEFINE                 uint32_t primask = __get_PRIMASK()
#define IRQ_LOCK                        __disable_irq()
#define IRQ_UNLOCK                      __set_PRIMASK(primask)

/* Function prototypes for driver */
static uint8_t prv_init(void* arg);
static uint8_t prv_deinit(void* arg);
static uint8_t prv_set_baudrate(uint32_t baud, void* arg);
static uint8_t prv_transmit_receive(const uint8_t* tx, uint8_t* rx, size_t len, void* arg);

/* Global driver structure for application use */
const
lwow_ll_drv_t ow_driver_gpio = {
    .init = prv_init,
    .deinit = prv_deinit,
    .set_baudrate = prv_set_baudrate,
    .tx_rx = prv_transmit_receive
};
static uint32_t baudrate;               /* Expected baudrate set by the application */

/**
 * \brief           Actual data exchange function
 * 
 * This is the demo for STM32L4xx; with slight modifications it will work on any other architecture.
 * 
 * Requirements to be provided for application
 * - microseconds timing function (example below uses 16-bit timer as source)
 * - interrupt locking mechanism
 * - GPIO manupulation features (open-drain mode, pull, etc)
 * 
 * \param           low_init_pulse_time: Time in microseconds for initial low pulse width
 * \param           pre_sample_time: Time in us to wait after pin release.
 *                      Bus is samples after time expiration
 * \param           post_sample_time: Time in us to wait after sample has been completed
 * \return          Bus value, `1 == high`, `0 == low`
 */
static uint8_t
prv_exch(uint16_t low_init_pulse_time, uint16_t pre_sample_time, uint16_t post_sample_time) {
    uint8_t b = 0;
    uint16_t time, start_time;
    IRQ_LOCK_DEFINE;

    /* Lock interrupts and start execution */
    IRQ_LOCK;
    time = timebase_get_us_tick();

    /* Initiate start low pulse */
    start_time = time;
    OW_PIN_LOW;
    OW_PIN_OUTPUT;
    while ((uint16_t)((time = timebase_get_us_tick()) - start_time) < low_init_pulse_time) {}

    /* Release line and wait for mid pulse */
    start_time = time;
    OW_PIN_INPUT;
    while ((uint16_t)((time = timebase_get_us_tick()) - start_time) < pre_sample_time) {}

    /* Read pin state */
    b = LL_GPIO_IsInputPinSet(GPIO_PORT, GPIO_PIN);

    /* Wait remaining time */
    start_time = time;
    while ((uint16_t)((time = timebase_get_us_tick()) - start_time) < post_sample_time) {}
    
    IRQ_UNLOCK;

    return b;
}

/*******************************************/
/* LwOW driver interface functions         */
/*******************************************/
static uint8_t
prv_init(void* arg) {
    LL_GPIO_InitTypeDef gpio_init;

    /* Peripheral clock enable */
    LL_AHB2_GRP1_EnableClock(GPIO_CLK_EN);

    /* Configure GPIO pin with open-drain mode */
    LL_GPIO_StructInit(&gpio_init);
    gpio_init.Mode = LL_GPIO_MODE_INPUT;
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_init.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    gpio_init.Pull = LL_GPIO_PULL_UP;
    gpio_init.Pin = GPIO_PIN;
    LL_GPIO_Init(GPIO_PORT, &gpio_init);

    LWOW_UNUSED(arg);
    return 1;
}

static uint8_t
prv_deinit(void* arg) {
    LWOW_UNUSED(arg);
    return 1;
}

static uint8_t
prv_set_baudrate(uint32_t baud, void* arg) {
    LWOW_UNUSED(arg);
    baudrate = baud;
    return 1;
}

static uint8_t
prv_transmit_receive(const uint8_t* tx, uint8_t* rx, size_t len, void* arg) {
    const uint8_t* t = tx;
    uint8_t* r = rx;

    /* 
     * For baudrate set at 9600 - by UART definition
     * this fits timing only for reset sequence at onewire level
     * 
     * Length must always be zero, or error is returned
     */
    if (baudrate == 9600) {
        if (len == 1) {
            uint8_t v = prv_exch(480, 70, 410);
            *r = v ? *t : 0x01;
        } else {
            return 0;
        }
    } else if (baudrate == 115200) {
        /*
         * Regular transmission process
         *
         * Exchange values and set timings for different events,
         * according to the byte value to be transmitted
         */
        for (size_t i = 0; i < len; ++i, ++r, ++t) {
            uint8_t v = prv_exch(
                *t ? 6 : 60,
                *t ? 9 : 0,
                *t ? 55 : 10
            );

            /*
             * Set value as 0xFF in case of positive reading, 0x00 otherwise.
             * This is to be compliant with LwOW UART expectations 
             */
            *r = v ? 0xFF : 0x00;
        }
    } else {
        return 0;
    }
    LWOW_UNUSED(arg);
    return 1;
}

#endif /* !__DOXYGEN__ */
