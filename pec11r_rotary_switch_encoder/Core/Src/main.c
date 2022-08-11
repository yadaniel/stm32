/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

// fix cubemx bug
#include "usb.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

// check the wiring of USART to ST-LINK
int __io_putchar_UART(int ch) {
	uint8_t tmp = (uint8_t)ch;
	HAL_UART_Transmit(&huart1, &tmp, 1, 100);
	return 0;
}

// check the wiring of T_SWO/PB3 to ST-LINK
// usage with debugger, STM32CubeProgrammer, STM32 ST-LINK Utility
// set manually the correct core frequency and enable ITM stimulus port bit 0
int __io_putchar_ITM(int ch) {
	ITM_SendChar(ch);
	return 0;
}

#define PRINTF_UART
//#define PRINTF_ITM

int __io_putchar(int ch) {
#if defined(PRINTF_UART)
	return __io_putchar_UART(ch);
#elif defined(PRINTF_ITM)
	return __io_putchar_ITM(ch);
#else
	return 0;
#endif
}

volatile int32_t enc_position = 0;

volatile uint32_t enc_a_pos;
volatile uint32_t enc_a_neg;
volatile uint32_t enc_b_pos;
volatile uint32_t enc_b_neg;

volatile int8_t enc_a_dir;
volatile int8_t enc_b_dir;

#define ENC_LEFT 	(-1)
#define ENC_RIGHT 	(+1)

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	// evaluating rising and falling edges
	// between two mechanical intermediate positions two rising edges are detected
	// rising(enc_a) => state(b) = 0 => cw, state(b) = 1 => ccw
	// falling(enc_a) => state(b) = 1 => cw, state(b) = 0 => ccw
	// rising(enc_b) => state(a) = 1 => cw, state(a) = 0 => ccw
	// falling(enc_b) => state(a) = 0 => cw, state(a) = 1 => ccw

	if(GPIO_Pin == ENC_A_Pin) {
		// ENC_A_Pin, GPIO_PIN_10

		if(HAL_GPIO_ReadPin(ENC_A_GPIO_Port, ENC_A_Pin) == GPIO_PIN_SET) {
			// enc_a rising edge

			// debug
			enc_a_pos += 1;

			if(HAL_GPIO_ReadPin(ENC_B_GPIO_Port, ENC_B_Pin) == GPIO_PIN_RESET) {
				enc_a_dir = ENC_RIGHT;
			} else {
				enc_a_dir = ENC_LEFT;
			}
		} else {
			// enc_a falling edge

			// debug
			enc_a_neg += 1;

			if(HAL_GPIO_ReadPin(ENC_B_GPIO_Port, ENC_B_Pin) == GPIO_PIN_SET) {
				enc_a_dir = ENC_RIGHT;
			} else {
				enc_a_dir = ENC_LEFT;
			}
		}
	}

	if(GPIO_Pin == ENC_B_Pin) {
		// ENC_B_Pin, GPIO_PIN_90

		if(HAL_GPIO_ReadPin(ENC_B_GPIO_Port, ENC_B_Pin) == GPIO_PIN_SET) {
			// enc_b rising edge

			// debug
			enc_b_pos += 1;

			if(HAL_GPIO_ReadPin(ENC_A_GPIO_Port, ENC_A_Pin) == GPIO_PIN_SET) {
				enc_b_dir = ENC_RIGHT;
			} else {
				enc_b_dir = ENC_LEFT;
			}
		} else {
			// enc_b falling edge

			// debug
			enc_b_neg += 1;

			if(HAL_GPIO_ReadPin(ENC_A_GPIO_Port, ENC_A_Pin) == GPIO_PIN_RESET) {
				enc_b_dir = ENC_RIGHT;
			} else {
				enc_b_dir = ENC_LEFT;
			}
		}
	}

	// encoder position
	if((enc_a_dir == ENC_RIGHT) && (enc_b_dir == ENC_RIGHT)) {
		enc_position -= 1;
	}

	if((enc_a_dir == ENC_LEFT) && (enc_b_dir == ENC_LEFT)) {
		enc_position += 1;
	}

}

