/**
 * \file            ow_ll_stm32l4xx.c
 * \brief           UART implementation for STM32L4xx MCUs
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
 * This file is part of OneWire library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#include "system/ow_ll.h"

/*
 * STM32L496G-Discovery comes with external STMOD+ board with CN4 ESP-01 connector
 *
 * UART configuration is:
 *
 * UART:                USART1
 * STM32 TX:            GPIOB, GPIO_PIN_6
 * STM32 RX:            GPIOG, GPIO_PIN_10; Note: VDDIO2 must be enabled in PWR register
 */
#include "stm32l4xx_ll_usart.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_gpio.h"
#include "main.h"

#define ONEWIRE_USART                           USART1
#define ONEWIRE_USART_CLK_EN                    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1)
#define ONEWIRE_USART_RCC_CLOCK                 PCLK2_Frequency

#define ONEWIRE_TX_PORT                         GPIOB
#define ONEWIRE_TX_PORT_CLK_EN                  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define ONEWIRE_TX_PIN                          LL_GPIO_PIN_6
#define ONEWIRE_TX_PIN_AF                       LL_GPIO_AF_7

/*
 * Since GPIOG.10 is on VDDIO2, we have to enable VDDIO2 in power management
 */
#define ONEWIRE_RX_PORT                         GPIOG
#define ONEWIRE_RX_PORT_CLK_EN                  do { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOG); LL_PWR_EnableVddIO2(); } while (0)
#define ONEWIRE_RX_PIN                          LL_GPIO_PIN_10
#define ONEWIRE_RX_PIN_AF                       LL_GPIO_AF_7

#define ONEWIRE_USART_TX_DMA                    DMA1
#define ONEWIRE_USART_TX_DMA_CLK_EN             LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)
#define ONEWIRE_USART_TX_DMA_CHANNEL            LL_DMA_CHANNEL_4
#define ONEWIRE_USART_TX_DMA_REQUEST            LL_DMA_REQUEST_2
#define ONEWIRE_USART_TX_DMA_CLEAR_FLAGS        do {    \
    LL_DMA_ClearFlag_TC4(ONEWIRE_USART_TX_DMA);         \
    LL_DMA_ClearFlag_HT4(ONEWIRE_USART_TX_DMA);         \
    LL_DMA_ClearFlag_TE4(ONEWIRE_USART_TX_DMA);         \
} while (0)

#define ONEWIRE_USART_RX_DMA                    DMA1
#define ONEWIRE_USART_RX_DMA_CLK_EN             LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)
#define ONEWIRE_USART_RX_DMA_CHANNEL            LL_DMA_CHANNEL_5
#define ONEWIRE_USART_RX_DMA_REQUEST            LL_DMA_REQUEST_2
#define ONEWIRE_USART_RX_DMA_IRQn               DMA1_Channel5_IRQn
#define ONEWIRE_USART_RX_DMA_IRQ_HANDLER        DMA1_Channel5_IRQHandler
#define ONEWIRE_USART_RX_DMA_CLEAR_FLAGS        do {    \
    LL_DMA_ClearFlag_TC5(ONEWIRE_USART_RX_DMA);         \
    LL_DMA_ClearFlag_HT5(ONEWIRE_USART_RX_DMA);         \
    LL_DMA_ClearFlag_TE5(ONEWIRE_USART_RX_DMA);         \
} while (0)

