/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
struct hardSerial_t {
    uint8_t Number[8];
};
uint64_t data = 0;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define DS2401_SUCCESS          0
#define DS2401_WRONG_CRC        1
#define DS2401_NO_RESPONSE      2
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
struct hardSerial_t HardSerial;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myTask02 */
osThreadId_t myTask02Handle;
const osThreadAttr_t myTask02_attributes = {
  .name = "myTask02",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myTask03 */
osThreadId_t myTask03Handle;
const osThreadAttr_t myTask03_attributes = {
  .name = "myTask03",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myTask04 */
osThreadId_t myTask04Handle;
const osThreadAttr_t myTask04_attributes = {
  .name = "myTask04",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myQueue01 */
osMessageQueueId_t myQueue01Handle;
const osMessageQueueAttr_t myQueue01_attributes = {
  .name = "myQueue01"
};
/* Definitions for myQueue02 */
osMessageQueueId_t myQueue02Handle;
const osMessageQueueAttr_t myQueue02_attributes = {
  .name = "myQueue02"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
inline static uint8_t onewire_in(void);
inline static void onewire_out(uint8_t);
inline static void onewire_dir_in(void);
inline static void onewire_dir_out(void);

static void shiftToLeft(uint8_t * Ptr_u8, uint8_t Size_u8, int8_t Bitval_u1);
static void shiftToRight(uint8_t * Ptr_u8, uint8_t Size_u8, int8_t Bitval_u1);
uint32_t DWT_Delay_Init(void);
void DWT_Delay_us(volatile uint32_t microseconds);
void TIM6_delay_us(uint16_t us);
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

  /* Create the queue(s) */
  /* creation of myQueue01 */
  myQueue01Handle = osMessageQueueNew (16, sizeof(uint16_t), &myQueue01_attributes);

  /* creation of myQueue02 */
  myQueue02Handle = osMessageQueueNew (16, sizeof(uint32_t), &myQueue02_attributes);

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
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    for(;;) {
        osDelay(1);
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
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
    /* Infinite loop */

	// stm32l432kc => FLASH 256K => 128 pages
	// hex(0x0800_0000 + 2*2**10*100) => 0x8032000
	/*
	 0x8000000,0x8000800,0x8001000,0x8001800,0x8002000,0x8002800,0x8003000,0x8003800,
	 0x8004000,0x8004800,0x8005000,0x8005800,0x8006000,0x8006800,0x8007000,0x8007800,
	 0x8008000,0x8008800,0x8009000,0x8009800,0x800a000,0x800a800,0x800b000,0x800b800,
	 0x800c000,0x800c800,0x800d000,0x800d800,0x800e000,0x800e800,0x800f000,0x800f800,
	 0x8010000,0x8010800,0x8011000,0x8011800,0x8012000,0x8012800,0x8013000,0x8013800,
	 0x8014000,0x8014800,0x8015000,0x8015800,0x8016000,0x8016800,0x8017000,0x8017800,
	 0x8018000,0x8018800,0x8019000,0x8019800,0x801a000,0x801a800,0x801b000,0x801b800,
	 0x801c000,0x801c800,0x801d000,0x801d800,0x801e000,0x801e800,0x801f000,0x801f800,
	 0x8020000,0x8020800,0x8021000,0x8021800,0x8022000,0x8022800,0x8023000,0x8023800,
	 0x8024000,0x8024800,0x8025000,0x8025800,0x8026000,0x8026800,0x8027000,0x8027800,
	 0x8028000,0x8028800,0x8029000,0x8029800,0x802a000,0x802a800,0x802b000,0x802b800,
	 0x802c000,0x802c800,0x802d000,0x802d800,0x802e000,0x802e800,0x802f000,0x802f800,
	 0x8030000,0x8030800,0x8031000,0x8031800,0x8032000,0x8032800,0x8033000,0x8033800,
	 0x8034000,0x8034800,0x8035000,0x8035800,0x8036000,0x8036800,0x8037000,0x8037800,
	 0x8038000,0x8038800,0x8039000,0x8039800,0x803a000,0x803a800,0x803b000,0x803b800,
	 0x803c000,0x803c800,0x803d000,0x803d800,0x803e000,0x803e800,0x803f000,0x803f800 -> 0x8040000
	 */

	typedef enum PAGE_ERASE_STATUS {
		eNOT_CLEANED,
		eERASE_SUCCESS,
		eERASE_FAILURE,
	} PAGE_ERASE_STATUS_t;

	PAGE_ERASE_STATUS_t page_status = eNOT_CLEANED;

	uint8_t written = 0;
	uint32_t data1 = 0;
	uint32_t data2 = 0;
	//uint32_t * ptr = (uint32_t *)0x8032000; 	// hex(0x0800_0000 + 2*2**10*100) => 0x8032000
	//uint32_t * ptr = (uint32_t *)0x8032000 + 8; 	// hex(0x0800_0000 + 2*2**10*100) => 0x8032000
	//uint32_t * ptr = (uint32_t *)0x8032000 + 16; 	// hex(0x0800_0000 + 2*2**10*100) => 0x8032000
	uint32_t * ptr = (uint32_t *)0x8032000 + 4; 	// hex(0x0800_0000 + 2*2**10*100) => 0x8032000
	// ptr 4 byte aligned, 2 32-bits values per write

	data = data1;
	data <<= 32;
	data |= data2;

    for(;;) {

        osDelay(1000);

        if(page_status == eNOT_CLEANED) {
        	if((FLASH->SR & FLASH_SR_BSY_Msk) == 0) {
        		// clear FLASH page

        		// unlock FLASH
        		if(FLASH->CR & FLASH_CR_LOCK_Msk) {
        			FLASH->KEYR = ((uint32_t)0x45670123);	// hard fault when wrong sequence
        			FLASH->KEYR = ((uint32_t)0xCDEF89AB);	// e.g. comment out first or second key
        		}

        		// wait until done
        		while(FLASH->SR & FLASH_SR_BSY_Msk) {
        			// wait for busy to be cleared
        		}

        		// clear FLASH errors
        		FLASH->SR |= FLASH_SR_OPTVERR_Msk; 	// FLASH option not valid. Cleared by writing 1.
        		FLASH->SR |= FLASH_SR_RDERR_Msk; 	// FLASH reading from protected address. Cleared by writing 1.
        		FLASH->SR |= FLASH_SR_FASTERR_Msk; 	// FLASH fast programming interrupted. Cleared by writing 1.
        		FLASH->SR |= FLASH_SR_MISERR_Msk; 	// FLASH fast programming data missing. Cleared by wiring 1.
        		FLASH->SR |= FLASH_SR_PGSERR_Msk; 	// FLASH programming sequence error. Cleared by writing 1.
        		FLASH->SR |= FLASH_SR_SIZERR_Msk; 	// FLASH not word size access. Cleared by writing 1.
        		FLASH->SR |= FLASH_SR_PGAERR_Msk;	// FLASH write align error not 64bit row. Cleared by writing 1.
        		FLASH->SR |= FLASH_SR_WRPERR_Msk; 	// FLASH not writable at address. Cleared by writing 1.
        		FLASH->SR |= FLASH_SR_PROGERR_Msk; 	// FLASH not 0xFF before write, except 0x00. Cleared by writing 1.
        		FLASH->SR |= FLASH_SR_OPERR_Msk; 	// FLASH program or erase operation fails. Cleared by wiring 1.

        		// wait until done
        		while(FLASH->SR & FLASH_SR_BSY_Msk) {
        			// wait for busy to be cleared
        		}

        		// select page to erase and start
        		FLASH->CR |= FLASH_CR_PER_Msk; 			// set FLASH erase permit
        		FLASH->CR |= (FLASH_CR_PNB_Msk & (100 << FLASH_CR_PNB_Pos)); 	// select FLASH page number to erase
        		FLASH->CR |= FLASH_CR_STRT_Msk; 		// start erase

        		// wait until done
        		while(FLASH->SR & FLASH_SR_BSY_Msk) {
        			// wait for busy to be cleared
        		}

        		// check if erase successful
        		if(FLASH->SR & FLASH_SR_PGSERR_Msk) {
        			page_status = eERASE_FAILURE;
        		} else {
        			page_status = eERASE_SUCCESS;
        		}
        	}
        } else if (((FLASH->SR & FLASH_SR_BSY_Msk) == 0) && (page_status == eERASE_SUCCESS) && (written == 0)) {

    		// unlock FLASH
    		if(FLASH->CR & FLASH_CR_LOCK_Msk) {
    			FLASH->KEYR = ((uint32_t)0x45670123);	// hard fault when wrong sequence
    			FLASH->KEYR = ((uint32_t)0xCDEF89AB);	// e.g. comment out first or second key
    		}

    		// wait until done
    		while(FLASH->SR & FLASH_SR_BSY_Msk) {
    			// wait for busy to be cleared
    		}

    		// clear FLASH errors
    		FLASH->SR |= FLASH_SR_OPTVERR_Msk; 	// FLASH option not valid. Cleared by writing 1.
    		FLASH->SR |= FLASH_SR_RDERR_Msk; 	// FLASH reading from protected address. Cleared by writing 1.
    		FLASH->SR |= FLASH_SR_FASTERR_Msk; 	// FLASH fast programming interrupted. Cleared by writing 1.
    		FLASH->SR |= FLASH_SR_MISERR_Msk; 	// FLASH fast programming data missing. Cleared by wiring 1.
    		FLASH->SR |= FLASH_SR_PGSERR_Msk; 	// FLASH programming sequence error. Cleared by writing 1.
    		FLASH->SR |= FLASH_SR_SIZERR_Msk; 	// FLASH not word size access. Cleared by writing 1.
    		FLASH->SR |= FLASH_SR_PGAERR_Msk;	// FLASH write align error not 64bit row. Cleared by writing 1.
    		FLASH->SR |= FLASH_SR_WRPERR_Msk; 	// FLASH not writable at address. Cleared by writing 1.
    		FLASH->SR |= FLASH_SR_PROGERR_Msk; 	// FLASH not 0xFF before write, except 0x00. Cleared by writing 1.
    		FLASH->SR |= FLASH_SR_OPERR_Msk; 	// FLASH program or erase operation fails. Cleared by wiring 1.

    		// PER must be cleared when PG is set
    		FLASH->CR &= ~FLASH_CR_PER_Msk; 	// clear FLASH erase permit

    		// allow FLASH programming
    		FLASH->CR |= FLASH_CR_PG_Msk;

    		// wait until done
    		while(FLASH->SR & FLASH_SR_BSY_Msk) {
    			// wait for busy to be cleared
    		}

    		// write
        	FLASH->CR |= FLASH_CR_EOPIE_Msk; 	// allow to set EOP
        	FLASH->SR |= FLASH_SR_EOP_Msk; 	// clear by writing 1
        	*ptr = 12345;	 			// only possible to program 2x 32-bit data
        	*(ptr+1) = 54321;			// see refman

    		// wait until done
    		while(FLASH->SR & FLASH_SR_BSY_Msk) {
    			// wait for busy to be cleared
    		}

    		// this bit set only after second written 32-bit value
    		if(FLASH->SR & FLASH_SR_EOP_Msk) {
    			FLASH->SR |= FLASH_SR_EOP_Msk; 	// clear by writing 1
    		}

    		// disallow FLASH programming
    		FLASH->CR &= ~FLASH_CR_PG_Msk;

        	written = 1;

    	} else if ((page_status == eERASE_SUCCESS) && (written == 1)) {
    		data1 = *ptr; 	// read
    		data2 = *(ptr+1); 	// read
    	}
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
void StartTask03(void *argument)
{
  /* USER CODE BEGIN StartTask03 */
    /* Infinite loop */

    uint16_t cnt = 0;

    for(;;) {
        uint8_t Idx_u8;
        uint8_t CRCtmp_u8;
        uint8_t CRC_u8;
        uint8_t Bitval_u8;
        uint8_t errVal = DS2401_SUCCESS;

        // initialize
        for(Idx_u8 = 0u; Idx_u8 < 8u; Idx_u8++) {
            HardSerial.Number[Idx_u8] = 0u;
        }

        // start
        onewire_out(0);
        onewire_dir_out();
        TIM6_delay_us(600);

        // wait for slave bus response
        onewire_dir_in();
        TIM6_delay_us(60);
        if(onewire_in() == 1u) {
            // sample slave bus response
            errVal = DS2401_NO_RESPONSE;
            for(Idx_u8 = 0u; Idx_u8 < 8u; Idx_u8++) {
                HardSerial.Number[Idx_u8] = 0u;
            }
        } else {
        	TIM6_delay_us(240);

            // read command 0x33
            // 1
        	onewire_out(0);
            onewire_dir_out();
            DWT_Delay_us(2);
            onewire_out(1);
            DWT_Delay_us(115);
            // 1
            onewire_out(0);
            DWT_Delay_us(2);
            onewire_out(1);
            DWT_Delay_us(115);
            // 0
            onewire_out(0);
            DWT_Delay_us(110);
            onewire_out(1);
            DWT_Delay_us(2);
            // 0
            onewire_out(0);
            DWT_Delay_us(110);
            onewire_out(1);
            DWT_Delay_us(2);
            // 1
            onewire_out(0);
            DWT_Delay_us(2);
            onewire_out(1);
            DWT_Delay_us(115);
            // 1
            onewire_out(0);
            DWT_Delay_us(2);
            onewire_out(1);
            DWT_Delay_us(115);
            // 0
            onewire_out(0);
            DWT_Delay_us(110);
            onewire_out(1);
            DWT_Delay_us(2);
            // 0
            onewire_out(0);
            DWT_Delay_us(110);
            onewire_out(1);
            DWT_Delay_us(2);

            CRC_u8 = 0u;

            for(Idx_u8 = 0u; Idx_u8 < 64u; Idx_u8++) {
            	onewire_out(0);     // acknowledge each bit
                onewire_dir_out();     // acknowledge each bit
                onewire_dir_in();
                DWT_Delay_us(5);

                Bitval_u8 = (onewire_in() == 1u) ? 1u : 0u;
                shiftToLeft(&(HardSerial.Number[0]), 8u, Bitval_u8);
                CRCtmp_u8 = CRC_u8 & 0x01u;
                CRCtmp_u8 ^= Bitval_u8;
                shiftToRight(&CRC_u8, 1u, CRCtmp_u8);
                if(CRCtmp_u8) {
                    CRC_u8 = CRC_u8 ^ 0x0Cu;
                }

                DWT_Delay_us(60);
            }

            onewire_dir_in();

            if(CRC_u8 != 0u) {
                errVal = DS2401_WRONG_CRC;
                for(Idx_u8 = 0u; Idx_u8 < 8u; Idx_u8++) {
                    HardSerial.Number[Idx_u8] = 0u;
                }
            }
        }

        char msg[64] = {0};
        cnt += 1;
        switch(errVal) {
        case DS2401_SUCCESS: {
            uint8_t n0 = HardSerial.Number[0];
            uint8_t n1 = HardSerial.Number[1];
            uint8_t n2 = HardSerial.Number[2];
            uint8_t n3 = HardSerial.Number[3];
            uint8_t n4 = HardSerial.Number[4];
            uint8_t n5 = HardSerial.Number[5];
            uint8_t n6 = HardSerial.Number[6];
            uint8_t n7 = HardSerial.Number[7];
            uint8_t len = snprintf(&msg[0], 64, "[%4d] id = %X,%X,%X,%X,%X,%X,%X,%X\n", cnt, n0, n1, n2, n3, n4, n5, n6, n7);
            HAL_UART_Transmit(&huart2, (uint8_t *)&msg[0], len, 100);
        }
        break;

        case DS2401_NO_RESPONSE: {
            uint8_t len = snprintf(&msg[0], 64, "[%4d] failed response\n", cnt);
            HAL_UART_Transmit(&huart2, (uint8_t *)&msg[0], len, 100);
        }
        break;

        case DS2401_WRONG_CRC: {
            uint8_t len = snprintf(&msg[0], 64, "[%4d] failed crc\n", cnt);
            HAL_UART_Transmit(&huart2, (uint8_t *)&msg[0], len, 100);
        }
        break;

        default: {
            uint8_t len = snprintf(&msg[0], 64, "[%4d] failed default\n", cnt);
            HAL_UART_Transmit(&huart2, (uint8_t *)&msg[0], len, 100);
        }
        break;
        }

        osDelay(500);
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
void StartTask04(void *argument)
{
  /* USER CODE BEGIN StartTask04 */
    /* Infinite loop */
    for(;;) {

        osDelay(1000);

    	/*
        osDelay(1);
    	for(uint16_t i=0; i<20; i++) {
    		TIM6_delay_us(50000);
    	}
    	*/

        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

    }
  /* USER CODE END StartTask04 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
inline static void onewire_dir_out(void) {
    GPIOA->MODER |= 0x04; 	// PA1 output
    GPIOA->OTYPER |= 0x02;	// PA1 open drain
    GPIOA->OSPEEDR |= 0x0C; 	// PA1 very high speed
    /*
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = onewire_io_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    //GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(onewire_io_GPIO_Port, &GPIO_InitStruct);
    */
}

inline static void onewire_dir_in(void) {
    GPIOA->MODER &= ~(0x0C);	// PA1 input
    /*
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = onewire_io_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(onewire_io_GPIO_Port, &GPIO_InitStruct);
    */
}

inline static void onewire_out(uint8_t bit) {

    if(bit) {
        GPIOA->ODR |= 0x02;
    } else {
        GPIOA->ODR &= ~0x02;
    }

    /*
    if(bit == 0) {
        HAL_GPIO_WritePin(onewire_io_GPIO_Port, onewire_io_Pin, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(onewire_io_GPIO_Port, onewire_io_Pin, GPIO_PIN_SET);
    }
    */
}

inline static uint8_t onewire_in(void) {

    return (GPIOA->IDR & 0x02) == 0x02;

    /*
    if(HAL_GPIO_ReadPin(onewire_io_GPIO_Port, onewire_io_Pin) == GPIO_PIN_RESET) {
        return 0;
    } else {
        return 1;
    }
    */
}

/****************************************************************************************/
/**
    @name     shiftToLeft
    @brief    shift bits to the left, Bitval_u1 will be shifted in on the right side
              Ptr_u8[0] = most right byte
              Ptr_u8[n-1] = most left byte

    @return   void

    @param    [in,out] Ptr_u8 = pointer to array
    @param    [in] Size_u8 = array size, range 0..255
    @param    [in] Bitval_u1 = bit value to shift in
*****************************************************************************************/
static void shiftToLeft(uint8_t * Ptr_u8, uint8_t Size_u8, int8_t Bitval_u1) {
    /* [msb(n-1)...lsb(n-1)] .. [msb1...lsb1] << [msb0..lsb0] << bitval */
    /* Ptr_u8[n-1] .. Ptr_u8[1] << Ptr_u8[0] << Bitval_u1 */
    uint8_t Idx_u8;
    uint8_t Tmp_u8;
    int8_t Shiftin_u1;
    int8_t Shiftout_u1;
    Shiftin_u1 = Bitval_u1 & 0x01u;
    for(Idx_u8 = 0u; Idx_u8 < Size_u8; Idx_u8++) {
        Tmp_u8 = Ptr_u8[Idx_u8];
        Shiftout_u1 = ((Tmp_u8 & 0x80u) == 0x80u) ? 1u : 0u;
        Tmp_u8 <<= 1;
        Tmp_u8 |= Shiftin_u1;
        Ptr_u8[Idx_u8] = Tmp_u8;
        Shiftin_u1 = Shiftout_u1;
    }
}

/****************************************************************************************/
/**
    @name     shiftToRight
    @brief    shift bits to the right, Bitval_u1 will be shifted in on the left side
              Ptr_u8[0] = most right byte
              Ptr_u8[n-1] = most left byte

    @return   void

    @param    [in,out] Ptr_u8 = pointer to array
    @param    [in] Size_u8 = array size, range 0..255
    @param    [in] Bitval_u1 = bit value to shift in
*****************************************************************************************/
static void shiftToRight(uint8_t * Ptr_u8, uint8_t Size_u8, int8_t Bitval_u1) {
    /* bitval >> [msb(n-1)...lsb(n-1)] .. [msb1...lsb1] >> [msb0..lsb0] */
    /* bitval >> ptr[n-1] .. ptr[1] >> ptr[0] */
    uint8_t Tmp_u8;
    int8_t Shiftin_u1;
    int8_t Shiftout_u1;
    Shiftin_u1 = Bitval_u1 & 0x01u;
    while(Size_u8) {
        Tmp_u8 = Ptr_u8[Size_u8 - 1u];
        Shiftout_u1 = ((Tmp_u8 & 1u) == 1u) ? 1u : 0u;
        Tmp_u8 >>= 1;
        if(Shiftin_u1 == 0x01u) {
            Tmp_u8 |= 0x80u;
        }
        Ptr_u8[Size_u8 - 1u] = Tmp_u8;
        Shiftin_u1 = Shiftout_u1;
        Size_u8 -= 1u;
    }
}

//#include "dwt_stm32_delay.h"


/**
 * @brief  Initializes DWT_Clock_Cycle_Count for DWT_Delay_us function
 * @return Error DWT counter
 *         1: clock cycle counter not started
 *         0: clock cycle counter works
 */
uint32_t DWT_Delay_Init(void) {
    /* Disable TRC */
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
    /* Enable TRC */
    CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;

    /* Disable clock cycle counter */
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
    /* Enable  clock cycle counter */
    DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;

    /* Reset the clock cycle counter value */
    DWT->CYCCNT = 0;

    /* 3 NO OPERATION instructions */
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");

    /* Check if clock cycle counter has started */
    if(DWT->CYCCNT) {
        return 0; /*clock cycle counter started*/
    } else {
        return 1; /*clock cycle counter not started*/
    }
}

void DWT_Delay_us(volatile uint32_t microseconds) {
    uint32_t clk_cycle_start = DWT->CYCCNT;

    /* Go to number of cycles for system */
    microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);

    /* Delay till end */
    while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
}

void TIM6_delay_us(uint16_t us) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
    TIM6->CR1 = 0;      // stop tim6
    TIM6->EGR = 1;      // event generation register
    //TIM6->ARR = 10000;  // 16 bits auto reload register
    TIM6->PSC = 81;   	// 16bits prescaler
    TIM6->CNT = 0xFFFF - us;     // 16bits counter value
    //TIM6->DIER = 1;     // DMA interrupt enable register
    //TIM6->CR2 = 0;      // control register 2 with master mode selection
    TIM6->SR = 0;       // clear interrupt flag
    TIM6->CR1 = TIM_CR1_CEN | TIM_CR1_OPM;     // start timer
    //TIM6->CR1 = TIM_CR1_URS | TIM_CR1_ARPE | TIM_CR1_CEN;     // start timer

	while((TIM6->SR & TIM_SR_UIF_Msk) == 0) {
	}
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
