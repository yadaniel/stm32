#include <stdint.h>
#include "main_fw.h"
#include "fatfs.h"
#include "../Drivers/CMSIS/Device/ST/STM32L4xx/Include/stm32l432xx.h"
/* #include "../Drivers/CMSIS/Device/ST/STM32L4xx/Include/stm32l3xx.h" */

void UART_Sendchar(uint8_t c) {
    while(!(USART2->ISR & USART_ISR_TXE)); // block until tx empty
    USART2->TDR = c;
}

uint8_t UART_Getchar() {
    uint8_t c;
    while(!(USART2->ISR & USART_ISR_RXNE));  // nothing received so just block
    c = USART2->RDR; // read Receiver buffer register
    return c;
}

typedef struct crc_t {
    uint32_t saved;
    uint32_t calculated;
} crc_t;

crc_t crc_data = {.saved = 0x00, .calculated = 0x00};

// d = ba.a2b_hex("00112233445566778899AABBCCDDEEFF0123456789ABCDEF")
// c = crc.crc.Crc32Bzip2()
// hex(c.process(d).final() ^ 0xFFFFFFFF) # => '0xc4e0c81'
// python result is XORed with 0xFFFFFFFF
// stm32 below is not XORed but has same result
uint8_t test_crc_(void) {
    uint32_t buffer[6] = {0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF, 0x01234567, 0x89ABCDEF};
    CRC->INIT = 0xFFFFFFFF;     // seed
    CRC->POL = 0x04C11DB7;      // CCITT32
    CRC->CR = 1;
    /* CRC->CR = 33; */
    /* CRC->CR = 1 + 32 + 64; */
    for(uint8_t i = 0; i < sizeof(buffer) / sizeof(buffer[0]); i += 1) {
        CRC->DR = buffer[i];
    }
    uint32_t crc = CRC->DR;
    /* crc ^= 0xFFFFFFFF; */
    /* crc ^= 0x00000000; */

    uint32_t stored_crc = 0xAABBCCDD;

    crc_data.saved = stored_crc;
    crc_data.calculated = crc;

    uint8_t result = 0;
    if (crc == stored_crc) {
        result = 1;
    }
    return result;
}

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

void print_uint32(uint32_t u32, const char * msg) {
    const uint8_t tbl[] = "0123456789ABCDEF";

    while(msg && *msg) {
        UART_Sendchar(*msg);
        msg += 1;
    }
    uint8_t d3 = ((u32 >> 24) & 0xFF);
    uint8_t d2 = ((u32 >> 16) & 0xFF);
    uint8_t d1 = ((u32 >> 8) & 0xFF);
    uint8_t d0 = ((u32 >> 0) & 0xFF);

    UART_Sendchar(tbl[(d3 >> 4) & 0x0F]);
    UART_Sendchar(tbl[(d3 >> 0) & 0x0F]);
    UART_Sendchar(tbl[(d2 >> 4) & 0x0F]);
    UART_Sendchar(tbl[(d2 >> 0) & 0x0F]);
    UART_Sendchar(tbl[(d1 >> 4) & 0x0F]);
    UART_Sendchar(tbl[(d1 >> 0) & 0x0F]);
    UART_Sendchar(tbl[(d0 >> 4) & 0x0F]);
    UART_Sendchar(tbl[(d0 >> 0) & 0x0F]);

    UART_Sendchar(0x0D);
}

int main_fw(void) {
    /* uint8_t crc_result = test_crc_(); */
    uint8_t crc_result = test_crc();
    uint8_t msg[] = "crc passed = ";

    for(uint8_t idx = 0; idx < sizeof(msg) / sizeof(msg[0]) - 1; idx += 1) {
        UART_Sendchar(msg[idx]);
    }
    UART_Sendchar(crc_result + '0');
    UART_Sendchar(0x0D);

    print_uint32(0xAABBCCDD, "assert 0xAABBCCDD = ");
    print_uint32(crc_data.saved, "crc save = ");
    print_uint32(crc_data.calculated, "crc calc = ");

    uint32_t locked = LL_GPIO_IsAnyPinLocked(GPIOA);
    print_uint32(locked, "GPIOA any pin locked ");
    locked = LL_GPIO_IsAnyPinLocked(GPIOB);
    print_uint32(locked, "GPIOB any pin locked ");

    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
    /* LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3); */

    MX_FATFS_Init();
    FRESULT res;

    //FATFS *ptr_fs = malloc(sizeof(FATFS));
    //res = f_mount(&ptr_fs, "spi_drive", 0);
    
    FATFS fs = {0};
    FIL f = {0};
    uint8_t buffer[1024] = {};

    res = f_mount(&fs, "spi_drive", 0);
    if(res != FR_OK) {
        print_uint32(res, "FRESULT f_mount failed with => ");
    } else {
        print_uint32(res, "FRESULT f_mount succeeded ... ");

        res = f_open(&f, "data.dat", FA_WRITE);
        if(res != FR_OK) {
            print_uint32(res, "FRESULT f_open failed with => ");

            res = f_mkfs("spi_drive", FM_FAT, 512, &buffer[0], 512);
            if(res != FR_OK) {
                print_uint32(res, "FRESULT f_mkfs failed with => ");
            }
            else {
                print_uint32(res, "FRESULT f_mkfs succeeded ... ");

                res = f_open(&f, "data.dat", FA_WRITE);
                if(res != FR_OK) {
                    print_uint32(res, "FRESULT f_open failed with => ");
                } else {
                    print_uint32(res, "FRESULT f_open succeeded ... ");
                }
            }
        } else {
            print_uint32(res, "FRESULT f_open succeeded ... ");
        }
    }

    while(1) {

        /* uint8_t c = UART_Getchar(); */
        /* UART_Sendchar(c); */
        /* if(c == 'r') { */
        /*     NVIC_SystemReset(); */
        /* } */

        /* print_uint32(HAL_GetTick(), "HAL_GetTick() => "); */

        HAL_Delay(1000);
        LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);
        HAL_Delay(1000);
        LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);

    }
    return 0;
}