void HAL_GPIO_EXTI_Callback_(uint16_t GPIO_Pin) {

	// evaluating only rising edges
	// between two mechanical intermediate positions two rising edges are detected
	// rising(enc_a) => state(b) = 0 => cw, state(b) = 1 => ccw
	// rising(enc_b) => state(a) = 1 => cw, state(a) = 0 => ccw

	if(GPIO_Pin == ENC_A_Pin) {

		// debug
		enc_a_pos += 1;

		// ENC_A_Pin, GPIO_PIN_10
		if(HAL_GPIO_ReadPin(ENC_B_GPIO_Port, ENC_B_Pin) == GPIO_PIN_RESET) {
			enc_a_dir = ENC_RIGHT;
		} else {
			enc_a_dir = ENC_LEFT;
		}
	} else if(GPIO_Pin == ENC_B_Pin) {

		// debug
		enc_b_pos += 1;

		// ENC_B_Pin, GPIO_PIN_9
		if(HAL_GPIO_ReadPin(ENC_A_GPIO_Port, ENC_A_Pin) == GPIO_PIN_SET) {
			enc_b_dir = ENC_RIGHT;
		} else {
			enc_b_dir = ENC_LEFT;
		}
	}

	// encoder position
	if((enc_a_dir == ENC_RIGHT) && (enc_b_dir == ENC_RIGHT)) {
		enc_position -= 1;
	}

	if((enc_a_dir == ENC_LEFT) && (enc_b_dir == ENC_LEFT)) {
		enc_position += 1;
	}

}

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
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USB_PCD_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint32_t cnt = 0;
  int32_t enc_position_latch = enc_position;
  while (1) {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  if(enc_position != enc_position_latch) {
		  char delta = ' ';
		  if(abs(enc_position - enc_position_latch) != 1) {
			  delta = '!';
		  }
		  enc_position_latch = enc_position; 	// next loop

		  printf("cnt = %lu, enc_a = (%lu,%lu), enc_b = (%lu,%lu), enc_position = %ld, [%c]\n", cnt, enc_a_pos, enc_a_neg, enc_b_pos, enc_b_neg, enc_position, delta);
		  cnt += 1;
	  }

	  /*
	  uint32_t leds = abs(enc_position);
	  switch(leds % 8) {
	  case 0:
		  HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		  break;
	  case 1:
		  HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
		  break;
	  case 2:
		  HAL_GPIO_TogglePin(LD7_GPIO_Port, LD7_Pin);
		  break;
	  case 3:
		  HAL_GPIO_TogglePin(LD9_GPIO_Port, LD9_Pin);
		  break;
	  case 4:
		  HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
		  break;
	  case 5:
		  HAL_GPIO_TogglePin(LD8_GPIO_Port, LD8_Pin);
		  break;
	  case 6:
		  HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
		  break;
	  case 7:
		  HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
		  break;
	  }
	  */

	  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LD7_GPIO_Port, LD7_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LD9_GPIO_Port, LD9_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LD10_GPIO_Port, LD10_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LD8_GPIO_Port, LD8_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);

	  uint32_t leds = abs(enc_position);
	  switch(leds % 8) {
	  case 0:
		  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
		  break;
	  case 1:
		  HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
		  break;
	  case 2:
		  HAL_GPIO_WritePin(LD7_GPIO_Port, LD7_Pin, GPIO_PIN_SET);
		  break;
	  case 3:
		  HAL_GPIO_WritePin(LD9_GPIO_Port, LD9_Pin, GPIO_PIN_SET);
		  break;
	  case 4:
		  HAL_GPIO_WritePin(LD10_GPIO_Port, LD10_Pin, GPIO_PIN_SET);
		  break;
	  case 5:
		  HAL_GPIO_WritePin(LD8_GPIO_Port, LD8_Pin, GPIO_PIN_SET);
		  break;
	  case 6:
		  HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);
		  break;
	  case 7:
		  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
		  break;
	  }
  }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
