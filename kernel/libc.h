#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#define UART        0x10000000
#define UART_THR    (uint8_t*)(UART+0x00) // THR:transmitter holding register
#define UART_LSR    (uint8_t*)(UART+0x05) // LSR:line status register
#define UART_LSR_EMPTY_MASK 0x40          // LSR Bit 6: Transmitter empty; both the THR and LSR are empty

inline int lib_putc(char ch) {
    while ((*UART_LSR & UART_LSR_EMPTY_MASK) == 0);
    return *UART_THR = ch;
}

inline void lib_puts(char *s) {
    while (*s) lib_putc(*s++);
}

int lib_vsnprintf(char * out, size_t n, const char* s, va_list vl);

int lib_vprintf(const char* s, va_list vl);

int lib_printf(const char* s, ...);

inline void delay(volatile int count)
{
    count *= 50000;
    while(count--);
}