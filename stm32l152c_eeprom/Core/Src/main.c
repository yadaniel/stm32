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

// 1) IWDG
// 2) WWDG
// 3) EEPROM
// 4) bitbanding

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "lcd.h"
#include "ts.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdint.h>
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum {
	eM,
	eCM,
	eMM,
	eMIL,
	eINCH,
} distance_unit_t;

typedef struct {
	distance_unit_t unit;
	int32_t value;
} distance_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define VERSION 	"0.0.1"

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#define M(x,y) 	x ## y
#define ARRAY_SIZE(arr) 	(sizeof(arr)/sizeof(arr[0]))

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

#define N 	16
uint_fast32_t arr[N] __attribute__((section(".DATA_ARR"))) = {0};

uint_least32_t cnt = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int __io_putchar(int ch) {
	ITM_SendChar(ch);
	return 0;
}

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
  MX_ADC_Init();
  MX_LCD_Init();
  MX_TS_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  //uint32_t cnt = 0;
  //uint_fast32_t cnt = 0;
  //uint_least32_t cnt = 0;

  //uint_least16_t cnt = 0;
  //uint_least8_t cnt = 0;

  for(int i=0; i<ARRAY_SIZE(arr); i+=1) {
	  arr[i] = i;
  }

  // unlock eeprom, lock again later
  FLASH->PEKEYR = 0x89ABCDEF;
  FLASH->PEKEYR = 0x02030405;
  //FLASH->PECR |= FLASH_PECR_PELOCK;

  // unlock program memory, lock again
  FLASH->PRGKEYR = 0x8C9DAEBF;
  FLASH->PRGKEYR = 0x13141516;
  //asm("nop");
  FLASH->PECR |= FLASH_PECR_PRGLOCK;

  // unlock option byte, lock again
  FLASH->OPTKEYR = 0xFBEAD9C8;
  FLASH->OPTKEYR = 0x24252627;
  FLASH->PECR |= FLASH_PECR_OPTLOCK;

  //FLASH->PECR |= FLASH_PECR_PELOCK;
  //FLASH->PEKEYR = 1; 	// hard error

  uint32_t f = 50;

  uint32_t * ptr = (uint32_t *)0x08080000; 	// data eeprom from 0x0808_0000 to 0x0808_1FFF
  uint32_t * ptr_r = (uint32_t *)0x08080000; 	// data eeprom from 0x0808_0000 to 0x0808_1FFF
  uint32_t mode = 1;
  //printf("data = %lu\n", *ptr);
  //*ptr = 0xABCD1234;
  //printf("data = %lu\n", *ptr);

  while (1)
  {
	  f -= 1;
	  cnt += 1;

	  if(mode == 0) {
		  //printf("[%06lu] loop \n", cnt);
		  printf("[%06u] loop \n", cnt);
	  }

	  HAL_Delay(10);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6 | GPIO_PIN_7);

	  if((cnt % 50) == 0) {
		  //printf("div by 0 => %lu\n", cnt/(f+1));
		  //printf("div by 0 => %lu\n", cnt/f);
	  }

	  // write to eeprom
	  /*
	  if(ptr < 0x08081FFF) {
		  *ptr = cnt;
		  ptr += 1; 	// increment by 4
		  if(ptr >= 0x08081FFF) {
			  mode = 1;
		  }
	  }
	  */

	  // read from eeprom
	  if(mode == 1) {
		  printf("eeprom[%08lX] = %08lX\n", ptr_r, *ptr_r);
		  ptr_r += 1;
		  if(ptr_r >= 0x08081FFF) {
			  ptr_r = 0x08080000;
		  }
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_LCD;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.LCDClockSelection = RCC_RTCCLKSOURCE_LSE;

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
