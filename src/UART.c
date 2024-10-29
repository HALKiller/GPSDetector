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


//  **********************  CONSTANT EXPRESSIONS  ************************  //

const uint8_t rx_header[] = "$EESL";



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

uint8_t tx_data[CHARS_TO_RECEIVE + 2];	// 1 orderbyte + 34 data + 1 chcksum + 1 NULL_Terminator



//  **********************  PRIVATE FUNCTIONS PROTOTYPES  ************************  //

static void check_on_startbyte(uint8_t the_data);

static uint8_t received_new_header(void);

static void reset_header_reception(void);

static void reset_data_reception(void);

static void transmit_char(uint8_t n_char);

static uint8_t chcksum_checker(void);

static void int_to_str_converter_32bits(uint32_t the_value, unsigned char *str_pnt);

static void int_to_str_converter(uint16_t the_value, unsigned char *str_pnt);

static void prepare_tx_handler(void);





//  **********************  PUBLIC FUNCTIONS BODY  ************************  //

void init_uart_flags(void){
	
	
	huart.FLGS.reg = 0u;
	huart.data_lencnt = 0u;
	huart.header_lencnt = 0u;
	huart.header_pnt = &(huart.header[0]);
	huart.data_pnt = &tx_data[0];
	huart.header[5] = NULL_TERMINATOR;
	
}

#if 0
// this one functions now in the way that we parse into it the 
// baudrate and the CLockspeed --> therefroe it calcultaes by
// itself the necessary settings
void init_UART_calculated(uint32_t baudrate){
  
  uint32_t clck = MIPS * 4000000u;
  

  
}

#endif


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

	
	TXSTAbits.SYNC = FALSE;
	// BAUD1CONbits.SCKP = true;
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
  UWT(str);
  
}

#elif 0

void UART_int(uint16_t hvar){

  char str[11];

  uint16_t parser = hvar;

  uint_to_str(parser, str);
	
  UWT(&str[0]);
	
}

#else

void UART_int(uint16_t hvar){

  unsigned char str[8];

  // uint_to_str(hvar, str);

  int_to_str_converter(hvar, str);
	
  UWT(&str[0]);
	
}

#endif







void send_string(const unsigned char *str_pnt){

  const uint8_t const_max_length = 128;
  uint8_t sent_char_cnt = 0;

	while((*str_pnt != NULL_TERMINATOR) && (sent_char_cnt < const_max_length))
	{
		
		transmit_char(*str_pnt);
		str_pnt++;
#if ENFORCE_MISRA_RULE    
    sent_char_cnt = (uint8_t)((uint16_t)sent_char_cnt + 1u);
#else    
    sent_char_cnt++;
#endif  
	}	
	

}


void send_character(uint8_t the_char){
	
	
	transmit_char(the_char);
	
	
}

#if DB_LED_PWM

