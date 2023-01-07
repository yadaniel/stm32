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
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "usb.h"
#include "i3g4250d_reg.h"

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

int __io_putchar(int ch) {
    uint8_t u8 = ch;
    if(HAL_UART_Transmit(&huart1, &u8, 1, HAL_MAX_DELAY) == HAL_OK) {
        return ch;
    }
    return -1;
}

int __io_getchar(void) {
    uint8_t u8 = 0;
    if(HAL_UART_Receive(&huart1, &u8, 1, HAL_MAX_DELAY) == HAL_OK) {
        return u8;
    }
    return -1;
}

//typedef int32_t (*stmdev_write_ptr)(void *, uint8_t, const uint8_t *, uint16_t);
//typedef int32_t (*stmdev_read_ptr)(void *, uint8_t, uint8_t *, uint16_t);
//typedef void (*stmdev_mdelay_ptr)(uint32_t millisec);

void tim7_delay_ms(uint32_t ms) {
    TIM7->CR1 = TIM_CR1_OPM_Msk; 	// stop timer, set one pulse mode
    TIM7->CNT = 0;
    TIM7->PSC = 48000 - 1; 	// 48MHz
    //TIM7->ARR = 1000-1; 	// 1000 => 1s
    TIM7->ARR = ms - 1;
    TIM7->SR = 0;
    TIM7->CR1 |= TIM_CR1_CEN_Msk; 	// start timer
    while(TIM7->SR != TIM_SR_UIF_Msk) {}
}

void platform_delay(uint32_t ms) {
    tim7_delay_ms(ms);
}

int32_t platform_write(void * handle, uint8_t reg, const uint8_t * bufp, uint16_t len) {
    reg |= 0x40;
    HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(handle, &reg, 1, 1000);
    HAL_SPI_Transmit(handle, (uint8_t*) bufp, len, 1000);
    HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_SET);
    return 0;
}

int32_t platform_read(void * handle, uint8_t reg, uint8_t * bufp, uint16_t len) {
    reg |= 0xC0;
    HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(handle, &reg, 1, 1000);
    HAL_SPI_Receive(handle, bufp, len, 1000);
    HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_SET);
    return 0;
}

static void tx_com(uint8_t *tx_buffer, uint16_t len) {
    HAL_UART_Transmit(&huart1, tx_buffer, len, 1000);
}

static int16_t data_raw_angular_rate[3];
static float angular_rate_mdps[3]; 	// -u print_float
static uint8_t tx_buffer[1000];
static uint8_t whoamI;
#define BOOT_TIME_MS 	10

// based on examples from https://github.com/STMicroelectronics/STMems_Standard_C_drivers

