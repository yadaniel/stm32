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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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

volatile uint32_t tim6_cnt = 0; 	// 32e6 / 32e1 => 10us resolution
volatile bool pc8_pulse_detected = false;
volatile uint32_t pc8_pulse_duration = 0; 	// noexti --> detect in tim6_interrupt
volatile uint32_t pa8_pulse_duration = 0; 	// exti --> count based tim6
volatile uint32_t pc8_pulse_duration_rising_edge = 0;
volatile uint32_t pa8_pulse_duration_rising_edge = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	if(GPIO_Pin == PA8_EXTI_Pin) {
		if(HAL_GPIO_ReadPin(PA8_EXTI_GPIO_Port, PA8_EXTI_Pin) == GPIO_PIN_SET) {
			pa8_pulse_duration_rising_edge = tim6_cnt;
			// pa8_pulse_detected not needed, function called 2 times only --> @rising_edge and @falling_edge
		}
		if(HAL_GPIO_ReadPin(PA8_EXTI_GPIO_Port, PA8_EXTI_Pin) == GPIO_PIN_RESET) {
			if(tim6_cnt >= pa8_pulse_duration_rising_edge) {
				// no wrap around 1e6
				pa8_pulse_duration = tim6_cnt - pa8_pulse_duration_rising_edge;
			} else {
				// wrap around 1e6
				pa8_pulse_duration = 1000000 - pa8_pulse_duration_rising_edge + tim6_cnt;
			}
		}
	}

}

// valid range [0 ... 2047]
void delay_us(uint32_t us) {
	// NOTE: APB1 fmax = 32MHz, but timer clocks are x2 => 64MHz
	TIM7->CR1 = TIM_CR1_OPM_Msk; 	// one pulse mode, stop after ARR reached
	TIM7->SR = 0;
	TIM7->CNT = 0;
	TIM7->ARR = (us << 5) - 1; 		// 64MHz, prescaler = 2, 32 counts per 1us
	TIM7->CR1 = TIM_CR1_CEN_Msk; 	// start
	while(TIM7->SR != TIM_SR_UIF_Msk) {}
}

// without echo
int __io_putchar(int ch) {
	uint8_t u8 = ch;
	HAL_StatusTypeDef ret = HAL_UART_Transmit(&huart1, &u8, 1, HAL_MAX_DELAY);
	if(ret == HAL_OK) {
		return ch;
	} else {
		return -1;
	}
}

// with echo
int __io_getchar(void) {
	uint8_t ch = 0;
	HAL_StatusTypeDef ret = HAL_UART_Receive(&huart1, &ch, 1, HAL_MAX_DELAY);
	if(ret == HAL_OK) {
		// debug
		//HAL_GPIO_WritePin(LD7_GPIO_Port, LD7_Pin, GPIO_PIN_SET);
		return ch;
	} else {
		// debug
		//HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
		return -1;
	}
}

