// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

//  **********************  COMMENT BLOCK  ************************  //



//  **********************  INCLUDES BLOCK  ************************  //

#include "UART.h"
#include "Global.h"
#include "handlers.h"
#include "ring_buffer.h"
#include "checksumming.h"
#include "int2string.h"

#include "io_port_sfr_names.h"

#include <stddef.h>

#include "string.h"
#include <stdint.h>
#include "xc.h"

#if DEBUGGING_IS_ON
#include "generic_union_flgs.h"
#endif



//  **********************  DATA TYPES, STRUCTS, ENUMS  ************************  //
#define HEADER_LENGTH 5

#define LOCAL_DB_UART 0

#if 0
struct udt_uart{
	union
  {
		uint8_t reg;
		struct
    {
			unsigned startbyte_found : 1;
			unsigned header_found	: 1;
			unsigned first_char_after_header : 1;
			unsigned free	: 5;
		};
	}FLGS;
	
	uint8_t data_lencnt;
	uint8_t *data_pnt;
	uint8_t header_lencnt;
	uint8_t *header_pnt;
	uint8_t header[HEADER_LENGTH + 1];
}huart;

#endif

//  **********************  CONSTANT EXPRESSIONS  ************************  //

const uint8_t rx_header[] = "$EESL";

static const uint16_t brg_value[] = {
  
  
  
};

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
  
  
  switch(baudrate)
  {
    
    
    case B9600:
      SPBRGH = SPBRGH_9600_VAL;
      SPBRG = SPBRGL_9600_VAL;
    break;
    case B57600:
      SPBRGH = SPBRGH_57600_VAL;
      SPBRG = SPBRGL_57600_VAL;
    break;
    case B115200:
      SPBRGH = SPBRGH_115200_VAL;
      SPBRG = SPBRGL_115200_VAL;    
    break;    
    case B9600_low_clk:
      SPBRGH = SPBRGH_9600_LCKL_VAL;
      SPBRG = SPBRGL_9600_LCKL_VAL;    
    break;
    
  }
  
 
 
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



#if 0

#if MIPS==1

void init_UART(void)
{
  // 57600
  BRG16 = 1u;
  BRGH = 1u;
  SPBRG = 16u;
  SYNC = 0u;
  SPEN = TRUE;
  CREN = TRUE;
  TXEN = TRUE;
#if DEBUG_16F1936_GPS_FORCE_RCIE	
  RCIE = TRUE;
#endif

}

#elif MIPS==8

void init_UART(void){
	
	// Baudrate = 57600
	RCSTAbits.SPEN = false;

	BAUDCONbits.BRG16 = true;	// false; 
	TXSTAbits.BRGH = true;

	SPBRGH = 0u;
	SPBRG = 138u;	// 51;

	// BAUD1CONbits.SCKP = true;
	TXSTAbits.SYNC = FALSE;
	
	RCSTAbits.SPEN = TRUE;
	
	RCSTAbits.CREN = TRUE;
	TXSTAbits.TXEN = TRUE;
	

	
}


#elif MIPS==4

	// Baudrate = 57600
	RCSTAbits.SPEN = false;

	BAUDCONbits.BRG16 = true;	// false; 
	TXSTAbits.BRGH = true;

	SPBRGH = 0u;
	SPBRG = 138u;	// 51;

	
	TXSTAbits.SYNC = false;
	// BAUD1CONbits.SCKP = true;
	RCSTAbits.SPEN = true;
	
	RCSTAbits.CREN = true;
	TXSTAbits.TXEN = true;

}

#elif 0
void init_UART(void)
{
  // 115200
  BRG16 = 1;
  BRGH = 1;
  SPBRG = 8;
  SYNC = 0;
  SPEN = 1;
  CREN = 1;
  TXEN = 1;
#if DEBUG_16F1936_GPS_FORCE_RCIE	
  RCIE = 1;
#endif

}

#elif 0 
// that works for 4800 BAUD
void init_UART(void)
{
  BRG16 = 0;
  BRGH = 0;
  SPBRG = 12;
  SYNC = 0;
  SPEN = 1;
  CREN = 1;
  TXEN = 1;
#if DEBUG_16F1936_GPS_FORCE_RCIE	
  RCIE = 1;
#endif

}

#else


to be confirmed

void init_UART(void){
	
	
	RC1STAbits.SPEN = false;

	BAUD1CONbits.BRG16 = true;	// false; 
	TX1STAbits.BRGH = false;

	SP1BRGH = 0;
	SP1BRG = 51;	// 51;

	
	TX1STAbits.SYNC = false;
	// BAUD1CONbits.SCKP = true;
	RC1STAbits.SPEN = true;
	
	RC1STAbits.CREN = true;
	TX1STAbits.TXEN = true;
	

	
}

	
#endif

#endif// IF 0 becaue no more uart_iniot stuff here---



#if USE_THE_GENERIC


// UART function with void* and size
void UART_ui2s(void* hvar, size_t size){
  
  char str[11];  // Buffer for the string representation




  uint32_t num = 0;  // Store the final number in uint32_t to handle all cases

  // Handle based on the size of the input
  if (size == sizeof(uint8_t)) 
  {
    num = *(uint8_t*)hvar;  // Dereference as uint8_t
  } 
  else if (size == sizeof(uint16_t)) 
  {
    num = *(uint16_t*)hvar; // Dereference as uint16_t
  } 
  else if (size == sizeof(uint32_t)) 
  {
    num = *(uint32_t*)hvar; // Dereference as uint32_t
  } 
  else 
  {
    // If size is unsupported, just return an empty string or handle error
    str[0] = NULL_TERMINATOR; // '\0';
    return;
  }


  UINT_TO_STR(num, str);

  // Parse the void* to string based on size
  // uint_to_str(hvar, size, str);

  // Send the string via UART (simulated here)
  DB_PRINT(str);
  
}

