/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usb.h"

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

int __io_putchar(int ch) {
	uint8_t u8 = ch;
	HAL_StatusTypeDef ret = HAL_UART_Transmit(&huart1, &u8, 1, HAL_MAX_DELAY);
	if(ret == HAL_OK) {
		return 0;
	} else {
		return -1;
	}
}

int __io_getchar(void) {
	uint8_t ch = 0;
	HAL_StatusTypeDef hstatus1;
	HAL_StatusTypeDef hstatus2;
	__HAL_UART_CLEAR_OREFLAG(&huart1);
	hstatus1 = HAL_UART_Receive(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	hstatus2 = HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	if((hstatus1 == HAL_OK) && (hstatus2 == HAL_OK)) {
		return ch;
	}
	return -1;
}

// TIM6_DAC_IRQHandler defined as weak symbol in startup.s
// no need to place function pointer in NVIC table
void TIM6_DAC_IRQHandler(void) {
	TIM6->SR = 0;
	HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
	//HAL_TIM_IRQHandler(&htim6);
	//NVIC_ClearPendingIRQ(TIM6_DAC_IRQn);
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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USB_PCD_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_USART1_UART_Init();
  MX_DAC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  // scanf buffer disabled => _IONBF
  //setvbuf(stdin, NULL, _IONBF, 0);
  // scanf buffer user provided => _IOFBF, evaluated only when full
  uint8_t stdin_buffer[1] = {0};
  setvbuf(stdin, (char*)&stdin_buffer[0], _IOFBF, sizeof(stdin_buffer));

  // NVIC->IP[240] interrupt priority
  // NVIC->ICER[8] clear-enable
  // NVIC->ISER[8] set-enable
  // NVIC->ICPR[8] clear-pending
  // NVIC->ISPR[8] set-pending
  // NVIC->IABR[8] active?

  //HAL_TIM_Base_Start(&htim6); 	// enable timer only
  HAL_TIM_Base_Start_IT(&htim6); 	// enable timer and interrupt
  //TIM6->DIER |= TIM_DIER_UIE; 	// enable interrupt on update event
  //TIM6->CR1 |= TIM_CR1_CEN; 	// enable timer
  HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn); 	// 54 --> refman NVIC vector table

  HAL_TIM_Base_Start_IT(&htim7);
  //HAL_TIMEx_OnePulseN_Start_IT(&htim7, TIM_CHANNEL_1);
  HAL_NVIC_EnableIRQ(TIM7_IRQn); 		// 55 --> refman NVIC vector table

  //HAL_DAC_Start(&hdac, DAC1_CHANNEL_1);
  uint16_t dac_value[16] = {0x0FF, 0x1FF, 0x2FF, 0x3FF, 0x4FF, 0x5FF, 0x6FF, 0x7FF,
		  	  	  	  	    0x8FF, 0x9FF, 0xAFF, 0xBFF,0xCFF, 0xDFF, 0xEFF, 0xFFF};
  HAL_DAC_Start_DMA(&hdac, DAC1_CHANNEL_1, (uint32_t *)&dac_value[0], 16, DAC_ALIGN_12B_R);
  // tim6: Triger Event Selection must be changed to Update Event
  // dac: DAC trigger must be set to Timer6 trigger out event

  uint32_t arr = 500-1;
  uint32_t cnt = 0;
  uint32_t cmd = 0;
  while (1) {
	  HAL_Delay(100);
	  cnt += 1;
	  printf("cnt=%lu, arr=%lu, tim6=%3lu, tim7=%3lu\n", cnt, arr+1, TIM6->CNT, TIM7->CNT);

	  // DMA to DAC1_OUT1
	  //uint32_t dac_value = TIM6->CNT;
	  //uint32_t dac_value = 4095;
	  //HAL_DAC_Start_DMA(&hdac, DAC1_CHANNEL_1, &dac_value, 1, DAC_ALIGN_12B_R);

	  // without DMA
	  //HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_12B_L, 4095<<4);
	  //HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, 4095);
	  //HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_8B_R, 255);

	  if(USART1->ISR & USART_ISR_RXNE) {
		  printf(">>> input: ");
		  //
		  //uint8_t b[4] = {0};
		  //fgets((char*)&b[0], 4, stdin);
		  //sscanf((char*)&b[0], "%lu", &cmd);
		  //
		  //uint8_t u8 = 0;
		  //scanf("%01hhd", &u8);
		  //cmd = u8;
		  //scanf("%01lu", &cmd);
		  //
		  //cmd = 0;
		  //fread(&cmd, 1, 1, stdin);
		  //cmd -= '0';
		  //
		  cmd = getchar();
		  cmd -= '0';
		  //
		  printf("<<< input: %lu\n", cmd);
		  switch(cmd) {
		  case 0:
			  // debug
			  HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
			  break;
		  case 1:
			  // reset cnt
			  cnt = 0;
			  break;
		  case 2:
			  if(arr < 9900) {
				  arr += 100;
			  }
			  __disable_irq();
			  HAL_TIM_Base_Stop_IT(&htim6);
			  TIM6->CNT = 0;
			  TIM6->ARR = arr; 	// 0.1s longer
			  HAL_TIM_Base_Start_IT(&htim6);
			  __enable_irq();
			  break;
		  case 3:
			  if(arr > 100) {
				  arr -= 100;
			  }
			  __disable_irq();
			  HAL_TIM_Base_Stop_IT(&htim6);
			  TIM6->CNT = 0;
			  TIM6->ARR = arr; 	// 0.1 shorter
			  HAL_TIM_Base_Start_IT(&htim6);
			  __enable_irq();
			  break;
		  case 4:
			  // basic counter in OPM mode clear CEN when on update event
			  // timer value runs from 0 to ARR and stops => CEN is cleared
			  TIM7->CR1 |= TIM_CR1_CEN;
			  // HAL_TIM_Base_Start(&htim7); 	// not working
			  // HAL_TIM_Base_Start_IT(&htim7); 	// not working
			  //TIM7->EGR = 0; 	// not working
			  break;
		  case 5:
			  TIM7->EGR = 1; 	// generate update event with software
			  break;
		  case 6:
			  TIM7->CR1 |= TIM_CR1_URS_Msk; 	// setting UG will not call interrupt
			  break;
		  case 7:
			  TIM7->CR1 &= ~TIM_CR1_URS_Msk; 	// setting UG will call interrupt
			  break;
		  case 8:
			  TIM7->CR1 &= ~TIM_CR1_UDIS_Msk; 	// update events enabled
			  break;
		  case 9:
			  TIM7->CR1 |= TIM_CR1_UDIS_Msk; 	// update events disabled
			  break;
		  default:
			  break;
		  }
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_SYSCLK;
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