/**
 * \brief           Initialize low-level communication
 * \param[in]       arg: User argument
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ll_init(void* arg) {
    LL_USART_InitTypeDef USART_InitStruct;
    LL_GPIO_InitTypeDef GPIO_InitStruct;
    
    /* Peripheral clock enable */
    ONEWIRE_TX_PORT_CLK_EN;
    ONEWIRE_RX_PORT_CLK_EN;
    ONEWIRE_USART_TX_DMA_CLK_EN;
    ONEWIRE_USART_RX_DMA_CLK_EN;
    ONEWIRE_USART_CLK_EN;

    /*
     * USART GPIO Configuration
     *
     * USART pins are configured in open-drain mode!
     */
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    
    /* TX pin */
    GPIO_InitStruct.Alternate = ONEWIRE_TX_PIN_AF;
    GPIO_InitStruct.Pin = ONEWIRE_TX_PIN;
    LL_GPIO_Init(ONEWIRE_TX_PORT, &GPIO_InitStruct);
    /* RX pin */
    GPIO_InitStruct.Alternate = ONEWIRE_RX_PIN_AF;
    GPIO_InitStruct.Pin = ONEWIRE_RX_PIN;
    LL_GPIO_Init(ONEWIRE_RX_PORT, &GPIO_InitStruct);

    /* USART RX DMA Init */
    LL_DMA_SetPeriphRequest(ONEWIRE_USART_RX_DMA, ONEWIRE_USART_RX_DMA_CHANNEL, ONEWIRE_USART_RX_DMA_REQUEST);
    LL_DMA_SetDataTransferDirection(ONEWIRE_USART_RX_DMA, ONEWIRE_USART_RX_DMA_CHANNEL, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetMode(ONEWIRE_USART_RX_DMA, ONEWIRE_USART_RX_DMA_CHANNEL, LL_DMA_MODE_NORMAL);
    LL_DMA_SetPeriphIncMode(ONEWIRE_USART_RX_DMA, ONEWIRE_USART_RX_DMA_CHANNEL, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(ONEWIRE_USART_RX_DMA, ONEWIRE_USART_RX_DMA_CHANNEL, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(ONEWIRE_USART_RX_DMA, ONEWIRE_USART_RX_DMA_CHANNEL, LL_DMA_PDATAALIGN_BYTE);
    LL_DMA_SetMemorySize(ONEWIRE_USART_RX_DMA, ONEWIRE_USART_RX_DMA_CHANNEL, LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_SetPeriphAddress(ONEWIRE_USART_RX_DMA, ONEWIRE_USART_RX_DMA_CHANNEL, (uint32_t)&ONEWIRE_USART->RDR);

    /* USART TX DMA Init */
    LL_DMA_SetPeriphRequest(ONEWIRE_USART_TX_DMA, ONEWIRE_USART_TX_DMA_CHANNEL, ONEWIRE_USART_TX_DMA_REQUEST);
    LL_DMA_SetDataTransferDirection(ONEWIRE_USART_TX_DMA, ONEWIRE_USART_TX_DMA_CHANNEL, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetMode(ONEWIRE_USART_TX_DMA, ONEWIRE_USART_TX_DMA_CHANNEL, LL_DMA_MODE_NORMAL);
    LL_DMA_SetPeriphIncMode(ONEWIRE_USART_TX_DMA, ONEWIRE_USART_TX_DMA_CHANNEL, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(ONEWIRE_USART_TX_DMA, ONEWIRE_USART_TX_DMA_CHANNEL, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(ONEWIRE_USART_TX_DMA, ONEWIRE_USART_TX_DMA_CHANNEL, LL_DMA_PDATAALIGN_BYTE);
    LL_DMA_SetMemorySize(ONEWIRE_USART_TX_DMA, ONEWIRE_USART_TX_DMA_CHANNEL, LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_SetPeriphAddress(ONEWIRE_USART_TX_DMA, ONEWIRE_USART_TX_DMA_CHANNEL, (uint32_t)&ONEWIRE_USART->TDR);
    
    /* Enable DMA RX interrupt */
//    NVIC_SetPriority(ONEWIRE_USART_RX_DMA_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));
//    NVIC_EnableIRQ(ONEWIRE_USART_RX_DMA_IRQn);

    /* Configure UART peripherals */
    USART_InitStruct.BaudRate = 9600;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(ONEWIRE_USART, &USART_InitStruct);
    LL_USART_ConfigAsyncMode(ONEWIRE_USART);
    LL_USART_Enable(ONEWIRE_USART);
    
    return 1;
}

/**
 * \brief           Deinit low-level
 * \param[in]       arg: User argument
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ll_deinit(void* arg) {
    return 1;
}

/**
 * \brief           UART set baudrate function
 * \param[in]       arg: User argument
 * \param[in]       baud: Expected baudrate for UART
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ll_set_baudrate(uint32_t baud, void* arg) {
    LL_RCC_ClocksTypeDef rcc_clocks;

    LL_RCC_GetSystemClocksFreq(&rcc_clocks);/* Read system frequencies */
    LL_USART_Disable(ONEWIRE_USART);        /* First disable USART */
    LL_USART_SetBaudRate(ONEWIRE_USART, rcc_clocks.ONEWIRE_USART_RCC_CLOCK, LL_USART_OVERSAMPLING_16, baud);
    LL_USART_Enable(ONEWIRE_USART);         /* Enable USART back */
    
    return 1;
}

/**
 * \brief           Transmit-Receive data over OneWire bus
 * \param[in]       tx: Array of data to send
 * \param[out]      rx: Array to save receive data 
 * \param[in]       len: Number of bytes to send
 * \param[in]       arg: User argument
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ll_transmit_receive(const uint8_t* tx, uint8_t* rx, size_t len, void* arg) {
    const uint8_t* t = tx;
    uint8_t* r = rx;
    
    if (len == 1) {
        LL_USART_TransmitData8(ONEWIRE_USART, *t);
        while (!LL_USART_IsActiveFlag_TXE(ONEWIRE_USART));
        while (!LL_USART_IsActiveFlag_RXNE(ONEWIRE_USART));
        *r = LL_USART_ReceiveData8(ONEWIRE_USART);
        return 1;
    }
    
    /* Clear all DMA flags */
    LL_USART_ClearFlag_TC(ONEWIRE_USART);
    ONEWIRE_USART_RX_DMA_CLEAR_FLAGS;
    ONEWIRE_USART_TX_DMA_CLEAR_FLAGS;
    
    /* Set data length */
    LL_DMA_SetDataLength(ONEWIRE_USART_RX_DMA, ONEWIRE_USART_RX_DMA_CHANNEL, len);
    LL_DMA_SetDataLength(ONEWIRE_USART_TX_DMA, ONEWIRE_USART_TX_DMA_CHANNEL, len);
    
    /* Set memory addresses */
    LL_DMA_SetMemoryAddress(ONEWIRE_USART_RX_DMA, ONEWIRE_USART_RX_DMA_CHANNEL, (uint32_t)r);
    LL_DMA_SetMemoryAddress(ONEWIRE_USART_TX_DMA, ONEWIRE_USART_TX_DMA_CHANNEL, (uint32_t)t);
    
    /* Enable TX and RX requests */
    LL_USART_EnableDMAReq_RX(ONEWIRE_USART);
    LL_USART_EnableDMAReq_TX(ONEWIRE_USART);
    
    /* Enable UART DMA requests and start stream */
    LL_DMA_EnableChannel(ONEWIRE_USART_RX_DMA, ONEWIRE_USART_RX_DMA_CHANNEL);
    LL_DMA_EnableChannel(ONEWIRE_USART_TX_DMA, ONEWIRE_USART_TX_DMA_CHANNEL);
    
    /*
     * Wait to receive all bytes over UART
     *
     * We have to wait for RX DMA to complete and to transfer
     * all bytes to memory before we can finish the transaction
     */
    while (LL_DMA_GetDataLength(ONEWIRE_USART_RX_DMA, ONEWIRE_USART_RX_DMA_CHANNEL));
    
    /* Disable requests */
    LL_DMA_DisableChannel(ONEWIRE_USART_RX_DMA, ONEWIRE_USART_RX_DMA_CHANNEL);
    LL_DMA_DisableChannel(ONEWIRE_USART_TX_DMA, ONEWIRE_USART_TX_DMA_CHANNEL);
    LL_USART_DisableDMAReq_RX(ONEWIRE_USART);
    LL_USART_DisableDMAReq_TX(ONEWIRE_USART);
    
    return 1;
}
