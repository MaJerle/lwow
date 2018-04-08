/**
 * \file            main.c
 * \brief           Application
 */
#include "main.h"
#include "onewire.h"

/* Function declarations */
static void LL_Init(void);
static void SystemClock_Config(void);
static void debug_init(void);

/* Printf for bits */
#define B2B_PATT "%c%c%c%c %c%c%c%c"
#define B2B(byte)    \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0') 

/**
 * \brief           Application entry point
 */
int
main(void) {
    uint8_t c;
    uint8_t id[8][8];
    ow_res_t res;
    ow_t ow;
    
    LL_Init();                                  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    SystemClock_Config();                       /* Configure the system clock */

    debug_init();                               /* Init debug UART */
    printf("OneWire over UART example!\r\n");
    
    ow_init(&ow);                               /* Init OneWire */
    
    ow_lock(&ow);                               /* Lock 1-Wire from concurrent access, in case of RTOS usage */
    ow_search_reset(&ow);                       /* Reset search */
    c = 0;
    while ((res = ow_search(&ow, id[c])) == owOK) {
        /* Print device */
        printf("ID: " B2B_PATT " (%02X) | " B2B_PATT " (%02X) | " B2B_PATT " (%02X) | " B2B_PATT " (%02X) | " B2B_PATT
                " (%02X) | " B2B_PATT " (%02X) | " B2B_PATT " (%02X) | " B2B_PATT " (%02X)\r\n",
            B2B(id[c][0]), id[c][0],
            B2B(id[c][1]), id[c][1],
            B2B(id[c][2]), id[c][2],
            B2B(id[c][3]), id[c][3],
            B2B(id[c][4]), id[c][4],
            B2B(id[c][5]), id[c][5],
            B2B(id[c][6]), id[c][6],
            B2B(id[c][7]), id[c][7]
        );
        c++;
    }
    ow_unlock(&ow);                             /* Unlock 1-Wire to allow concurrent access */
    printf("Search done!\r\n");
    (void)res;

    /* Infinite loop */
    while (1) {
        
    }
}

/**
 * \brief           printf handle function
 */
int
fputc(int ch, FILE* f) {
    LL_USART_TransmitData8(USART2, (uint8_t)ch);
    while (!LL_USART_IsActiveFlag_TXE(USART2));
    return ch;
}

/**
 * \brief           Low-Layer init
 */
static void
LL_Init(void) {
    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* System interrupt init*/
    /* MemoryManagement_IRQn interrupt configuration */
    NVIC_SetPriority(MemoryManagement_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(BusFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(UsageFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(SVCall_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(DebugMonitor_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
}

/**
 * \brief           System Clock Configuration
 */
static void
SystemClock_Config(void) {
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

    if (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2) {
        while (1);
    }
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);
    LL_RCC_HSI_SetCalibTrimming(16);
    LL_RCC_HSI_Enable();

    /* Wait till HSI is ready */
    while (LL_RCC_HSI_IsReady() != 1);
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_8, 84, LL_RCC_PLLP_DIV_2);
    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while (LL_RCC_PLL_IsReady() != 1);
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);
    LL_Init1msTick(84000000);
    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
    LL_SetSystemCoreClock(84000000);
    LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);

    /* SysTick_IRQn interrupt configuration */
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    LL_SYSTICK_EnableIT();                      /* Enable systick interrupts */
}

/**
 * \brief           Initialize debug USART
 */
static void
debug_init(void) {
    LL_USART_InitTypeDef USART_InitStruct;
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    /* Peripheral clock enable */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

    /*
     * USART2 GPIO Configuration  
     * PA2   ------> USART2_TX
     * PA3   ------> USART2_RX 
     */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Init USART 2 */
    USART_InitStruct.BaudRate = 921600;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART2, &USART_InitStruct);
    LL_USART_ConfigAsyncMode(USART2);
    LL_USART_Enable(USART2);
}
