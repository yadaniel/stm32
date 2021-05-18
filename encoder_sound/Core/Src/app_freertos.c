/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "tim.h"
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define ARR_SIZE(arr)   (sizeof(arr)/sizeof(arr[0]))

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 128 * 4
};
/* Definitions for myTask02 */
osThreadId_t myTask02Handle;
const osThreadAttr_t myTask02_attributes = {
    .name = "myTask02",
    .priority = (osPriority_t) osPriorityLow,
    .stack_size = 128 * 4
};
/* Definitions for myTask03 */
osThreadId_t myTask03Handle;
const osThreadAttr_t myTask03_attributes = {
    .name = "myTask03",
    .priority = (osPriority_t) osPriorityLow,
    .stack_size = 128 * 4
};
/* Definitions for myTask04 */
osThreadId_t myTask04Handle;
const osThreadAttr_t myTask04_attributes = {
    .name = "myTask04",
    .priority = (osPriority_t) osPriorityLow,
    .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);
void StartTask03(void *argument);
void StartTask04(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* creation of myTask02 */
    myTask02Handle = osThreadNew(StartTask02, NULL, &myTask02_attributes);

    /* creation of myTask03 */
    myTask03Handle = osThreadNew(StartTask03, NULL, &myTask03_attributes);

    /* creation of myTask04 */
    myTask04Handle = osThreadNew(StartTask04, NULL, &myTask04_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument) {
    /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    for(;;) {
        osDelay(10);
    }
    /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument) {
    /* USER CODE BEGIN StartTask02 */
    /* Infinite loop */

    char msg[100] = {0};
    int32_t cnt = 0;

    for(;;) {

        /* cnt =(TIM2->CNT) >> 2; */
        cnt = (TIM2->CNT) >> 1;
        /* cnt =(TIM2->CNT); */
        snprintf(&msg[0], ARR_SIZE(msg), "Encoder Switch Released, Encoder Ticks = %ld\n\r", cnt);
        HAL_UART_Transmit(&huart2, (uint8_t *)&msg[0], strlen(msg), 100);
        osDelay(1000);

    }
    /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void *argument) {
    /* USER CODE BEGIN StartTask03 */
    /* Infinite loop */

    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
    for(;;) {

        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        osDelay(250);

    }
    /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the myTask04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void *argument) {
    /* USER CODE BEGIN StartTask04 */
    /* Infinite loop */

    int32_t cnt = 0;

    for(;;) {
        /* cnt =(TIM2->CNT) >> 2; */
        cnt = (TIM2->CNT) >> 1;
        /* cnt =(TIM2->CNT); */

        switch(cnt) {
            case 10:
                for(uint16_t i = 1000; i < 1500; i++) {
                    for(uint16_t j = 1000; j < 1500 + i; j++) {
                        asm("nop");
                    }
                    HAL_GPIO_TogglePin(SOUND_OUT_GPIO_Port, SOUND_OUT_Pin);
                }
                break;

            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
                for(uint16_t i = 1000; i < 1000+cnt*100; i++) {
                    for(uint16_t j = 1000; j < 1000+cnt*50 + i; j++) {
                        asm("nop");
                    }
                    HAL_GPIO_TogglePin(SOUND_OUT_GPIO_Port, SOUND_OUT_Pin);
                }
                break;

            default:
                HAL_GPIO_TogglePin(SOUND_OUT_GPIO_Port, SOUND_OUT_Pin);
                if(cnt < 1000) {
                    osDelay(cnt);
                } else {
                    osDelay(1);
                }
                break;
        }

    }
    /* USER CODE END StartTask04 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
