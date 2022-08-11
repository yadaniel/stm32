#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "../Core/Inc/rng.h"
#include "main_fw.h"
#include "../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal.h"
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
uint8_t test_crc(void) {
    CRC->INIT = 0xFFFFFFFF;     // seed
    CRC->POL = 0x04C11DB7;      // CCITT32
    CRC->CR = 1;
    for(uint32_t p = 0x08000000; p<0x0803FFFC; p+=4) {
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

    UART_Sendchar(0x0A);
}

void crc_print(void) {
    uint8_t crc_result = test_crc();
    uint8_t msg[] = "crc passed = ";

    for(int8_t idx=0; idx<sizeof(msg)/sizeof(msg[0])-1; idx+=1) {
        UART_Sendchar(msg[idx]);
    }
    UART_Sendchar(crc_result + '0');
    UART_Sendchar(0x0A);

    print_uint32(crc_data.saved, "crc save = ");
    print_uint32(crc_data.calculated, "crc calc = ");
}


/* The GCC C library makes calls to the following functions to perform low-level I/O : */
/* int _read(int file, char *data, int len) */
/* int _write(int file, char *data, int len) */
/* int _close(int file) */
/* int _lseek(int file, int ptr, int dir) */
/* int _fstat(int file, struct stat *st) */
/* int _isatty(int file) */

void vprint(const char *fmt, va_list argp) {
    char msg[128] = {0};
    const uint8_t MSG_LEN = sizeof(msg)/sizeof(msg[0]);
    if(0 < vsnprintf(&msg[0],MSG_LEN, fmt,argp)) {
        const uint8_t msg_len = strlen(msg);
        for(uint8_t i=0; i<msg_len; i+=1) {
            UART_Sendchar(msg[i]);
        }
        // UART_Sendchar(0x0D);
    }
}

void my_printf(const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    vprint(fmt, argp);
    va_end(argp);
}

int main_fw(void) {
    uint32_t r = 0;
    HAL_StatusTypeDef res;

    crc_print();

    while(1) {
        HAL_Delay(250);
        res = HAL_RNG_GenerateRandomNumber(&hrng, &r); 
        my_printf("data = %.2f, foo = 0x%04X, [%d] rand=%4u\n", 0.1234, 0x1234, (res == HAL_OK), (r % 1000));
    }

    // test my_printf
    //my_printf("data = %.2f, foo = 0x%04X", 0.1234, 0x1234);

    // while(1) {
    //     uint8_t c = UART_Getchar();
    //     UART_Sendchar(c);
    //     if(c=='r') {
    //         NVIC_SystemReset();
    //     }
    // }

    return 0;
}

