#ifndef UART1_PRINTF_INCLUDED
#define UART1_PRINTF_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

uint8_t UART1_Getchar(void);
void UART1_Sendchar(uint8_t c);
void UART1_printf(const char *fmt, ...);
void UART1_vprint(const char *fmt, va_list argp);

#endif  // UART1_PRINTF_INCLUDED