void i3g4250d_wake_up(void) {
    i3g4250d_int1_route_t int1_reg;
    i3g4250d_int1_cfg_t int1_cfg;
    stmdev_ctx_t dev_ctx;

    // Initialize mems driver interface
    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;
    dev_ctx.handle = &hspi1;

    // Initialize platform specific hardware
    // platform_init(); 	// TODO: pwm not needed?
    // Wait sensor boot time
    platform_delay(BOOT_TIME_MS);

    // Check device ID
    i3g4250d_device_id_get(&dev_ctx, &whoamI);

    if (whoamI != I3G4250D_ID) {
        printf("DEBUG: whoamI failed\n");

        // manage here device not found
        while (1) {}
    }
    printf("DEBUG: whoamI passed\n");

    /* The value of 1 LSB of the threshold corresponds to ~7.5 mdps
     * Set Threshold ~100 dps on X, Y and Z axis
     */
    i3g4250d_int_x_treshold_set(&dev_ctx, 0x3415);
    i3g4250d_int_y_treshold_set(&dev_ctx, 0x3415);
    i3g4250d_int_z_treshold_set(&dev_ctx, 0x3415);

    // Set event duration to 0 1/ODR
    i3g4250d_int_on_threshold_dur_set(&dev_ctx, 0);

    /* Simple interrupt configuration for detect wake-up
     *
     * The angular rate applied along either the
     * X, Y or Z-axis exceeds threshold
     */
    i3g4250d_int_on_threshold_conf_get(&dev_ctx, &int1_cfg);
    int1_cfg.xlie = PROPERTY_DISABLE;
    int1_cfg.ylie = PROPERTY_DISABLE;
    int1_cfg.zlie = PROPERTY_DISABLE;
    int1_cfg.lir  = PROPERTY_ENABLE;
    int1_cfg.zhie = PROPERTY_ENABLE;
    int1_cfg.xhie = PROPERTY_ENABLE;
    int1_cfg.yhie = PROPERTY_ENABLE;
    i3g4250d_int_on_threshold_conf_set(&dev_ctx, &int1_cfg);
    i3g4250d_int_on_threshold_mode_set(&dev_ctx, I3G4250D_INT1_ON_TH_OR);

    // Configure interrupt on INT1
    i3g4250d_pin_int1_route_get(&dev_ctx, &int1_reg);
    int1_reg.i1_int1 = PROPERTY_ENABLE;
    i3g4250d_pin_int1_route_set(&dev_ctx, int1_reg);

    // Set Output Data Rate
    i3g4250d_data_rate_set(&dev_ctx, I3G4250D_ODR_100Hz);

    // Wait Events Loop
    while (1) {
        i3g4250d_int1_src_t int1_src;

        // Read interrupt status
        i3g4250d_int_on_threshold_src_get(&dev_ctx, &int1_src);

        if (int1_src.ia) {
            sprintf((char *)tx_buffer, "wake-up event on ");

            if (int1_src.zh) {
                sprintf((char *)tx_buffer, "%sz", tx_buffer);
            }

            if (int1_src.yh) {
                sprintf((char *)tx_buffer, "%sy", tx_buffer);
            }

            if (int1_src.xh) {
                sprintf((char *)tx_buffer, "%sx", tx_buffer);
            }

            //sprintf((char *)tx_buffer, "debug ");
            sprintf((char *)tx_buffer, "%s\r\n", tx_buffer);
            tx_com(tx_buffer, strlen((char const *)tx_buffer));
        }
    }
}

