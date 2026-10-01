#ifndef BIT_BANGED_UART_H
#define BIT_BANGED_UART_H

#include <stdint.h>

// Set to 0 for the portable C transmitter, 1 for PIC16F1936/XC8 1.x assembly.
// Can also be overridden with a compiler -D option.
#ifndef BB_UART_USE_ASM
#if defined(XC8_V_138) && defined(PIC_16F1936)
#define BB_UART_USE_ASM 1
#else
#define BB_UART_USE_ASM 0
#endif
#endif

#if BB_UART_USE_ASM && (!defined(XC8_V_138) || !defined(PIC_16F1936))
#error BB_UART_USE_ASM requires the PIC16F1936 XC8 1.38 configuration
#endif


#define NORMAL_CLOCK 1
#define SLOW_CLOCK 0

void init_TMR_bitbang_uart(uint8_t clockspeed);

void send_bb_string(const unsigned char *str_pnt);





































#endif //  INT_TO_FLOAT_STRING_H
