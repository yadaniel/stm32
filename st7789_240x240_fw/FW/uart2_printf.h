#ifndef UART2_PRINTF_INCLUDED
#define UART2_PRINTF_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

uint8_t UART2_Getchar(void);
void UART2_Sendchar(uint8_t c);
void UART2_printf(const char *fmt, ...);
void UART2_vprint(const char *fmt, va_list argp);

// simple alternative
void UART2_print_uint32(uint32_t u32, const char * msg);

#endif  // UART2_PRINTF_INCLUDED

