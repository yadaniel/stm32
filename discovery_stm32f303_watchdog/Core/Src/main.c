/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iwdg.h"
#include "wwdg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    // reset occurs due to watchdog test continuously
    // wait after reset => allow to notice the reset
    HAL_Delay(3000);

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_IWDG_Init();
    MX_WWDG_Init();
    /* USER CODE BEGIN 2 */

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    HAL_IWDG_Refresh(&hiwdg);
    //HAL_WWDG_Refresh(&hwwdg);
    //HAL_Delay(5000);
    HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
    /* HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin); */
    HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
    /* HAL_GPIO_TogglePin(LD7_GPIO_Port, LD7_Pin); */
    HAL_GPIO_TogglePin(LD9_GPIO_Port, LD9_Pin);
    /* HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin); */
    HAL_GPIO_TogglePin(LD8_GPIO_Port, LD8_Pin);
    /* HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin); */

    // WWDG setup: fAPB=48MHz, Prescaler=8, T=127, W=127 => 1/(48e6/4096/8)*2*(127-0x40) => 0.086
    // T starts with 127 and gets decremented with frequency fAPB/4096/Prescaler/2
    // reset if refresh occurs when T > W
    // reset if refresh occurs when T < 0x40

    // WWDG setup: fAPB=48MHz, Prescaler=8, T=127, W=127 => 1/(48e6/4096/8)*2*(127-0x40) => 0.086
    // min. delay 0ms, max. delay = 86ms

    // WWDG setup: fAPB=48MHz, Prescaler=8, T=127, W=96 => t=1/(48e6/4096/8)*2, Dmin = t*(127-96) = 0.043, Dmax = t*(96-32) = 0.043
    // min. delay 43ms, max. delay = 86ms

    //uint32_t cnt = 0;
    uint8_t direction = 0;  // 0 => up, 1 => down
    uint8_t delay_pass_min = 43;
    uint8_t delay_pass_max = 85;
    uint8_t delay = delay_pass_min;
    //uint8_t delay = 85;     // 43..85ms
    //uint8_t delay_changed = 0;
    while(1) {
        //cnt += delay;
        HAL_Delay(delay);

        if(direction == 0) {
            if(delay < delay_pass_max) {
                delay += 1;
            } else {
                direction = 1;
            }
        } else {
            if(delay > delay_pass_min) {
                delay -= 1;
            } else {
                direction = 0;
            }
        }

        // test make delay too short
        //if((cnt > 15000) && (delay_changed == 0)) {
        //    delay_changed = 1;
        //    delay -= 1;     // after 15s change decrease delay by 1ms
        //}

        // test make delay too long
        //if((cnt > 15000) && (delay_changed == 0)) {
        //    delay_changed = 1;
        //    delay += 1;     // after 15s change increase delay by 1ms
        //}

        HAL_WWDG_Refresh(&hwwdg);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
        // 40kHz, prescaler 256 => 256/40e3*4095 = 26.2s
        HAL_IWDG_Refresh(&hiwdg);
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    /* USER CODE BEGIN Callback 0 */

    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM1) {
        HAL_IncTick();
    }
    /* USER CODE BEGIN Callback 1 */

    /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
