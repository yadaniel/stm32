#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "main_fw.h"
#include "stm32g4xx_hal.h"
#include "../Core/Inc/main.h"
#include "../Core/Inc/tim.h"
#include "../Core/Inc/usart.h"
#include "../Drivers/CMSIS/Device/ST/STM32G4xx/Include/stm32g431xx.h"

#define ARR_SIZE(arr)   (sizeof(arr)/sizeof(arr[0]))

int main_fw(void) {

    char msg[100] = {0};
    int32_t cnt = 0;

    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

    while(1) {

        HAL_Delay(1000);
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

        /* cnt =(TIM2->CNT) >> 2; */
        cnt =(TIM2->CNT) >> 1;
        /* cnt =(TIM2->CNT); */
        snprintf(&msg[0], ARR_SIZE(msg), "Encoder Switch Released, Encoder Ticks = %ld\n\r", cnt);
        HAL_UART_Transmit(&huart2, (uint8_t *)&msg[0], strlen(msg), 100);

    }
    return 0;
}

