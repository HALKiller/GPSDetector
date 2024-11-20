#ifndef UART_H
#define UART_H

#include "Global.h"

#include <stdint.h>
#include <stddef.h>


typedef enum baudtype{
  
  BAUD_9600 = 96,
  BAUD_57600 = 576,
  BAUD_115200 = 1152,
  BAUD_1953,
  
}baudtype_t;



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

#define UWF always_send_string

#define UART_GPS_SEND(str) send_string((const unsigned char *)(str))



#if DEBUGGING_IS_ON
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


typedef enum {
  
  B9600 = 96,
  B57600 = 576,
  B115200 =1152,
  B9600_low_clk,
  
}baudrate_t;


void uart_init_cfg(baudrate_t baudrate);

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