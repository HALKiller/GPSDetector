// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

//  **********************  COMMENT BLOCK  ************************  //



//  **********************  INCLUDES BLOCK  ************************  //

#include "UART.h"

#include "Global.h"

#include "handlers.h"

#include "ring_buffer.h"

#include "checksumming.h"

#include "io_port_sfr_names.h"

#include <stddef.h>

#include "string.h"

#include <stdint.h>

#include "xc.h"


#if DEBUGGING_IS_ON
#include "generic_union_flgs.h"
#endif

// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  
#ifndef FILE_UART_DB_ENABLED
#define FILE_UART_DB_ENABLED 0
#endif

#if FILE_UART_DB_ENABLED
#define DB_PRINT(str) G_DB_PRINT(str)
#define UART_int(var) G_UART_INT(var)
#else
#define DB_PRINT(str)
#define UART_int(var)
#endif
// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  


//  **********************  DATA TYPES, STRUCTS, ENUMS  ************************  //



//  **********************  CONSTANT EXPRESSIONS  ************************  //

// const uint8_t rx_header[] = "$EESL";

// static const uint16_t brg_value[] = {
  
 
  
// };

//  **********************  MACRO DEFINITIONS  ************************  //

#define UART_BTN_SIMUL 1

// #define NULL_TERMINATOR	'\0'
#define TRANSMIT_BUFFER	TXREG
#define TRANSMIT_BUFFER_FULL	PIR1bits.TXIF
#define TRANSMISSION_IN_PROGRESS	!TXSTAbits.TRMT


#define STARTBYTE '$'
#define HEADER_FOUND huart.FLGS.header_found
#define STARTBYTE_FOUND huart.FLGS.startbyte_found
#define NEXT_CHAR_IS_ORDER_BYTE huart.FLGS.first_char_after_header

// ret_values from check on rx_f_pc
#define RX_PC_BAD_CHCKSUM 1




//  **********************  STATIC DATA DECLARATIONS  ************************  //

// uint8_t tx_data[CHARS_TO_RECEIVE + 2];	// 1 orderbyte + 34 data + 1 chcksum + 1 NULL_Terminator



//  **********************  PRIVATE FUNCTIONS PROTOTYPES  ************************  //

static void check_on_startbyte(uint8_t the_data);

static uint8_t received_new_header(void);

static void reset_header_reception(void);

static void reset_data_reception(void);

static void transmit_char(uint8_t n_char);

static uint8_t chcksum_checker(void);

static void int_to_str_converter_32bits(uint32_t the_value, unsigned char *str_pnt);

static void uint32_to_str(uint32_t num, char *str);

static void int32_to_str(int32_t num, char *str);
  
static void int_to_str_converter(uint16_t the_value, char *str_pnt);

static void prepare_tx_handler(void);





//  **********************  PUBLIC FUNCTIONS BODY  ************************  //



#if SLOW_CLCK==LOW_31_25KHZ

void uart_init_slow_clock(void){
  
    // 1953 BAUD
  BRG16 = 1u;
  BRGH = 1u;
  SPBRG = 3u;
  SYNC = FALSE;
  SPEN = TRUE;
  CREN = TRUE;
  TXEN = TRUE;
#if DEBUG_16F1936_GPS_FORCE_RCIE	
  RCIE = TRUE;
#endif

  
}

#elif SLOW_CLCK==LOW_500KHZ

void uart_init_slow_clock(void){
  
    // 9600 BAUD
  BRG16 = 1u;
  BRGH = 1u;
  SPBRG = 12u;
  SYNC = FALSE;
  SPEN = TRUE;
  CREN = TRUE;
  TXEN = TRUE;
#if DEBUG_16F1936_GPS_FORCE_RCIE	
  RCIE = TRUE;
#endif


  
}

#else
  
err here...

#endif

#if CALCULATE_BAUDRATE

// TODO: before a change of baudrate happens assure that the last msg got allready transmitted!
// 283 words
void uart_init_cfg(baudrate_t baudrate){
  
  uint32_t brg_value = MIPS * 10000;
  
  brg_value = brg_value / baudrate;
  
  brg_value--;
  
  SYNC = FALSE;
  
  BRG16 = 1u;
  BRGH  = 1u;

  if(baudrate == B9600_low_clk)
  {
    SPBRGH = SPBRGH_9600_LCKL_VAL;
    SPBRG = SPBRGL_9600_LCKL_VAL; 
  }
  else
  {
    SPBRGH = (uint8_t)((brg_value >> 8) & 0x00FF);
    SPBRG = (uint8_t)(brg_value & 0x00FF);    
  }

 
#if DEBUGGING_IS_ON

	RCSTAbits.SPEN = TRUE;
	
	RCSTAbits.CREN = TRUE;
	TXSTAbits.TXEN = TRUE;

#endif
  
  
  __delay_ms(100);
  
#if DEBUGGING_IS_ON&&0
  DB_PRINT("\r\nBRGH: ");
  UART_int(SPBRGH);
  DB_PRINT("\r\nBRG: ");
  UART_int(SPBRG);
  UART_CRLF;
#endif
  
  
}

#else
  