uint8_t check_next_char(void){
	
	uint8_t ret_value = 0;
	uint8_t rx_data = 0;
  

	static uint8_t max_len = CHARS_TO_RECEIVE + 1;

	
#if LOCAL_DB_UART
  
uint8_t db_data[3];
db_data[1] = CR;
db_data[2] = LF;
db_data[3] = NULL_TERMINATOR;
#endif


#if 0
	rx_data = get_data_value_from_buffer();
#else  
  get_data_from_buffer_with_pnt(&rx_data);
#endif	

#if LOCAL_DB_UART&&0
  
  UWT("Next char: ");
  UART_int(rx_data);
  db_data[0] = rx_data;
  UWT(db_data);
  
#endif
	// first we check if there was allready a header found and 
	// therefore we just add this one to the rest...
	if(HEADER_FOUND == true)
	{
		// this would be the kind of order we have
		if(NEXT_CHAR_IS_ORDER_BYTE == true)
		{
			NEXT_CHAR_IS_ORDER_BYTE = false;
			
			switch(rx_data)
			{
				case 'r':
					// set handler flag, reset everything
					*(huart.data_pnt) = rx_data;
					huart.data_pnt++;
					*(huart.data_pnt) = NULL_TERMINATOR;
					huart.data_lencnt++;
					prepare_tx_handler();
					max_len = CHARS_TO_RECEIVE + 1;
				break;
				case 'w':
					// basically nothing else to do
					*(huart.data_pnt) = rx_data;
					huart.data_pnt++;
					huart.data_lencnt++;
					max_len = CHARS_TO_RECEIVE + 1;
				break;
#if DB_LED_PWM
				case 'd':
					
					*(huart.data_pnt) = rx_data;
					huart.data_pnt++;
					huart.data_lencnt++;
					max_len = 1;
				break;
#endif					
				default:
					// reset everything
#if LANGUAGE_SPANISH
					UWT("\r\nMAL COMANDO BYTE\r\n");
#else						
					UWT("\r\nBAD ORDER BYTE\r\n");
#endif					
					reset_data_reception();
				break;
				
				
			}
			// we accept only 'w' or 'r'
		}
		else
		{
			*(huart.data_pnt) = rx_data;
			huart.data_pnt++;
			huart.data_lencnt++;
			if(huart.data_lencnt >= max_len)	// (CHARS_TO_RECEIVE + 1))
			{
#if DB_LED_PWM
				UWT("rx: ");
				UART_int(huart.data_lencnt);

				if(max_len == 1)
				{
					UWT("Order received!\r\n");
					set_led_var(rx_data);
				}
#endif				
				
				*(huart.data_pnt) = NULL_TERMINATOR;
				reset_data_reception();
				if(chcksum_checker() == false)
				{
					prepare_tx_handler();
				}
				else
				{
					ret_value = RX_PC_BAD_CHCKSUM;
				}
			}
		}
	}	

	check_on_startbyte(rx_data);	
	
	// reset_swoff_tmr_of_cnt();
	
	return ret_value;
	
}

#else
	
uint8_t check_next_char(void){
	
	uint8_t ret_value = 0;
	uint8_t rx_data = 0;
  static uint8_t max_len = CHARS_TO_RECEIVE + 1;

	
#if LOCAL_DB_UART
  
uint8_t db_data[3];
db_data[1] = CR;
db_data[2] = LF;
db_data[3] = NULL_TERMINATOR;
#endif


#if 0
	rx_data = get_data_value_from_buffer();
#else  
  get_data_from_buffer_with_pnt(&rx_data);
#endif	


#if DEBUGGING_IS_ON&&0
  if((rx_data - 0x30u) < 0x08u)
  {
    UWT("Col: ");
    UART_int((rx_data - 0x30u));
    leds_update_shadow_led(e_LED_STATE, (rx_data - 0x30u));
    leds_update_shadow_led(e_LED_ON_OFF, (rx_data - 0x30u));
  }
#endif  
  

#if LOCAL_DB_UART&&0
  
  UWT("Next char: ");
  UART_int(rx_data);
  db_data[0] = rx_data;
  UWT(db_data);
  
#endif
	// first we check if there was allready a header found and 
	// therefore we just add this one to the rest...
	if(HEADER_FOUND == TRUE)
	{
		// this would be the kind of order we have
		if(NEXT_CHAR_IS_ORDER_BYTE == TRUE)
		{
				NEXT_CHAR_IS_ORDER_BYTE = FALSE;
				
				switch(rx_data)
				{
					case 's':
#if DEBUGGING_IS_ON          
            SWITCH_CLOCK = TRUE;
#endif            
					break;
					case 'w':
						// basically nothing else to do
						*(huart.data_pnt) = rx_data;
						huart.data_pnt++;
						huart.data_lencnt++;
						max_len = CHARS_TO_RECEIVE + 1;
					break;

					case 'n':
						*(huart.data_pnt) = rx_data;
						huart.data_pnt++;
						huart.data_lencnt++;
						// this max_len here is actually one shorter because i do not send a chcksum from the pc!!
						// i rather create it afterwards...
						max_len = 4;
					break;
					
          case 'f':
            RESET();
          break;
					default:
						reset_data_reception();
					break;
					
					
				}
			// we accept only 'w' or 'r'
		}
		else
		{
			*(huart.data_pnt) = rx_data;
			huart.data_pnt++;
			huart.data_lencnt++;
			if(huart.data_lencnt >= max_len)	// (CHARS_TO_RECEIVE + 1))
#if UART_BTN_SIMUL				
			{
				if(max_len == 4)
				{
					*(huart.data_pnt) = checksumming_chcksum_creator(&tx_data[1], 3);
					max_len++;								
				}
				// *(huart.data_pnt) = checksumming_chcksum_creator(&tx_data[1], 3);
				
				reset_data_reception();
				// checksumming_chcksum_creator
				// if(chcksum_checker() == false)
				if(checksumming_chcksum_checker(&tx_data[1], max_len - 1) == false)
				{
					prepare_tx_handler();
				}
				else
				{
					ret_value = RX_PC_BAD_CHCKSUM;
				}
			}
#else
			{
				*(huart.data_pnt) = NULL_TERMINATOR;
				reset_data_reception();
#if 0				
				if(chcksum_checker() == false)
#else			
				if(checksumming_chcksum_checker(&tx_data[1], max_len - 1) == false)
#endif			
				{
					prepare_tx_handler();
				}
				else
				{
					ret_value = RX_PC_BAD_CHCKSUM;
				}
			}
#endif			
		}
	}	

	check_on_startbyte(rx_data);	

	return ret_value;
	
}

