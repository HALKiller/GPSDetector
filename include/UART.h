#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stddef.h>

#define CR	13
#define LF	10
#define SPACE 32
#define TAB 9

#define UWF always_send_string

#if DEBUGGING_IS_ON&&0
#define UWT(str) send_string((const unsigned char *)(str))
#else
#define UWT(str)  
#endif

// #define UWT send_string
#define UART_CRLF UWT("\r\n");
// #define UART_CRLF UWT((const unsigned char *)"\r\n");

#define USE_TX_ISR 0


#define USE_THE_GENERIC 0

#if USE_THE_GENERIC&&0

void uint8_to_str(uint8_t num, char *str);
void uint16_to_str(uint16_t num, char *str);
void uint32_to_str(uint32_t num, char *str);



#define uint_to_str(num, str) _Generic((num),  \
    uint8_t: uint8_to_str,                    \
    uint16_t: uint16_to_str,                  \
    uint32_t: uint32_to_str                   \
)(num, str)

#endif


uint8_t check_next_char(void);

void init_uart_flags(void);

void uart_init_slow_clock(void);

void init_UART(void);

inline void uart_swoff_reception(void);


void send_string(const unsigned char *str_pnt);


// void always_send_string(const unsigned char *str_pnt);
#if USE_THE_GENERIC
#define UART_int(var) UART_ui2s(&var, sizeof(var))
void UART_ui2s(void* hvar, size_t size);
#else
void UART_int(uint16_t hvar);
#endif
void UART_32_int(uint32_t hvar);


void send_character(uint8_t the_char);



uint8_t *get_pnt_to_uart_rx_buffer(void);

























#endif	// INTENSE_RESISTOR_H