void uart_init_cfg(baudrate_t baudrate){
  
  
  SYNC = FALSE;
  
  BRG16 = 1u;
  BRGH  = 1u;
  
  send_bb_string("\r\nUART_CFG ");
  
  switch(baudrate)
  {
    
    
    case B9600:
      DB_PRINT("U96");
      SPBRGH = SPBRGH_9600_VAL;
      SPBRG = SPBRGL_9600_VAL;
    break;
    case B57600:
      DB_PRINT("U57");
      SPBRGH = SPBRGH_57600_VAL;
      SPBRG = SPBRGL_57600_VAL;
    break;
    case B115200:
      DB_PRINT("U115");
      SPBRGH = SPBRGH_115200_VAL;
      SPBRG = SPBRGL_115200_VAL;    
    break;    
    case B9600_low_clk:
      SPBRGH = SPBRGH_9600_LCKL_VAL;
      SPBRG = SPBRGL_9600_LCKL_VAL;    
    break;
    
  }
  
 send_bb_string("\r\n");
 
#if 0

	RCSTAbits.SPEN = TRUE;
	
	RCSTAbits.CREN = TRUE;
  
	TXSTAbits.TXEN = TRUE;

#endif

  __delay_ms(100);
  
#if DEBUGGING_IS_ON&&0

  DB_PRINT("\r\nBRGH: ");
  UART_int(SPBRGH);
  DB_PRINT("\r\nBRG: ");
  UART_int(SPBRG);
  UART_CRLF;
  
#endif



}


#endif


void UART_on(void){
  
  RCSTAbits.SPEN = TRUE;
	
	RCSTAbits.CREN = TRUE;
  
	TXSTAbits.TXEN = TRUE;
  
}


void UART_off(void){
  
  RCSTAbits.SPEN = FALSE;
	
	RCSTAbits.CREN = FALSE;
  
	TXSTAbits.TXEN = FALSE;
  
}


#if 1  

void UART_INT_C(uint32_t hvar){

  char str[12];

  int32_to_str((int32_t)hvar, str);
  // uint32_to_str(hvar, str);
  
  DB_PRINT(&str[0]);
  // DB_PRINT(&str[0]);
	
}

#else
  
void UART_INT_C(uint32_t hvar){


  char str[11];

  uint32_to_str(hvar, str);

  DB_PRINT(&str[0]);
	
}

#endif

static void int32_to_str(int32_t num, char *str) {
  
  // Create a buffer to hold the digits in reverse order
  char temp[12];  // int32_t min is -2147483648, so 10 digits + sign + null terminator
  int i = 0;
  int is_negative = 0;
  
  // Handle negative numbers
  if (num < 0) 
  {
    is_negative = 1;
    // Handle the special case of INT32_MIN (-2147483648)
    // We can't just negate it because abs(INT32_MIN) > INT32_MAX
    if (num == INT32_MIN) 
    {
      strcpy(str, "-2147483648");
      return;
    }
    num = -num;
  }
  
  // Special case for zero
  if (num == 0) 
  {
    str[i++] = '0';
    str[i] = '\0';
    return;
  }
  
  // Extract digits from the number and put them into the buffer in reverse order
  while (num != 0) 
  {
    temp[i++] = (num % 10) + '0';
    num /= 10;
  }
  
  // Add negative sign if needed
  if (is_negative) 
  {
    temp[i++] = '-';
  }
  
  // Reverse the digits and put them into the output string
  int j = 0;
  while (i > 0) 
  {
    str[j++] = temp[--i];
  }
  
  // Null terminate the result string
  str[j] = '\0';
}  
  
  




static void uint32_to_str(uint32_t num, char *str) {
  
  // Create a buffer to hold the digits in reverse order
  char temp[11];  // uint32_t max is 4294967295, so 10 digits + null terminator
  int i = 0;

  // Special case for zero
  if (num == 0) 
  {
    str[i++] = '0';
    str[i] = '\0';
    return;
  }

  // Extract digits from the number and put them into the buffer in reverse order
  while (num != 0) 
  {
    temp[i++] = (num % 10) + '0';
    num /= 10;
  }

  // Reverse the digits and put them into the output string
  int j = 0;
  while (i > 0) 
  {
    str[j++] = temp[--i];
  }

  // Null terminate the result string
  str[j] = '\0';
  
}


#if 1

// these uart converters can be usefull

static char nibbleToHexChar(unsigned char nibble) {
  if (nibble < 10)
  {
    return '0' + nibble;
  } 
  else 
  {
    return 'A' + (nibble - 10);
  }
}


// Function to convert a byte to a hexadecimal string
static void byteToHexString(unsigned char byte, char *hexString) {
  // Extract the high nibble and convert it to a hex character
  hexString[0] = nibbleToHexChar((byte >> 4) & 0x0F);
  // Extract the low nibble and convert it to a hex character
  hexString[1] = nibbleToHexChar(byte & 0x0F);
  // Null-terminate the string
  hexString[2] = '\0';
}


void uart_hex(uint8_t hvar){
  
  unsigned char str[5];

  str[0] = '0';
  str[1] = 'x';
  
  byteToHexString(hvar, &str[2]);

  DB_PRINT(str);



}

#endif



void send_string(const unsigned char *str_pnt){

  const uint8_t const_max_length = 128;
  uint8_t sent_char_cnt = 0;
  unsigned char *local_pnt = str_pnt;
  
	while((*local_pnt != NULL_TERMINATOR) && (sent_char_cnt < const_max_length))
	{
		
		transmit_char(*local_pnt);
		local_pnt++;
#if ENFORCE_MISRA_RULE    
    sent_char_cnt = (uint8_t)((uint16_t)sent_char_cnt + 1u);
#else    
    sent_char_cnt++;
#endif  
	}	
	
  
  
#if DEBUGGING_BB_IS_ON&&0
  send_bb_string(str_pnt);
#endif
  
  

}


void send_character(uint8_t the_char){
	
	
	transmit_char(the_char);
	
	
}


#if 0
inline void uart_swoff_reception(void){
	
	RCSTAbits.CREN = false;
	
}
#endif
// PRIVATE functions *******************


static void transmit_char(uint8_t n_char){
	
	
	while(!TRANSMIT_BUFFER_FULL);
	
	TRANSMIT_BUFFER = n_char;
	
	
}




