#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "main_fw.h"
#include "shell.h"
#include "crc_flash.h"
#include "uart2_printf.h"
#include "../Core/Inc/main.h"
/* #include "../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal.h" */
#include "../Drivers/CMSIS/Device/ST/STM32L4xx/Include/stm32l4xx.h"

volatile uint32_t tim6_cnt = 0;
volatile uint8_t tim6_irq = 0;
volatile uint32_t tim7_cnt = 0;
volatile uint8_t tim7_irq = 0;

volatile uint8_t rx_buffer[32] = {0};
volatile uint8_t rx_idx = 0;
volatile uint8_t rx_done = 0;

volatile uint8_t tx_buffer[32] = {0,1,2,3,4,5,6,7};
volatile uint8_t tx_idx = 0;
volatile uint8_t tx_stop = 0;

int main_fw(void) {
    crc_print();

    /* // 976 Hz 15625 Hz */
    /* RCC->CFGR = 0x38F0; //(HCLK/16), (SYSCLK/512), (HSI Selected clock at SW) */
    /* RCC->CR = 0x01; //HSI enabled */
    /* RCC->APB2ENR = 0x01; //Enabling System Clock enabled */
    /* RCC->APB1RSTR = 0x20; //Reseting TIM7 1=reset, 0=no-action */
    /* RCC->APB1ENR = 0x20; //Enabling TIM7 Clock */
    /* TIM7->CR1 = 0x85; //Clock enabled, URS enabled */
    /* TIM7->SR = 0x00; //Clearing the UIF (Update Interrupt Flag) */
    /* TIM7->DIER = 0x01; //Enabling the interrupt for the overflow of the Timer 7 */
    /* TIM7->EGR = 0x01; //Event Generation Register to Update Registers */
    /* TIM7->ARR = 0x8E; //The value required for the Event to occur, Counter value. */

    // TIMx_ARR => auto-reload register and the preload register ... see RM0394 counting diagram
    // TIMx_CR1.ARPE = 0, no buffer effect => modification of the TIMx_ARR affects the length of the current period
    // TIMx_CR1.ARPE = 1, buffer effect => modification of the TIMx_ARR affects the length of the next period

    /* RCC->APB1RSTR1 |= RCC_APB1RSTR1_TIM7RST;     // reset tim7 */
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM7EN;
    TIM7->CR1 = 0;      // stop tim7
    TIM7->EGR = 1;      // event generation register
    TIM7->ARR = 1000;   // 16 bits auto reload register
    TIM7->PSC = 8000;   // 16bits prescaler
    TIM7->CNT = 0;      // 16bits counter value
    TIM7->DIER = 1;     // DMA interrupt enable register
    TIM7->CR2 = 0;      // control register 2 with master mode selection
    TIM7->SR = 0;       // clear interrupt flag
    TIM7->CR1 = TIM_CR1_URS | TIM_CR1_ARPE | TIM_CR1_CEN;     // start timer
    NVIC->ISER[1] = 0x800000;   // 55>>5 => 1, hex(1<<(55 & 0x1F)) => 0x800000
    NVIC->IP[55] = (uint8_t)((2 << (8U - 4)) & (uint32_t)0xFFUL);

    /* TIM6 interrupt Init */

    // void __NVIC_EnableIRQ(IRQn_Type IRQn)
    // TIM6_DAC_IRQn = 54
    // __COMPILER_BARRIER();
    // NVIC->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    // __COMPILER_BARRIER();
    NVIC->ISER[1] = 0x400000;   // 54>>5 => 1, hex(1<<(54 & 0x1F)) => 0x400000

    // __NVIC_PRIO_BITS = 4 ... see stm32l432xx.h
    // void __NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
    // NVIC->IP[((uint32_t)IRQn)]               = (uint8_t)((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL);
    NVIC->IP[54] = (uint8_t)((2 << (8U - 4)) & (uint32_t)0xFFUL);

    /* NVIC_SetPriority(TIM6_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0)); */
    /* NVIC_EnableIRQ(TIM6_IRQn); */

    /* RCC->APB1RSTR1 |= RCC_APB1RSTR1_TIM6RST;     // reset tim6 */
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
    TIM6->CR1 = 0;      // stop tim6
    TIM6->EGR = 1;      // event generation register
    TIM6->ARR = 10000;  // 16 bits auto reload register
    TIM6->PSC = 8000;   // 16bits prescaler
    TIM6->CNT = 0;      // 16bits counter value
    TIM6->DIER = 1;     // DMA interrupt enable register
    TIM6->CR2 = 0;      // control register 2 with master mode selection
    TIM6->SR = 0;       // clear interrupt flag
    TIM6->CR1 = TIM_CR1_URS | TIM_CR1_ARPE | TIM_CR1_CEN;     // start timer
    /* TIM6->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;     // start timer */


    // usart2
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;     // enable clock
    LL_USART_EnableIT_ERROR(USART2);    // USART2->CR3 |= USART_CR3_EIE;
    LL_USART_EnableIT_RXNE(USART2);     // USART2->CR1 |= USART_CR1_RXNEIE;
    LL_USART_EnableIT_TC(USART2);       // USART2->CR1 |= USART_CR1_TCIE;

    uint32_t cnt = 0;


    while(1) {
        // check if command was received and execute command
        // only one command per main loop cycle evaluated
        shell_run();

        // 100ms delay
        tim7_irq = 0;
        while(tim7_irq == 0) {}

        // 100ms delay
        tim7_irq = 0;
        while(tim7_irq == 0) {}

        // 100ms delay
        tim7_irq = 0;
        while(tim7_irq == 0) {}

        // 1s delay
        if(tim6_cnt % 2) {
            LL_GPIO_SetOutputPin(LED1_PB3_GPIO_Port, LED1_PB3_Pin);
        } else {
            LL_GPIO_ResetOutputPin(LED1_PB3_GPIO_Port, LED1_PB3_Pin);
        }

        cnt += 1;

        // longer delay will ensure that tim6 and tim7 overflow and counters are out of sync
        //for(uint32_t i=0; i<0x1FFFFFF; i++) {}
        //UART2_print_uint32(t6, "tim6_cnt = ");
        //UART2_print_uint32(t7, "tim7_cnt = ");
        //UART2_print_uint32(cnt, "cnt = ");
        //
        // UART2_printf("cnt=%6d, tim6=%6d, tim7=%6d\n", cnt, tim6_cnt, tim7_cnt);
        /* if(rx_idx > 0) { */
        /*     __disable_irq(); */
        /*     for(uint8_t i=0; i<rx_idx; i++) { */
        /*         UART2_Sendchar(rx_buffer[i]); */
        /*     } */
        /*     rx_idx = 0; */
        /*     __enable_irq(); */
        /* } */

    }

    /* while(1) { */
    /*     uint8_t c = UART2_Getchar(); */
    /*     UART2_Sendchar(c); */
    /*     if(c=='r') { */
    /*         NVIC_SystemReset(); */
    /*     } */
    /* } */

    return 0;
}