int __io_getchar_(void) {
	uint8_t ch = 0;
	HAL_StatusTypeDef hstatus1;
	HAL_StatusTypeDef hstatus2;
	//__HAL_UART_CLEAR_OREFLAG(&huart1);
	hstatus1 = HAL_UART_Receive(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	hstatus2 = HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	if((hstatus1 == HAL_OK) && (hstatus2 == HAL_OK)) {
		return ch;
	}
	return -1;
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
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  // scanf buffer disabled => _IONBF
  //setvbuf(stdin, NULL, _IONBF, 0);
  // scanf buffer user provided => _IOFBF, evaluated only when full
  //uint8_t stdin_buffer[2] = {0};
  uint8_t stdin_buffer[1] = {0};
  setvbuf(stdin, (char*)&stdin_buffer[0], _IOFBF, sizeof(stdin_buffer));

  uint32_t cnt = 0;
  while (1)
  {
	  HAL_Delay(100);
	  cnt += 1;
	  printf("cnt = %lu, tim6 = %5lu, tim6_cnt = %7lu, pc8_pd = %lu, pa8_pd = %lu\n",
			  cnt, TIM6->CNT, tim6_cnt, pc8_pulse_duration*10, pa8_pulse_duration*10);

	  if(USART1->ISR & USART_ISR_RXNE_Msk) {
		  // USART1->ISR &= ~USART_ISR_RXNE_Msk; 	// does not clear the bit
		  // this bit is cleared by reading from RDR
		  //uint8_t ch = USART1->RDR;
		  // or by writing to RQR
		  //USART1->RQR |= USART_RQR_RXFRQ_Msk;
		  // or one of above will be done implicitly by using getchar
		  // however when using getchar the input buffer is used and its must be set to 1 via
		  // setvbuf(stdin, (char*)&stdin_buffer[0], _IOFBF, 1);
		  // or the input buffer must be disabled via
		  // setvbuf(stdin, NULL, _IONBF, 0);
		  // NOTE: in non-interrupt modus with delay in main loop the input buffer not fast enough
		  uint8_t u8 = getchar();

		  switch(u8) {
		  case 'r':
			  cnt = 0;
			  break;
		  case 's':
			  // HAL_TIM_Base_Start(&htim6);
			  HAL_TIM_Base_Start_IT(&htim6);
			  // HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn); 	// already done in tim.c
			  break;
		  case 'S':
			  break;
		  case '0':
			  pc8_pulse_duration = 0;
			  pa8_pulse_duration = 0;
			  break;
		  case '1':
			  HAL_GPIO_WritePin(PC9_OUT_GPIO_Port, PC9_OUT_Pin, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(PA9_OUT_GPIO_Port, PA9_OUT_Pin, GPIO_PIN_SET);
			  //HAL_Delay(1);
			  delay_us(1000);
			  HAL_GPIO_WritePin(PC9_OUT_GPIO_Port, PC9_OUT_Pin, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(PA9_OUT_GPIO_Port, PA9_OUT_Pin, GPIO_PIN_RESET);
			  break;
		  case '2':
			  HAL_GPIO_WritePin(PC9_OUT_GPIO_Port, PC9_OUT_Pin, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(PA9_OUT_GPIO_Port, PA9_OUT_Pin, GPIO_PIN_SET);
			  //HAL_Delay(2);
			  delay_us(2000);
			  HAL_GPIO_WritePin(PC9_OUT_GPIO_Port, PC9_OUT_Pin, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(PA9_OUT_GPIO_Port, PA9_OUT_Pin, GPIO_PIN_RESET);
			  break;
		  case '3':
			  HAL_GPIO_WritePin(PC9_OUT_GPIO_Port, PC9_OUT_Pin, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(PA9_OUT_GPIO_Port, PA9_OUT_Pin, GPIO_PIN_SET);
			  //HAL_Delay(3);
			  delay_us(1000);
			  delay_us(2000);
			  HAL_GPIO_WritePin(PC9_OUT_GPIO_Port, PC9_OUT_Pin, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(PA9_OUT_GPIO_Port, PA9_OUT_Pin, GPIO_PIN_RESET);
			  break;
		  case '4':
			  HAL_GPIO_WritePin(PC9_OUT_GPIO_Port, PC9_OUT_Pin, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(PA9_OUT_GPIO_Port, PA9_OUT_Pin, GPIO_PIN_SET);
			  //HAL_Delay(4);
			  delay_us(2000);
			  delay_us(2000);
			  HAL_GPIO_WritePin(PC9_OUT_GPIO_Port, PC9_OUT_Pin, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(PA9_OUT_GPIO_Port, PA9_OUT_Pin, GPIO_PIN_RESET);
			  break;
		  case '5':
			  HAL_GPIO_WritePin(PC9_OUT_GPIO_Port, PC9_OUT_Pin, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(PA9_OUT_GPIO_Port, PA9_OUT_Pin, GPIO_PIN_SET);
			  //HAL_Delay(5);
			  delay_us(1000);
			  delay_us(2000);
			  delay_us(2000);
			  HAL_GPIO_WritePin(PC9_OUT_GPIO_Port, PC9_OUT_Pin, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(PA9_OUT_GPIO_Port, PA9_OUT_Pin, GPIO_PIN_RESET);
			  break;
		  case '6':
			  break;
		  case '7':
			  break;
		  case '8':
			  break;
		  case '9':
			  break;
		  case 'a':
			  HAL_GPIO_WritePin(LD10_GPIO_Port, LD10_Pin, GPIO_PIN_SET);
			  break;
		  case 'b':
			  HAL_GPIO_WritePin(LD10_GPIO_Port, LD10_Pin, GPIO_PIN_RESET);
			  break;
		  default:
			  break;
		  }

		  // debug purpose
		  //HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
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
