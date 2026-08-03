#ifndef UART_H
#define UART_H

#include "Global.h"

#include <stdint.h>
#include <stddef.h>


// typedef enum baudtype{
  
  // BAUD_9600 = 96,
  // BAUD_57600 = 576,
  // BAUD_115200 = 1152,
  // BAUD_1953,
  
// }baudtype_t;



#if MIPS == 1

#define SPBRGH_115200_VAL 0u
#define SPBRGL_115200_VAL 8u

#define SPBRGH_57600_VAL 0u
#define SPBRGL_57600_VAL 16u

#define SPBRGH_9600_VAL 0u
#define SPBRGL_9600_VAL 103u

#elif MIPS==2

#define SPBRGH_115200_VAL 0u
#define SPBRGL_115200_VAL 16u

#define SPBRGH_57600_VAL 0u
#define SPBRGL_57600_VAL 34u

#define SPBRGH_9600_VAL 0u
#define SPBRGL_9600_VAL 207u

#elif MIPS==4

#define SPBRGH_115200_VAL 0u
#define SPBRGL_115200_VAL 34u

#define SPBRGH_57600_VAL 0u
#define SPBRGL_57600_VAL 68u

#define SPBRGH_9600_VAL 1u
#define SPBRGL_9600_VAL 160u

#elif MIPS==8

#define SPBRGH_115200_VAL 0u
#define SPBRGL_115200_VAL 68

#define SPBRGH_57600_VAL 0u
#define SPBRGL_57600_VAL 138

#define SPBRGH_9600_VAL 3u
#define SPBRGL_9600_VAL 64u

#else	
	


#endif

#define SPBRGH_9600_LCKL_VAL 0u
#define SPBRGL_9600_LCKL_VAL 12u


#define CR	13
#define LF	10
#define SPACE 32
#define TAB 9

#define UART_GPS_SEND(str) send_string((const unsigned char *)(str))

#define UART_CRLF DB_PRINT("\r\n");


#define USE_TX_ISR 0


#define USE_THE_GENERIC 0

#define DEBUG_BAUDRATE B9600
typedef enum {
  
  B9600 = 96,
  B57600 = 576,
  B115200 = 1152,
  B9600_low_clk,
  
}baudrate_t;


void uart_init_cfg(baudrate_t baudrate);

// uint8_t check_next_char(void);

// void init_uart_flags(void);

void uart_init_slow_clock(void);

// void init_UART(void);

// inline void uart_swoff_reception(void);


void send_string(const unsigned char *str_pnt);



void UART_INT_C(uint32_t hvar);

void uart_hex(uint8_t hvar);

void UART_32_int(uint32_t hvar);

void send_character(uint8_t the_char);

// uint8_t *get_pnt_to_uart_rx_buffer(void);

void UART_off(void);

void UART_on(void);























#endif	// INTENSE_RESISTOR_H