void i3g4250d_read_data_polling(void) {
    stmdev_ctx_t dev_ctx;
    /* Uncomment to use interrupts on drdy */
    //i3g4250d_int2_route_t int2_reg;
    /* Initialize mems driver interface */
    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;
    dev_ctx.handle = &hspi1;
    /* Initialize platform specific hardware */
    //platform_init();
    /* Wait sensor boot time */
    platform_delay(BOOT_TIME_MS);
    /* Check device ID */
    i3g4250d_device_id_get(&dev_ctx, &whoamI);

    if (whoamI != I3G4250D_ID)
        while (1); /*manage here device not found */

    /* Configure filtering chain -  Gyroscope - High Pass */
    i3g4250d_filter_path_set(&dev_ctx, I3G4250D_LPF1_HP_ON_OUT);
    i3g4250d_hp_bandwidth_set(&dev_ctx, I3G4250D_HP_LEVEL_3);
    /* Uncomment to use interrupts on drdy */
    //i3g4250d_pin_int2_route_get(&dev_ctx, &int2_reg);
    //int2_reg.i2_drdy = PROPERTY_ENABLE;
    //i3g4250d_pin_int2_route_set(&dev_ctx, int2_reg);
    /* Set Output Data Rate */
    i3g4250d_data_rate_set(&dev_ctx, I3G4250D_ODR_100Hz);

    /* Read samples in polling mode (no int) */
    while (1) {
        uint8_t reg;
        /* Read output only if new value is available */
        i3g4250d_flag_data_ready_get(&dev_ctx, &reg);

        if (reg) {
            /* Read angular rate data */
            memset(data_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
            i3g4250d_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate);
            angular_rate_mdps[0] = i3g4250d_from_fs245dps_to_mdps(data_raw_angular_rate[0]);
            angular_rate_mdps[1] = i3g4250d_from_fs245dps_to_mdps(data_raw_angular_rate[1]);
            angular_rate_mdps[2] = i3g4250d_from_fs245dps_to_mdps(data_raw_angular_rate[2]);

            // TEST
            angular_rate_mdps[0] /= 1000.0f;
            angular_rate_mdps[1] /= 1000.0f;
            angular_rate_mdps[2] /= 1000.0f;

            sprintf((char *)tx_buffer,
                    "Angular Rate [mdps]:%4.1f,%4.1f,%4.1f\n",
                    angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
            tx_com(tx_buffer, strlen((char const *)tx_buffer));
        }
    }
}

void i3g4250d_fifo_read(void) {
    stmdev_ctx_t dev_ctx;
    /* Uncomment to use interrupts on drdy */
    //i3g4250d_int2_route_t int2_reg;
    /* Initialize mems driver interface */
    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;
    dev_ctx.handle = &hspi1;

    /* Initialize platform specific hardware */
    // platform_init();
    /* Wait sensor boot time */
    platform_delay(BOOT_TIME_MS);
    /* Check device ID */
    i3g4250d_device_id_get(&dev_ctx, &whoamI);

    if (whoamI != I3G4250D_ID)
        while (1); /*manage here device not found */

    /* Set FIFO watermark to 10 samples */
    i3g4250d_fifo_watermark_set(&dev_ctx, 10);
    /* Set FIFO mode to FIFO MODE */
    i3g4250d_fifo_mode_set(&dev_ctx, I3G4250D_FIFO_STREAM_MODE);
    /* Enable FIFO */
    i3g4250d_fifo_enable_set(&dev_ctx, PROPERTY_ENABLE);
    /* Uncomment to configure watermark interrupt on INT2 */
    //i3g4250d_pin_int2_route_get(&dev_ctx, &int2_route);
    //int2_route.i2_wtm = PROPERTY_ENABLE;
    //i3g4250d_pin_int2_route_set(&dev_ctx, int2_route);
    /* Set Output Data Rate */
    i3g4250d_data_rate_set(&dev_ctx, I3G4250D_ODR_100Hz);

    /* Wait Events Loop */
    while (1) {
        uint8_t flags;
        uint8_t num = 0;
        /* Read watermark interrupt flag */
        i3g4250d_fifo_wtm_flag_get(&dev_ctx, &flags);

        if (flags) {
            /* Read how many samples in fifo */
            i3g4250d_fifo_data_level_get(&dev_ctx, &num);

            while (num-- > 0) {
                /* Read angular rate data */
                memset(data_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
                i3g4250d_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate);
                angular_rate_mdps[0] = i3g4250d_from_fs245dps_to_mdps(
                                           data_raw_angular_rate[0]);
                angular_rate_mdps[1] = i3g4250d_from_fs245dps_to_mdps(
                                           data_raw_angular_rate[1]);
                angular_rate_mdps[2] = i3g4250d_from_fs245dps_to_mdps(
                                           data_raw_angular_rate[2]);
                sprintf((char *)tx_buffer,
                        "Angular Rate [mdps]:%4.2f\t%4.2f\t%4.2f\r\n",
                        angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
                tx_com(tx_buffer, strlen((char const *)tx_buffer));
            }
        }
    }
}

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
    MX_TIM7_Init();
    /* USER CODE BEGIN 2 */

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    setvbuf(stdin, NULL, _IONBF, 0);
    uint32_t cnt = 0;
    while (1) {
        HAL_Delay(100);
        cnt += 1;
        printf("cnt = %lu\n", cnt);
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        if(USART1->ISR & USART_ISR_RXNE_Msk) {
            switch(getchar()) {
            case '0':
                HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
                break;
            case '1':
                for(uint8_t i = 0; i < 10; i++) {
                    tim7_delay_ms(100);
                    HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
                }
                break;
            case '2':
                i3g4250d_wake_up();
                break;
            case '3':
                i3g4250d_read_data_polling();
                break;
            case '4':
            	i3g4250d_fifo_read();
            	break;
            default:
                break;
            }
        }

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
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
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
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB | RCC_PERIPHCLK_USART1
                                         | RCC_PERIPHCLK_I2C1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
    PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
