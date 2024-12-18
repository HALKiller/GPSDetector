#ifndef BIT_BANGED_UART_H
#define BIT_BANGED_UART_H

#include <stdint.h>


#define NORMAL_CLOCK 1
#define SLOW_CLOCK 0

void init_TMR_bitbang_uart(uint8_t clockspeed);

void send_bb_string(const unsigned char *str_pnt);





































#endif //  INT_TO_FLOAT_STRING_H