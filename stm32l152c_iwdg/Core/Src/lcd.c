/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lcd.c
  * @brief   This file provides code for the configuration
  *          of the LCD instances.
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
#include "lcd.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

LCD_HandleTypeDef hlcd;

/* LCD init function */
void MX_LCD_Init(void)
{

  /* USER CODE BEGIN LCD_Init 0 */

  /* USER CODE END LCD_Init 0 */

  /* USER CODE BEGIN LCD_Init 1 */

  /* USER CODE END LCD_Init 1 */
  hlcd.Instance = LCD;
  hlcd.Init.Prescaler = LCD_PRESCALER_1;
  hlcd.Init.Divider = LCD_DIVIDER_16;
  hlcd.Init.Duty = LCD_DUTY_1_4;
  hlcd.Init.Bias = LCD_BIAS_1_4;
  hlcd.Init.VoltageSource = LCD_VOLTAGESOURCE_INTERNAL;
  hlcd.Init.Contrast = LCD_CONTRASTLEVEL_0;
  hlcd.Init.DeadTime = LCD_DEADTIME_0;
  hlcd.Init.PulseOnDuration = LCD_PULSEONDURATION_0;
  hlcd.Init.MuxSegment = LCD_MUXSEGMENT_DISABLE;
  hlcd.Init.BlinkMode = LCD_BLINKMODE_OFF;
  hlcd.Init.BlinkFrequency = LCD_BLINKFREQUENCY_DIV8;
  if (HAL_LCD_Init(&hlcd) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LCD_Init 2 */

  /* USER CODE END LCD_Init 2 */

}

void HAL_LCD_MspInit(LCD_HandleTypeDef* lcdHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(lcdHandle->Instance==LCD)
  {
  /* USER CODE BEGIN LCD_MspInit 0 */

  /* USER CODE END LCD_MspInit 0 */
    /* LCD clock enable */
    __HAL_RCC_LCD_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**LCD GPIO Configuration
    PC0     ------> LCD_SEG18
    PC1     ------> LCD_SEG19
    PC2     ------> LCD_SEG20
    PC3     ------> LCD_SEG21
    PA1     ------> LCD_SEG0
    PA2     ------> LCD_SEG1
    PA3     ------> LCD_SEG2
    PB10     ------> LCD_SEG10
    PB11     ------> LCD_SEG11
    PB12     ------> LCD_SEG12
    PB13     ------> LCD_SEG13
    PB14     ------> LCD_SEG14
    PB15     ------> LCD_SEG15
    PC6     ------> LCD_SEG24
    PC7     ------> LCD_SEG25
    PC8     ------> LCD_SEG26
    PC9     ------> LCD_SEG27
    PA8     ------> LCD_COM0
    PA9     ------> LCD_COM1
    PA10     ------> LCD_COM2
    PA15     ------> LCD_SEG17
    PC10     ------> LCD_SEG28
    PC11     ------> LCD_SEG29
    PB4     ------> LCD_SEG8
    PB5     ------> LCD_SEG9
    PB8     ------> LCD_SEG16
    PB9     ------> LCD_COM3
    */
    GPIO_InitStruct.Pin = SEG14_Pin|SEG15_Pin|SEG16_Pin|SEG17_Pin
                          |SEG18_Pin|SEG19_Pin|SEG20_Pin|SEG21_Pin
                          |SEG22_Pin|SEG23_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SEG0_Pin|SEG1_Pin|SEG2_Pin|COM0_Pin
                          |COM1_Pin|COM2_Pin|SEG12_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SEG6_Pin|SEG7_Pin|SEG8_Pin|SEG9_Pin
                          |SEG10_Pin|SEG11_Pin|SEG4_Pin|SEG5_Pin
                          |SEG13_Pin|COM3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN LCD_MspInit 1 */

  /* USER CODE END LCD_MspInit 1 */
  }
}

void HAL_LCD_MspDeInit(LCD_HandleTypeDef* lcdHandle)
{

  if(lcdHandle->Instance==LCD)
  {
  /* USER CODE BEGIN LCD_MspDeInit 0 */

  /* USER CODE END LCD_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LCD_CLK_DISABLE();

    /**LCD GPIO Configuration
    PC0     ------> LCD_SEG18
    PC1     ------> LCD_SEG19
    PC2     ------> LCD_SEG20
    PC3     ------> LCD_SEG21
    PA1     ------> LCD_SEG0
    PA2     ------> LCD_SEG1
    PA3     ------> LCD_SEG2
    PB10     ------> LCD_SEG10
    PB11     ------> LCD_SEG11
    PB12     ------> LCD_SEG12
    PB13     ------> LCD_SEG13
    PB14     ------> LCD_SEG14
    PB15     ------> LCD_SEG15
    PC6     ------> LCD_SEG24
    PC7     ------> LCD_SEG25
    PC8     ------> LCD_SEG26
    PC9     ------> LCD_SEG27
    PA8     ------> LCD_COM0
    PA9     ------> LCD_COM1
    PA10     ------> LCD_COM2
    PA15     ------> LCD_SEG17
    PC10     ------> LCD_SEG28
    PC11     ------> LCD_SEG29
    PB4     ------> LCD_SEG8
    PB5     ------> LCD_SEG9
    PB8     ------> LCD_SEG16
    PB9     ------> LCD_COM3
    */
    HAL_GPIO_DeInit(GPIOC, SEG14_Pin|SEG15_Pin|SEG16_Pin|SEG17_Pin
                          |SEG18_Pin|SEG19_Pin|SEG20_Pin|SEG21_Pin
                          |SEG22_Pin|SEG23_Pin);

    HAL_GPIO_DeInit(GPIOA, SEG0_Pin|SEG1_Pin|SEG2_Pin|COM0_Pin
                          |COM1_Pin|COM2_Pin|SEG12_Pin);

    HAL_GPIO_DeInit(GPIOB, SEG6_Pin|SEG7_Pin|SEG8_Pin|SEG9_Pin
                          |SEG10_Pin|SEG11_Pin|SEG4_Pin|SEG5_Pin
                          |SEG13_Pin|COM3_Pin);

  /* USER CODE BEGIN LCD_MspDeInit 1 */

  /* USER CODE END LCD_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
