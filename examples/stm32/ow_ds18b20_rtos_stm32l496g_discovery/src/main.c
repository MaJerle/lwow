/**
 * \file            main.c
 * \brief           Main file
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
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v3.0.1
 */
#include "main.h"
#include "cmsis_os.h"
#include "lwow/lwow.h"
#include "lwow/devices/lwow_device_ds18x20.h"
#include "scan_devices.h"

static void LL_Init(void);
void SystemClock_Config(void);
static void USART_Printf_Init(void);

static void app_thread(void* arg);

/* Create new 1-Wire instance */
extern const lwow_ll_drv_t lwow_ll_drv_stm32;
lwow_t ow;
lwow_rom_t rom_ids[20];
size_t rom_found;

/**
 * \brief           Program entry point
 */
int
main(void) {
    LL_Init();                                  /* Reset of all peripherals, initializes the Flash interface and the Systick. */
    SystemClock_Config();                       /* Configure the system clock */
    USART_Printf_Init();                        /* Init USART for printf */

    printf("Application running on STM32L496G-Discovery!\r\n");

    osKernelInitialize();
    const osThreadAttr_t app_thread_attr = {
        .priority = osPriorityNormal,
        .stack_size = 512
    };
    osThreadNew(app_thread, NULL, &app_thread_attr);/* Create application thread */
    osKernelStart();                            /* Start kernel */

    while (1) {}
}

/**
 * \brief           Application thread
 * \param[in]       arg: Thread argument
 */
static void
app_thread(void* arg) {
    float avg_temp;
    size_t avg_temp_count;

    /* Initialize 1-Wire library and set user argument to NULL */
    lwow_init(&ow, &lwow_ll_drv_stm32, NULL);

    /* Get onewire devices connected on 1-wire port */
    do {
        if (scan_onewire_devices(&ow, rom_ids, LWOW_ARRAYSIZE(rom_ids), &rom_found) == lwowOK) {
            printf("Devices scanned, found %d devices!\r\n", (int)rom_found);
        } else {
            printf("Device scan error\r\n");
        }
        if (rom_found == 0) {
            osDelay(1000);
        }
    } while (rom_found == 0);

    if (rom_found > 0) {
        /* Infinite loop */
        while (1) {
            printf("Start temperature conversion\r\n");
            lwow_ds18x20_start(&ow, NULL);      /* Start conversion on all devices, use protected API */
            osDelay(1000);                      /* Release thread for 1 second */

            /* Read temperature on all devices */
            avg_temp = 0;
            avg_temp_count = 0;
            for (size_t i = 0; i < rom_found; i++) {
                if (lwow_ds18x20_is_b(&ow, &rom_ids[i])) {
                    float temp;
                    uint8_t resolution = lwow_ds18x20_get_resolution(&ow, &rom_ids[i]);
                    if (lwow_ds18x20_read(&ow, &rom_ids[i], &temp)) {
                        printf("Sensor %02u temperature is %d.%d degrees (%u bits resolution)\r\n",
                            (unsigned)i, (int)temp, (int)((temp * 1000.0f) - (((int)temp) * 1000)), (unsigned)resolution);

                        avg_temp += temp;
                        avg_temp_count++;
                    } else {
                        printf("Could not read temperature on sensor %u\r\n", (unsigned)i);
                    }
                }
            }
            if (avg_temp_count > 0) {
                avg_temp = avg_temp / avg_temp_count;
            }
            printf("Average temperature: %d.%d degrees\r\n", (int)avg_temp, (int)((avg_temp * 100.0f) - ((int)avg_temp) * 100));
        }
    }
    printf("Terminating application thread\r\n");
    osThreadExit();
}

/**
 * \brief           Low-Layer initialization
 */
static void
LL_Init(void) {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    NVIC_SetPriority(MemoryManagement_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(BusFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(UsageFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(SVCall_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(DebugMonitor_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));
}

/**
 * \brief           System clock configuration
 */
void
SystemClock_Config(void) {
    /* Configure flash latency */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
    if (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4) {
        while (1) {}
    }

    /* Set voltage scaling */
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

    /* Enable MSI */
    LL_RCC_MSI_Enable();
    while (LL_RCC_MSI_IsReady() != 1) {}
    LL_RCC_MSI_EnableRangeSelection();
    LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_6);
    LL_RCC_MSI_SetCalibTrimming(0);

    /* Configure PLL */
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);
    LL_RCC_PLL_EnableDomain_SYS();
    LL_RCC_PLL_Enable();
    while (LL_RCC_PLL_IsReady() != 1) {}

    /* Configure system clock to PLL */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {}

    /* Set prescalers */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    /* SysTick_IRQn interrupt configuration */
    LL_Init1msTick(80000000);
    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
    LL_SetSystemCoreClock(80000000);
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));
    LL_SYSTICK_EnableIT();
}

/**
 * \brief           Init USART2 for printf output
 */
static void
USART_Printf_Init(void) {
    LL_USART_InitTypeDef USART_InitStruct;
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

    /*
     * USART2 GPIO Configuration
     *
     * PA2  ------> USART2_TX
     * PD6  ------> USART2_RX
     */
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;

    GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    USART_InitStruct.BaudRate = 921600;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART2, &USART_InitStruct);

    LL_USART_ConfigAsyncMode(USART2);           /* Configure USART in async mode */
    LL_USART_Enable(USART2);                    /* Enable USART */
}

/**
 * \brief           Printf character handler
 * \param[in]       ch: Character to send
 * \param[in]       f: File pointer
 * \return          Written character
 */
#ifdef __GNUC__
int __io_putchar(int ch) {
#else
int fputc(int ch, FILE* fil) {
#endif
    LL_USART_TransmitData8(USART2, (uint8_t)ch);/* Transmit data */
    while (!LL_USART_IsActiveFlag_TXE(USART2)); /* Wait until done */
    return ch;
}