#elif 0

void UART_int(uint16_t hvar){

  char str[11];

  uint16_t parser = hvar;

  uint_to_str(parser, str);
	
  DB_PRINT(&str[0]);
	
}

#elif 0


void UART_int(uint16_t hvar){

DB_PRINT("Uart_int\r\n");
	
}



#else

void UART_int(uint16_t hvar){

  char str[11];

  // uint_to_str(hvar, str);
#if 1
  uint32_to_str(hvar, str);
#else
  int_to_str_converter(hvar, str);
#endif
	

  
  DB_PRINT(&str[0]);
	
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

#if 0
static void int_to_str_converter(uint16_t the_value, char *str_pnt){
	
union {
	uint32_t bcd;	// uint24_t bcd;
	struct
	{
		uint8_t byte_0;
		uint8_t byte_1;
		uint8_t byte_2;
		uint8_t byte_3;
    uint8_t byte_4;
	};

	struct 
	{
		unsigned ones					: 4;
		unsigned tens					: 4;
		unsigned hundreds			: 4;
		unsigned thousands		: 4;
		unsigned t_thousands	: 4;
		unsigned h_thousands	: 4;
    unsigned million			: 4;
		unsigned t_million		: 4;
		unsigned h_million  	: 4;
		unsigned billion    	: 4;
	};
}result;

unsigned char *bcd_pnt = &result.byte_0;

unsigned char hlooper = 0;
unsigned char hlooper_2 = 0;
unsigned char not_zero_flg = 0;

char temp_string[11];
char *temp_strpnt = &temp_string[10];

	*temp_strpnt = NULL_TERMINATOR;
	temp_strpnt--;
		
		
		
	result.bcd = 0;

	for(hlooper = 0; hlooper < 3; hlooper++)
	{
		result.bcd = result.bcd << 1;
		if(the_value >=32768)
		{
			result.bcd = result.bcd + 1;
		}
    
		the_value = (the_value << 1) & 0xFFFF;
	}

	for(hlooper = 0; hlooper < 29; hlooper++)
	{
	
		bcd_pnt = &result.byte_0;
		for(hlooper_2 = 0; hlooper_2 < 5; hlooper_2++)
		{

			if((0x0F & *bcd_pnt) >= 5)
			{
				*bcd_pnt = *bcd_pnt + 3;		
			}
			if((0xF0 & *bcd_pnt) >= 80)
			{
				*bcd_pnt = *bcd_pnt + 48;
			}
			bcd_pnt++;
		}
		
		result.bcd = result.bcd << 1;
		if(the_value >=32768)
		{
			result.bcd = result.bcd + 1;
		}
		the_value = (the_value << 1) & 0xFFFF;;
				
	}
	
	#if 1	// because in the end was this the fastes...
	*str_pnt = result.h_thousands + 0x30;
	if(*str_pnt != 0x30){
		not_zero_flg = true;
		str_pnt++;
	}
	
	*str_pnt = result.t_thousands+ 0x30;
	if((*str_pnt != 0x30) || not_zero_flg){
		not_zero_flg = true;
		str_pnt++;;
	}
	
	*str_pnt = result.thousands+ 0x30;
	if((*str_pnt != 0x30) || not_zero_flg){
		not_zero_flg = true;
		str_pnt++;;
	}

	*str_pnt = result.hundreds+ 0x30;	
	if((*str_pnt != 0x30) || not_zero_flg){
		not_zero_flg = true;
		str_pnt++;;
	}

	*str_pnt = result.tens+ 0x30;	
	if((*str_pnt != 0x30) || not_zero_flg){
		not_zero_flg = true;
		str_pnt++;;
	}
	*str_pnt = result.ones+ 0x30;	
	str_pnt++;
	*str_pnt = NULL_TERMINATOR;	
	
#elif 1
	

	for(hlooper = 0; hlooper < 6; hlooper++){
		bcd_pnt = (&result.byte_0 + (hlooper/2));
		
		if((hlooper % 2) == 0){	// we are with the low nibble...
			*temp_strpnt = (*bcd_pnt & 0X0F) + 0x30;;
		}else{	// we are with the high nibble
			*temp_strpnt = ((*bcd_pnt & 0XF0) >> 4) + 0x30;;
		}
		temp_strpnt--;

	
	}
	temp_strpnt++;
	hlooper = 0;
	while(hlooper < 6 && temp_strpnt != '\0'){
		hlooper++;
		if(not_zero_flg){
			*str_pnt = *temp_strpnt;
			str_pnt++;
			// temp_strpnt++;
		}else{
			if(*temp_strpnt != '0'){
				not_zero_flg = true;
				*str_pnt = *temp_strpnt;
				str_pnt++;
			}
			// temp_strpnt++;
		}
		temp_strpnt++;
		
	}
	if(hlooper == 0){
		*str_pnt == '0';
		str_pnt++;
		
	}
	*str_pnt = NULL_TERMINATOR;
	
	#endif
}

#endif

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




inline void uart_swoff_reception(void){
	
	RCSTAbits.CREN = false;
	
}

// PRIVATE functions *******************


static void transmit_char(uint8_t n_char){
	
	
	while(!TRANSMIT_BUFFER_FULL);
	
	TRANSMIT_BUFFER = n_char;
	
	
}








#undef LOCAL_DB_UART