#endif

uint8_t *get_pnt_to_uart_rx_buffer(void){
  
  return &tx_data[0];
  
  
}

inline void uart_swoff_reception(void){
	
	RCSTAbits.CREN = false;
	
}

// PRIVATE functions *******************


static void prepare_tx_handler(void){
	
	
	// RCSTAbits.CREN = false;
	// handlers_generic_set_handler_FLG(e_tx_to_gps);
	
}


static void check_on_startbyte(uint8_t the_data){
	
	if(STARTBYTE == the_data)
	{

#if LOCAL_DB_UART&&0
  
  UWT("D\r\n");

#endif	
		reset_header_reception();
		STARTBYTE_FOUND = true;
		// *(huart.header_pnt) = the_data;
		// huart.header_pnt++;	
		// huart.header_lencnt++;
		
	}
	
	if(STARTBYTE_FOUND == true)
	{
		
#if LOCAL_DB_UART&&0
  
  UWT("E\r\n");

#endif	
		*(huart.header_pnt) = the_data;
		huart.header_pnt++;	
		huart.header_lencnt++;
		if(huart.header_lencnt >= HEADER_LENGTH)
		{
      #if LOCAL_DB_UART&&0
  
  UWT("F\r\n");

#endif	
			if(received_new_header() == true)
			{
				// reset everything to this ...
				HEADER_FOUND = true;
				NEXT_CHAR_IS_ORDER_BYTE = true;
				huart.data_lencnt = 0;
				huart.data_pnt = &tx_data[0];
			}
			reset_header_reception();
		}
	}

#if LOCAL_DB_UART&&0
  
  UWT("Header flgs: ");
  UART_int(huart.FLGS.reg);
  // UART_int(rndcnt++);
#endif

}



static uint8_t received_new_header(void){
	
	uint8_t ret_value = false;
	
	if(strcmp((const char *)&huart.header[0], (char *)&rx_header[0]) == false)
	{
		ret_value = true;
#if LOCAL_DB_UART&&0
  
  UWT("G\r\n");

#endif	
	}
  
#if LOCAL_DB_UART&&0
  
  UWT("H\r\n");
  UWT(&huart.header);
  UWT(&rx_header[0]);
#endif	
  
	return ret_value;
}



static void reset_header_reception(void){
	
	huart.header_pnt = &huart.header[0];
	huart.header_lencnt = 0;
	huart.header[5] = NULL_TERMINATOR;
	STARTBYTE_FOUND = false;
	
	
}



static void reset_data_reception(void){
	
	// reset_header_reception();
	huart.data_lencnt = 0;
	huart.data_pnt = &tx_data[0];
	HEADER_FOUND = false;
	NEXT_CHAR_IS_ORDER_BYTE = false;
	
	
}


static void transmit_char(uint8_t n_char){
	
	
	while(!TRANSMIT_BUFFER_FULL);
	
	TRANSMIT_BUFFER = n_char;
	
	
}


static uint8_t chcksum_checker(void){
  
  uint8_t ret_value = 0u;
  uint8_t hlooper = 0u;
  uint8_t chcksum = 0u;
  
  for(hlooper = 0; hlooper < CHARS_TO_RECEIVE; hlooper++)
  {
    
    chcksum = chcksum ^ tx_data[hlooper + 1u];
    
  }

#if 0

	UWT("chcksum: ");
	UART_int(chcksum);

	return false;
  
#else		
	
	return chcksum;
  
#endif	

}





#undef LOCAL_DB_UART






















