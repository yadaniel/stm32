#include <stdint.h>
#include "main_fw.h"
#include "fatfs.h"
#include "shell.h"
#include "uart2_printf.h"
#include "../Drivers/CMSIS/Device/ST/STM32L4xx/Include/stm32l432xx.h"

volatile uint8_t rx_buffer[32] = {0};
volatile uint8_t rx_idx = 0;
volatile uint8_t rx_done = 0;

volatile uint8_t tx_buffer[32] = {0, 1, 2, 3, 4, 5, 6, 7};
volatile uint8_t tx_idx = 0;
volatile uint8_t tx_stop = 0;

typedef struct crc_t {
    uint32_t saved;
    uint32_t calculated;
} crc_t;

crc_t crc_data = {.saved = 0x00, .calculated = 0x00};

uint8_t test_crc(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
    CRC->INIT = 0xFFFFFFFF;     // seed
    CRC->POL = 0x04C11DB7;      // CCITT32
    CRC->CR = 1;
    for(uint32_t p = 0x08000000; p < 0x0803FFFC; p += 4) {
        CRC->DR = *((uint32_t *)p);
    }
    uint32_t crc = CRC->DR;
    /* crc ^= 0xFFFFFFFF; */
    /* crc ^= 0x00000000; */
    uint32_t stored_crc = (*(uint32_t *)0x0803FFFC);

    crc_data.saved = stored_crc;
    crc_data.calculated = crc;

    uint8_t result = 0;
    if (crc == stored_crc) {
        result = 1;
    }
    return result;
}

int main_fw(void) {
    // check crc
    UART2_printf("crc passed = %d\r", test_crc());
    UART2_printf("crc save = %lX\r", crc_data.saved);
    UART2_printf("crc calc = %lX\r", crc_data.calculated);

    // pin locked
    UART2_printf("GPIOA: any pin locked = %lX\r", LL_GPIO_IsAnyPinLocked(GPIOA));
    UART2_printf("GPIOB: any pin locked = %lX\r", LL_GPIO_IsAnyPinLocked(GPIOB));

    // set defaults
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
    /* LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3); */

    // usart2
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;     // enable clock
    LL_USART_EnableIT_ERROR(USART2);    // USART2->CR3 |= USART_CR3_EIE;
    LL_USART_EnableIT_RXNE(USART2);     // USART2->CR1 |= USART_CR1_RXNEIE;
    LL_USART_EnableIT_TC(USART2);       // USART2->CR1 |= USART_CR1_TCIE;

    // init fatfs
    MX_FATFS_Init();

    /* FRESULT fres; */
    /* FATFS fs = {0}; */
    /* FIL f = {0}; */
    /* FILINFO fi = {0}; */

    /* fres = f_mount(&fs, "", 1);  // [3]:1 => mount now */
    /* if(fres == FR_NO_FILESYSTEM) { */
    /*     print_uint32(fres, "FRESULT f_mount => FR_NO_FILESYSTEM"); */
    /*     fres = f_mkfs("", FM_ANY, 0, &buffer[0], sizeof(buffer)); */
    /*     if(fres != FR_OK) { */
    /*         print_uint32(fres, "FRESULT f_mkfs failed with => "); */
    /*     } else { */
    /*         fres = f_stat("data.txt", &fi); */
    /*         if(fres != FR_OK) { */
    /*             print_uint32(fres, "FRESULT f_stat failed with => "); */
    /*         } else { */
    /*             fres = f_open(&f, "data.txt", FA_READ); */
    /*             if(fres != FR_OK) { */
    /*                 print_uint32(fres, "FRESULT f_open failed with => "); */
    /*             } else { */
    /*                 char line[64]; */
    /*                 uint8_t cnt = 0; */
    /*                 while(f_gets(&line[0], sizeof(line), &f)) { */
    /*                     print_uint32(cnt, "f_gets line => "); */
    /*                     HAL_Delay(100); */
    /*                     cnt += 1; */
    /*                 } */
    /*             } */
    /*         } */
    /*     } */
    /* } else { */
    /*     fres = f_open(&f, "sensor.dat", FA_CREATE_ALWAYS); */
    /*     if(fres != FR_OK) { */
    /*         print_uint32(fres, "FRESULT f_open failed with => "); */
    /*     } else { */
    /*         print_uint32(fres, "FRESULT f_open succeeded ... "); */
    /*         char line[64] = "sensor data"; */
    /*         fres = f_puts(&line[0], &f); */
    /*         fres = f_close(&f); */
    /*         if(fres != FR_OK) { */
    /*             print_uint32(fres, "FRESULT f_close failed with => "); */
    /*         } else { */
    /*             print_uint32(fres, "FRESULT f_close succeeded ... "); */
    /*         } */
    /*     } */
    /* } */

    /* fres = f_mount(&fs, "", 1); */
    /* if(fres == FR_OK) { */
    /*     fres = f_open(&f, "foo.txt", FA_READ); */
    /*     if(fres != FR_OK) { */
    /*         print_uint32(fres, "FRESULT f_open failed with => "); */
    /*     } else { */
    /*         print_uint32(fres, "FRESULT f_open succeeded ... "); */
    /*         char line[64] = {0}; */
    /*         uint8_t cnt = 0; */
    /*         while(f_gets(&line[0], sizeof(line), &f)) { */
    /*             cnt += 1; */
    /*             print_uint32(cnt, "f_gets line => "); */
    /*             HAL_Delay(100); */
    /*         } */
    /*         fres = f_close(&f); */
    /*         if(fres != FR_OK) { */
    /*             print_uint32(fres, "FRESULT f_close failed with => "); */
    /*         } else { */
    /*             print_uint32(fres, "FRESULT f_close succeeded ... "); */
    /*         } */
    /*     } */
    /* } else { */
    /*     print_uint32(fres, "FRESULT f_mount failed with => "); */
    /* } */

    while(1) {

        shell_run();

        HAL_Delay(100);
        LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);
        HAL_Delay(100);
        LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);

    }
    return 0;
}

