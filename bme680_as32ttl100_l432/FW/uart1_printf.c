#include "uart1_printf.h"
#include "../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal.h"
#include "../Drivers/CMSIS/Device/ST/STM32L4xx/Include/stm32l4xx.h"

/* The GCC C library makes calls to the following functions to perform low-level I/O : */
/* int _read(int file, char *data, int len) */
/* int _write(int file, char *data, int len) */
/* int _close(int file) */
/* int _lseek(int file, int ptr, int dir) */
/* int _fstat(int file, struct stat *st) */
/* int _isatty(int file) */

void UART1_Sendchar(uint8_t c) {
    while(!(USART1->ISR & USART_ISR_TXE)); // block until tx empty
    USART1->TDR = c;
}

uint8_t UART1_Getchar(void) {
    uint8_t c;
    while(!(USART1->ISR & USART_ISR_RXNE));  // nothing received so just block
    c = USART1->RDR; // read Receiver buffer register
    return c;
}

void UART1_vprint(const char *fmt, va_list argp) {
    char msg[128] = {0};
    const uint8_t MSG_LEN = sizeof(msg)/sizeof(msg[0]);
    if(0 < vsnprintf(&msg[0],MSG_LEN, fmt,argp)) {
        const uint8_t msg_len = strlen(msg);
        for(uint8_t i=0; i<msg_len; i+=1) {
            UART1_Sendchar(msg[i]);
        }
        // UART1_Sendchar(0x0D);
    }
}

void UART1_printf(const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    UART1_vprint(fmt, argp);
    va_end(argp);
}

