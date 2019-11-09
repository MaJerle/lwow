/**
 * \file            ow_hal_stm32g070RB.c
 * \brief           Low-level UART communication
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
 * Author:          Marcio Figueira <mvfpoa@gmail.com>
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#include "system/ow_ll.h"
#include "stm32g0xx_hal.h"

/**
 * \brief           Initialize low-level communication
 * \param[in]       arg: User argument passed on \ref ow_init function
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ow_ll_init(void* arg) {
	/* Configure and enable UART at 115200 bauds */
	UART_HandleTypeDef* huart = (UART_HandleTypeDef*)arg;

	if(huart->Instance == NULL)
		huart->Instance = USART2;
	/*else
		HAL_UART_DeInit(huart);*/

	huart->Init.BaudRate = 115200;
	huart->Init.WordLength = UART_WORDLENGTH_8B;
	huart->Init.StopBits = UART_STOPBITS_1;
	huart->Init.Parity = UART_PARITY_NONE;
	huart->Init.Mode = UART_MODE_TX_RX;
	huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart->Init.OverSampling = UART_OVERSAMPLING_16;
	huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_ENABLE;
	huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_HalfDuplex_Init(huart) != HAL_OK)
	{
		return 0;
	}
    if (HAL_UARTEx_SetTxFifoThreshold(huart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
		return 0;
    }
    if (HAL_UARTEx_SetRxFifoThreshold(huart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
		return 0;
    }
    if (HAL_UARTEx_EnableFifoMode(huart) != HAL_OK)
    {
		return 0;
    }
	GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
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
	UART_HandleTypeDef* huart = (UART_HandleTypeDef*)arg;
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
	HAL_UART_DeInit(huart);
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
	UART_HandleTypeDef* huart = (UART_HandleTypeDef*)arg;
	huart->Init.BaudRate = baud;
	/*if (HAL_UART_DeInit(huart) != HAL_OK)
	{
		return 0;
	}*/
	if (HAL_HalfDuplex_Init(huart) != HAL_OK)
	{
		return 0;
	}
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
	UART_HandleTypeDef* huart = (UART_HandleTypeDef*)arg;
	HAL_StatusTypeDef stat;
	stat = HAL_UART_Transmit(huart, tx, len, 2);
	if(stat != HAL_OK)
		return 0;
	stat = HAL_UART_Receive(huart, rx, len, 1);
	if(stat != HAL_OK)
		return 0;
    return 1;
}
