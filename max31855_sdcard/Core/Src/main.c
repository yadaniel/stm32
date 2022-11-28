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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "max31855.h"

#include "fatfs.h"

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

char line[80] = {0};
void print_uint32(uint32_t val, const char * fmt) {
    snprintf(&line[0], sizeof(line) / sizeof(line[0]), "%s%lu\n", fmt, val);
    HAL_UART_Transmit(&huart2, (uint8_t*)&line[0], strlen(line), HAL_MAX_DELAY);
}

uint8_t buffer[256] = {0};

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
  MX_USART2_UART_Init();
  MX_SPI3_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

    MX_FATFS_Init();

    FRESULT fres;
    FATFS fs = {0};
    FIL f = {0};
    FIL flog = {0};
    FILINFO fi = {0};

    print_uint32(0, "starting sdcard ...");
    HAL_Delay(500);

    /*
    fres = f_mount(&fs, "", 1);  // [3]:1 => mount now
    if(fres == FR_NO_FILESYSTEM) {
        print_uint32(fres, "FRESULT f_mount => FR_NO_FILESYSTEM");
        fres = f_mkfs("", FM_ANY, 0, &buffer[0], sizeof(buffer));
        if(fres != FR_OK) {
            print_uint32(fres, "FRESULT f_mkfs failed with => ");
        } else {
            fres = f_stat("data.txt", &fi);
            if(fres != FR_OK) {
                print_uint32(fres, "FRESULT f_stat failed with => ");
            } else {
                fres = f_open(&f, "data.txt", FA_READ);
                if(fres != FR_OK) {
                    print_uint32(fres, "FRESULT f_open failed with => ");
                } else {
                    char line[64];
                    uint8_t cnt = 0;
                    while(f_gets(&line[0], sizeof(line), &f)) {
                        print_uint32(cnt, "f_gets line => ");
                        HAL_Delay(100);
                        cnt += 1;
                    }
                }
            }
        }
    } else {
        fres = f_open(&f, "sensor.dat", FA_CREATE_ALWAYS);
        if(fres != FR_OK) {
            print_uint32(fres, "FRESULT f_open failed with => ");
        } else {
            print_uint32(fres, "FRESULT f_open succeeded ... ");
            char line[64] = "sensor data";
            fres = f_puts(&line[0], &f);
            fres = f_close(&f);
            if(fres != FR_OK) {
                print_uint32(fres, "FRESULT f_close failed with => ");
            } else {
                print_uint32(fres, "FRESULT f_close succeeded ... ");
            }
        }
    }

    fres = f_mount(&fs, "", 1);
    if(fres == FR_OK) {
        fres = f_open(&f, "foo.txt", FA_READ);
        if(fres != FR_OK) {
            print_uint32(fres, "FRESULT f_open failed with => ");
        } else {
            print_uint32(fres, "FRESULT f_open succeeded ... ");
            char line[64] = {0};
            uint8_t cnt = 0;
            while(f_gets(&line[0], sizeof(line), &f)) {
                cnt += 1;
                print_uint32(cnt, "f_gets line => ");
                HAL_Delay(100);
            }
            fres = f_close(&f);
            if(fres != FR_OK) {
                print_uint32(fres, "FRESULT f_close failed with => ");
            } else {
                print_uint32(fres, "FRESULT f_close succeeded ... ");
            }
        }
    } else {
        print_uint32(fres, "FRESULT f_mount failed with => ");
    }
    */

    uint8_t fs_mounted = 0;
    uint8_t try_mount_cnt = 0;
    uint8_t fs_made = 0;

try_mount:
    fres = f_mount(&fs, "", 1 /* mount now */);
    if(fres == FR_NO_FILESYSTEM) {
        print_uint32(fres, "FRESULT f_mount => FR_NO_FILESYSTEM");
        fres = f_mkfs("", FM_ANY, 0, &buffer[0], sizeof(buffer));
        if(fres != FR_OK) {
            print_uint32(fres, "FRESULT f_mkfs failed with => ");
            fs_made = 0;
            fs_mounted = 0;
        } else {
            // file system was created
            fs_made = 1;
            f_mount(NULL, "", 1); 	// unmount and mount it
            try_mount_cnt += 1;
            if(try_mount_cnt <= 3) {
                goto try_mount;
            }
        }
    } else {
        fs_mounted = 1;
    }

    uint8_t log = 0;
    if(fs_mounted == 1) {
        fres = f_stat("init.txt", &fi);
        if(fres != FR_OK) {
            print_uint32(fres, "FRESULT f_stat(init.txt) failed with => ");
        } else {
            fres = f_open(&f, "init.txt", FA_READ);
            if(fres != FR_OK) {
                print_uint32(fres, "FRESULT f_open(init.txt,FA_READ) failed with => ");
            } else {
            }
        }

        fres = f_open(&flog, "data.txt", FA_CREATE_ALWAYS | FA_WRITE);
        if(fres != FR_OK) {
            print_uint32(fres, "FRESULT f_open(data.txt, FA_WRITE) failed with => ");
        } else {
            log = 1;
        }
    }

    print_uint32(fs_made, "fs_made = ");
    print_uint32(fs_mounted, "fs_mounted = ");
    print_uint32(0, "starting while(1) ...");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    char msg[80] = {0};
    uint32_t cnt = 0;
    float temp = 0.0f;
    while (1) {
        HAL_Delay(250);

        cnt += 1;
        temp = max31855_read_temp();
        snprintf(&msg[0], sizeof(msg) / sizeof(msg[0]), "counter = %04lu, temp = %.2f\n", cnt, temp);
        HAL_UART_Transmit(&huart2, (uint8_t*)&msg[0], strlen(msg), HAL_MAX_DELAY);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

        if(log == 1) {
        	int res = 0;
            res = f_puts(&msg[0], &flog);
            f_sync(&flog);
        	//res = f_printf(&flog, "%10s", &msg[0]);
            if(res < 0) {
                print_uint32(res, "fputs failed with => ");
            } else {
                print_uint32(res, "fputs succeded with => ");
            }

        	/*
        	UINT written = 0;
        	fres = f_write(&flog, &msg[0], 1, &written)
            if(fres != FR_OK) {
                print_uint32(fres, "fputs failed with => ");
            } else {
                print_uint32(fres, "fputs succeded with => ");
            }
            */
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
