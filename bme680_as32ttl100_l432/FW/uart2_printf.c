#include "uart2_printf.h"
#include "../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal.h"
#include "../Drivers/CMSIS/Device/ST/STM32L4xx/Include/stm32l4xx.h"

/* The GCC C library makes calls to the following functions to perform low-level I/O : */
/* int _read(int file, char *data, int len) */
/* int _write(int file, char *data, int len) */
/* int _close(int file) */
/* int _lseek(int file, int ptr, int dir) */
/* int _fstat(int file, struct stat *st) */
/* int _isatty(int file) */

void UART2_Sendchar(uint8_t c) {
    while(!(USART2->ISR & USART_ISR_TXE)); // block until tx empty
    USART2->TDR = c;
}

uint8_t UART2_Getchar(void) {
    uint8_t c;
    while(!(USART2->ISR & USART_ISR_RXNE));  // nothing received so just block
    c = USART2->RDR; // read Receiver buffer register
    return c;
}

void UART2_vprint(const char *fmt, va_list argp) {
    char msg[128] = {0};
    const uint8_t MSG_LEN = sizeof(msg)/sizeof(msg[0]);
    if(0 < vsnprintf(&msg[0],MSG_LEN, fmt,argp)) {
        const uint8_t msg_len = strlen(msg);
        for(uint8_t i=0; i<msg_len; i+=1) {
            UART2_Sendchar(msg[i]);
        }
        // UART2_Sendchar(0x0D);
    }
}

void UART2_printf(const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    UART2_vprint(fmt, argp);
    va_end(argp);
}

// simple alternative
void UART2_print_uint32(uint32_t u32, const char * msg) {
    const uint8_t tbl[] = "0123456789ABCDEF";

    while(msg && *msg) {
        UART2_Sendchar(*msg);
        msg += 1;
    }
    uint8_t d3 = ((u32 >> 24) & 0xFF);
    uint8_t d2 = ((u32 >> 16) & 0xFF);
    uint8_t d1 = ((u32 >> 8) & 0xFF);
    uint8_t d0 = ((u32 >> 0) & 0xFF);

    UART2_Sendchar(tbl[(d3 >> 4) & 0x0F]);
    UART2_Sendchar(tbl[(d3 >> 0) & 0x0F]);
    UART2_Sendchar(tbl[(d2 >> 4) & 0x0F]);
    UART2_Sendchar(tbl[(d2 >> 0) & 0x0F]);
    UART2_Sendchar(tbl[(d1 >> 4) & 0x0F]);
    UART2_Sendchar(tbl[(d1 >> 0) & 0x0F]);
    UART2_Sendchar(tbl[(d0 >> 4) & 0x0F]);
    UART2_Sendchar(tbl[(d0 >> 0) & 0x0F]);

    UART2_Sendchar(0x0D);
}

