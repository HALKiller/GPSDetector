// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com



//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //

// this part is getting called after a reset and. there is a timeout timer getting configured
// to return to main if the timer expires.


// the TMR2 is configured to OF every 15ms --> therefore if we have a halfbit_cnt of 5 we wait 75ms.
// therefore a full bit has 150ms length.

//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //




#include "rx_luz.h"
#include "user.h"
#include "USART.h"
#include "eeprom.h"
#include "global.h"

#include "bit_banged_uart.h"

#include <stdint.h>

//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

// if not set we compile only parts from here so that we have enough 
// memory available for debugging reasons...
#define COMPILE_FULL_PROJECT 1

#define REDUCE_MEM_USAGE 1
#define MEM_RED_VAR 0
#define USE_VARIABLE_INSTEAD_OF_RETURN_VALUE_FROM_FUNCTION_CALL 0
#define NOT_USE_TMR1_RESET_FUNCTION 0

#define FAST_PROGRAMMER 1

// #define INVERTED_LDR_SENSOR 1


#if MEM_RED_VAR

uint8_t TMR1_TIMEOUT = 0;	//  	luz_flgs.timeout
uint8_t AUX_FLG = 0;	// 				luz_flgs.aux_flg
uint8_t STARTBIT_FOUND = 0;	//  luz_flgs.startbit


#else

union udt_flags{
	uint8_t reg;
	
	struct {
		unsigned timeout				: 1;
		unsigned wait_for_order	: 1;
		unsigned aux_flg				: 1;
		unsigned startbit				: 1;
	};
	
	
	
};

union udt_flags luz_flgs;


#endif


enum {
	
	e_WRITE_ORDER	= 1,
	e_READ_ORDER,  
	e_WRITE_NUMBER,
	e_TIMED_OUT,
	
};

enum d_length{
	e_WRITE_NUMBER_LEN = 5,	// 1 orderbyte + 3 databyte + 1 chcksum
};


enum{
  
  TMR_15ms_OF_TIME,
  TMR_1ms_OF_TIME,
  HIGH_SPEED_LUZ_DEBUGGER,
  BAUD_65_RATE,
  BAUD_HALF_65_RATE,
  TEST_RATE,
  
};


//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 
 #if INVERTED_LDR_SENSOR
 // these are the adc from the light sensor (LDR) 
// const uint8_t ADC_threshold = 10;  
const uint8_t ADC_threshold = 154;  

#else
  
const uint8_t ADC_threshold = 95;  

#endif
// 95; For high load switching and R = 63k - 75k (smaller)
// 3330/255 


const uint8_t eeprom_addresses[] =
  {
		0x22,	0x23,	0x24,	0x25,	0x27,
		0x28,	0x29,	0x2A,	0x2C,	0x2D,
		0x2E,	0x2F,	0x31,	0x32,	0x33,
		0x34,	0x36,	0x37,	0x38,	0x39,
		0x3A,	0x3B,	0x3C,	0x3D,	0x3E,
		0x3F,	0x40,	0x42,	0x43,	0x44,
		0x45,	0x46,	0x47,	0x48 
	};


// char us_to_send[] = "007680";

// char us_to_send[] = "064000";
char us_to_send[] = "030720";

//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 

 
#define EEPROM_DETECTOR_NUMBER_LOOKUP_SLOT 24
#define EEPROM_MAX_DETECTORES_LOOKUP_SLOT	32
#define DETECTORES_MEM_SLOTS_FOR_NUMBER 3


#define SEND_FULL_CONFIG 0
#define SEND_DETECTOR_NUMBER 1
#define SEND_ERROR_MAX_DETECTORES 2
#define SEND_RX_SPEED 3




#define TX_ERROR_HEADER 101
#define TX_DUMMY_BYTE 0x55

#define RX_LUZ_HALFBIT_TIME 1	// that is 25ms OF_time
#define TX_LUZ_HALFBIT_TIME 2	// that is ...15ms(?) OF time

#define TX_LUZ_TMR_OF_CNT 6	// 150ms

#if !MEM_RED_VAR

#define TMR1_TIMEOUT 	luz_flgs.timeout
#define AUX_FLG				luz_flgs.aux_flg
#define STARTBIT_FOUND luz_flgs.startbit

#endif



#define TMR1_2_SECOND_OF_CNT 4	// 4 = 2seconds, 8 = 4seconds

#define RX_LUZ_HALFBIT_TIME_OF_CNT 5
#define RX_LUZ_FULLBIT_TIME_OF_CNT 10
#define BITCNT 8

#define WRITE_ORDER	1
#define READ_ORDER  2
#define TIMED_OUT		3

#define STARTBIT false	
#define STOPBIT true

#define CHARS_TO_RECEIVE	35	// 34 config bytes + 1 chcksum





// this is the SECOND_RELEASED_VERSION_RX_BYTE on the grabador de luz

// #define SENSOR_LDR_IDENTIFYER 49
// #define SENSOR_LDR_IDENTIFYER 61
// #define SENSOR_LDR_IDENTIFYER 0x85
#define BIG_Z 0x5A
#define SENSOR_LDR_IDENTIFYER BIG_Z // 0x85



//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //


uint8_t timeout_cnt = 0;

// uint8_t config_bytes[CHARS_TO_RECEIVE];

#if USE_VARIABLE_INSTEAD_OF_RETURN_VALUE_FROM_FUNCTION_CALL
uint8_t fake_ret_val = false;
static void detector_number_is_valid(void);
#else
static uint8_t detector_number_is_valid(void);
#endif


//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //



static void configure_tmr2(uint8_t timeout_setter);


static uint8_t wait_for_startbyte(void);
static void Inicio_uart_luz(void);
static void tx_build(void);
static void reset_timeout_timer(void);

static uint8_t rx_config_data(uint8_t order_id);

static uint8_t tx_config_data(uint8_t order_id);

static uint8_t timeout_checker(void);
static void wait_for_tmr_expires(uint8_t loopcnt);
inline static void t2_reset(void);
static uint8_t get_next_luz_char(void);
static uint8_t wait_for_startbit(void);

static uint8_t chcksum_checker(uint8_t to_loop_cnt);

static void write_rx_data_to_eeprom(void);
static void write_detector_number_data_to_eeprom(void);
static void tx_luz(uint8_t tx_data);
static void send_it(const uint8_t *bit_arr);
static void set_bb_uart(uint8_t b_val);


//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

#if COMPILE_WITH_RX_LUZ

void check_on_rx_luz(void){

#if REDUCE_MEM_USAGE
	uint8_t hlooper = 0;
#endif

	Inicio_uart_luz();

#if DB_UART_ON
  UART_Write_Text("\r\nReset\r\n");
  // UWT("Rx_LUZ_ON\r\n");
	tx_build();
#endif	

#if FAST_PROGRAMMER&&1

	tx_luz(SENSOR_LDR_IDENTIFYER);
  __delay_ms(200);
  tx_config_data(SEND_RX_SPEED);

#else

	LUZ_ON_OFF = true;
	TMR2ON = true;
	wait_for_tmr_expires(RX_LUZ_HALFBIT_TIME_OF_CNT);	// 75ms
	LUZ_ON_OFF = false;


#endif

  
#if REDUCE_MEM_USAGE&&NOT_USE_TMR1_RESET_FUNCTION	

	timeout_cnt = TMR1_2_SECOND_OF_CNT;
	TMR1IF = false;
	TMR1H = 0;
	TMR1L = 0;	

#else	
	
	reset_timeout_timer();

#endif	

	TMR1ON = true;

		switch(wait_for_startbyte())
		{
			case e_WRITE_ORDER:
				if(rx_config_data(CHARS_TO_RECEIVE) == true)
				{
					if(chcksum_checker(CHARS_TO_RECEIVE) == false) 
          {
            write_rx_data_to_eeprom();
#if DB_LUZ_UART						
						UWT("EEPROM write\r\n");
#endif						
						tx_config_data(SEND_FULL_CONFIG);
          }
				}
			break;
#if COMPILE_FULL_PROJECT	
			case e_WRITE_NUMBER:
				if(rx_config_data(e_WRITE_NUMBER_LEN - 1) == true)
				{
					if(chcksum_checker(e_WRITE_NUMBER_LEN - 1) == false)
          {
#if USE_VARIABLE_INSTEAD_OF_RETURN_VALUE_FROM_FUNCTION_CALL						
							
						detector_number_is_valid();	// run the function...
						if(fake_ret_val == true)
							
#else		
	
						if(detector_number_is_valid() == true)
							
#endif							
						{
							
#if REDUCE_MEM_USAGE			
							// saves 3 words --> function call overhead
							for(hlooper = 0; hlooper < e_WRITE_NUMBER_LEN - 2; hlooper++)
							{
								
								write_eeprom(eeprom_addresses[hlooper + EEPROM_DETECTOR_NUMBER_LOOKUP_SLOT], gEntradaDeTrama.BuferDeEntrada[hlooper]);
								
							}
#else							
							
							write_detector_number_data_to_eeprom();
							
#endif
							
#if DB_LUZ_UART						
							UWT("EEPROM write\r\n");
#endif						
							tx_config_data(SEND_DETECTOR_NUMBER);
						}
						else
						{
							// answering with err message --> "e + max.number"
							tx_config_data(SEND_ERROR_MAX_DETECTORES);
							
						}
          }
				}
			break;			
#endif	// COMPILE_FULL_PROJECT
			case e_READ_ORDER:
				tx_config_data(SEND_FULL_CONFIG);
			break;
			case e_TIMED_OUT:
				TMR1_TIMEOUT = true;
			default:
			break;
	
		}	
		
	
	
	TMR1ON = false;
	TMR2ON = false;
	
#if USE_BIT_BANGED_UART

  // Using TMR4 for that now...
  init_TMR_bitbang_uart();
  
#endif  
  
  
}

#else
	

// so that we are only making a short init here and than return to main
// that is for a reduction of code and therefore for debugging 
void check_on_rx_luz(void){
	
	Inicio_uart_luz();

#if DEBUGGING_IS_ON
  UART_Write_Text("\r\nReset\r\n");
	tx_build();
#endif	
	
	LUZ_ON_OFF = true;
	TMR2ON = true;
	wait_for_tmr_expires(RX_LUZ_HALFBIT_TIME_OF_CNT);	// 45ms
	LUZ_ON_OFF = false;
	
	
	
}

#endif

//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //

#if 1

void rx_luz_tx_db_luz(uint8_t *data){
  
  
  configure_tmr2(HIGH_SPEED_LUZ_DEBUGGER);
  
  while(*data != NULL_TERMINATOR)
  {
    tx_luz(*data);
    data++;
  }
  
}

#endif


static void Inicio_uart_luz(void){
  
#if PIC_16F1936
	// because the 16F1936 has a lower clock after reset....
	ConfigureOscillator();
	
	// LCDCONbits.LCDEN = 0;	// that should be false anyway after reset but well...
	LCDCON = 0;
#endif

#if !REDUCE_MEM_USAGE	
	luz_flgs.reg = 0;
#endif	


#if USE_OV_PINS
	TRISA = 0x28;
#else	
  TRISA = 0b00101000;
#endif


#if USE_OV_PINS


	#if COMPILE_WITH_ALERNATIV_INT_PIN_ASSIGNMENT
		TRISB = 0x23;
	#else
		TRISB = 0x03;
	#endif	


#else	// USE_OV_PINS

  TRISB = 0b00100011;
	
#endif

	WPUB = 0x00;
	

#if USE_OV_PINS
	TRISC = 0x80;
#else
  TRISC = 0b11000011;
#endif


	
#if PIC_16F1936
	LATA = 0x40;
	LATB = 0x00;
	LATC = 0x04;
#else
  PORTA = 0x40;
  PORTB = 0x00;
  PORTC = 0x04;
#endif
	
	

#if OV_INTERRUPT_POSICION

#if PIC_16F1936
  
	ANSELA = 0x28;
	
#if COMPILE_WITH_ALERNATIV_INT_PIN_ASSIGNMENT	
	ANSELB = 0x21;
#else
	ANSELB = 0x03;
#endif	
	
#else	// PIC_16F1936
	
	ANSEL = 0x18;
	ANSELH = 0x10;
	
#endif


#else	// OV_INTERRUPT_POSICION


#if PIC_16F1936
  
	ANSELA = 0x28;
	ANSELB = 0x00;
	
#else
	
	ANSEL = 0x18;
	ANSELH = 0x00;


#endif

#endif	// OV_INTERRUPT_POSICION
	
	



	ADIE = 0;
  ADIF = 0;
	
	
// Timer 1 //
  TMR1ON = 0;
  TMR1H = 0x00;
  TMR1L = 0x00;
  // Luego se ajusta el preescaler a 2
  T1CONbits.T1CKPS = 0x03;
		// 1:8 // Se ajustan los flags de interrupción y activación
  TMR1IF = 0;
  TMR1IE = 1;

// because we are sending now a version byte as identifyer ...

	configure_tmr2(TMR_1ms_OF_TIME);

#if USE_BIT_BANGED_UART
// Therefroe the IF flag is every 104us -->bittime!
  
  init_TMR_bitbang_uart();
#endif  

#if DB_UART_ON

#if DB_LUZ_UART
	
	UART_Init_125000_4MHz();
	
#else	
	
	UART_Init_9600_4MHz();
	
#endif	


#elif BAUD_65_DB

// #define B65_DB LATCbits.LATC6

  B65_DB = false;

#else

  UART_Init_9600_4MHz();

	__delay_ms(1);
  
#endif	
	
}

#if DEBUGGING_IS_ON

static void tx_build(void){
	

	UWT("BUILD: ");
	UWT(__DATE__);
	UWT("  ");
	UWT(__TIME__);
	UART_CRLF;

	
}

#endif

static uint8_t wait_for_startbyte(void){
  
	uint8_t ret_value = 0;
	uint8_t order_byte = 0;	

#define GET_BUG_OUT 1

#if GET_BUG_OUT
#define MAX_BAD_RECEIVED_CHARS 5
  uint8_t rnd_cnt = 0;
#endif

  // therefore we start a long timer to see if there is no more reception
  // --> during the development it should not matter but in the end i need that one-->
  // therefore develop it straight away correctly
  
  // the long TMR is perhaps TMR1 --> lets check on the max possible timeout...
  // on max. timeout we +- 500ms therefore we could give a cnt to 4 for a maximum of 2 second timeout OF-->
  
  // once we have received a startcondition we keep on going otherwise we might 
  // just stop when there is no TMR time left
  	

#if SENSOR_LDR_IDENTIFYER==61||SENSOR_LDR_IDENTIFYER==0x85||SENSOR_LDR_IDENTIFYER==BIG_Z


  configure_tmr2(BAUD_HALF_65_RATE);
  // configure_tmr2(TEST_RATE);

  
#else
  
  UWT("\r\nBaud_15ms");
	configure_tmr2(TMR_15ms_OF_TIME);
  
#endif






	TMR2ON = true;
		
	wait_for_tmr_expires(RX_LUZ_HALFBIT_TIME_OF_CNT);
	
#if REDUCE_MEM_USAGE

	TMR2ON = false;
	TMR2 = 0;
	TMR2IF = false;
	
#else
	
	t2_reset();
	
#endif	
    
#if GET_BUG_OUT
	while((ret_value == 0) && (rnd_cnt < MAX_BAD_RECEIVED_CHARS))
#else
  while(ret_value == 0)
#endif
	{

		if(wait_for_startbit() == true)	
		{



#if GET_BUG_OUT      
      rnd_cnt++;
#endif      
			order_byte = get_next_luz_char();

#if REDUCE_MEM_USAGE

	TMR2ON = false;
	TMR2 = 0;
	TMR2IF = false;
	
#else
	
	t2_reset();
	
#endif	
	
#if REDUCE_MEM_USAGE

			switch(order_byte)
			{
				case 'r':
					
					ret_value = e_READ_ORDER;
				break;
				case 'w':
					
					ret_value = e_WRITE_ORDER;
				break;
				case 'n':
					
					ret_value = e_WRITE_NUMBER;				
				break;
				default:
				break;

			}
			
			if(ret_value != false)
			{
#if REDUCE_MEM_USAGE&&NOT_USE_TMR1_RESET_FUNCTION	

				timeout_cnt = TMR1_2_SECOND_OF_CNT;
				TMR1IF = false;
				TMR1H = 0;
				TMR1L = 0;	

#else	
	
				reset_timeout_timer();

#endif

			}
			
#else
	
			switch(order_byte)
			{
				case 'r':
					reset_timeout_timer();
					ret_value = e_READ_ORDER;
				break;
				case 'w':
					reset_timeout_timer();
					ret_value = e_WRITE_ORDER;
				break;
				case 'n':
					reset_timeout_timer();
					ret_value = e_WRITE_NUMBER;				
				break;
				default:
				break;

			}
			
#endif

			
		}
		else
		{
			ret_value = e_TIMED_OUT;
		}
	}
  
  return ret_value;
  
  
  
}




#if INVERTED_LDR_SENSOR

                             
static uint8_t wait_for_startbit(void){
	
	// waiting for H->L transition (Dark to Light)

	while(1)
	{
		// LeerValorLDR();
// wait for darkness...
		if(LeerValorLDR() < ADC_threshold)
		{
			
			while(1)
			{
				
				// LeerValorLDR();
// wait for light
				if(LeerValorLDR() > ADC_threshold)
				{
#if BAUD_65_DB
          B65_DB = !B65_DB;
#endif
					return true;
				}
				else
				{
					if(timeout_checker() == true)
					{
						return false;
					}
				}			
			}
		}
		else
		{
			if(timeout_checker() == true)
			{
				
				return false;
			}
		}
	}
}


#else
	

                             


static uint8_t wait_for_startbit(void){
	
	// waiting for H->L transition (Dark to Light)


	while(1)
	{
		// LeerValorLDR();
// wait for darkness...
     
		if(LeerValorLDR() > ADC_threshold)
		{
			
			while(1)
     
        
			{
				
				// LeerValorLDR();
// wait for light
				if(LeerValorLDR() < ADC_threshold)
				{
					return true;
				}
				else
				{
					if(timeout_checker() == true)
					{
						return false;
					}
 
				}			
			}
		}
		else
		{
			if(timeout_checker() == true)
			{
				
				return false;
  
			}
		}
	}
}



#endif


#if 1

static uint8_t rx_config_data(uint8_t to_loop_cnt){
	// we have received a startbyte 'w' and now we are waiting for xy chars to be written inot th eEEPROM if the received 
	// chars are correct --> send with it a single chcksum byte
	uint8_t bytelooper = 0;
	// uint8_t ret_value = true;
  // uint8_t to_loop_to = 0;

	for(bytelooper = 0; bytelooper < to_loop_cnt; bytelooper++)
	{
		if(wait_for_startbit() == true)
		{
      
 #if DB_LUZ_UART&&1
      UART_int(bytelooper);
 #endif
			gEntradaDeTrama.BuferDeEntrada[bytelooper] = get_next_luz_char();

      
		}
		else
		{
		  return false;
		}
	}
	
	return true;
	
}

#else
  
static uint8_t rx_config_data(uint8_t to_loop_cnt){
	// we have received a startbyte 'w' and now we are waiting for xy chars to be written inot th eEEPROM if the received 
	// chars are correct --> send with it a single chcksum byte
	uint8_t bytelooper = 0;
	uint8_t ret_value = true;
  uint8_t to_loop_to = 0;

	for(bytelooper = 0; bytelooper < to_loop_cnt; bytelooper++)
	{
		if(wait_for_startbit() == true)
		{
      
 #if DB_LUZ_UART&&1
      UART_int(bytelooper);
 #endif
			gEntradaDeTrama.BuferDeEntrada[bytelooper] = get_next_luz_char();

      
		}
		else
		{
			// this is timeout
			bytelooper = to_loop_to;
			ret_value = false;
#if DB_LUZ_UART			
      UWT("Timeout\r\n");
#endif     
		}
	}
	
	return ret_value;
	
}

#endif


#if 1



static uint8_t tx_config_data(uint8_t order_id){

	uint8_t ret_value = false;
  uint8_t hlooper = 0;
  uint8_t chcksum = 0;
  uint8_t tx_data = 0;
  

#if 1

#if DB_LUZ_UART
	DB_SWAP;
#endif  





  
 // a 300ms waiter here...
	configure_tmr2(TMR_15ms_OF_TIME);

  TMR2ON = true;
	wait_for_tmr_expires(20);

	configure_tmr2(TMR_1ms_OF_TIME);
  

#endif    
  
  
#if DB_LUZ_UART
	DB_SWAP;
	UWT("\r\n");
	
#endif  	



	switch(order_id)
	{
		case SEND_FULL_CONFIG:
		
			for(hlooper = 0; hlooper < CHARS_TO_RECEIVE - 1; hlooper++)
			{
				
				tx_data = LeerEeprom(eeprom_addresses[hlooper]);
				chcksum = chcksum ^ tx_data;
				tx_luz(tx_data);
				
#if DB_LUZ_UART 
				UART_int(hlooper);
				UART_int(tx_data);
				UWT("\r\n");
#endif    
			}
		
		break;
		
		case SEND_DETECTOR_NUMBER:
		
			for(hlooper = 0; hlooper < e_WRITE_NUMBER_LEN - 2; hlooper++)
			{
				
				tx_data = LeerEeprom(eeprom_addresses[hlooper + EEPROM_DETECTOR_NUMBER_LOOKUP_SLOT]);
				chcksum = chcksum ^ tx_data;
				tx_luz(tx_data);
				
		#if DB_LUZ_UART 
				UART_int(hlooper);
				UART_int(tx_data);
				UWT("\r\n");
		#endif    
			}		
			
		break;
		
		case SEND_ERROR_MAX_DETECTORES:
			
			tx_data = TX_ERROR_HEADER;	// 'e';
			chcksum = chcksum ^ tx_data;
			tx_luz(tx_data);
			
			tx_data = LeerEeprom(eeprom_addresses[EEPROM_MAX_DETECTORES_LOOKUP_SLOT]);
			chcksum = chcksum ^ tx_data;
			tx_luz(tx_data);
			
			tx_data = TX_DUMMY_BYTE;
			chcksum = chcksum ^ tx_data;
			tx_luz(tx_data);
			
		break;
    case SEND_RX_SPEED:
    
      for(hlooper = 0; hlooper < 6; hlooper++)
      {
        
        tx_data = us_to_send[hlooper]; //LeerEeprom(eeprom_addresses[hlooper + EEPROM_DETECTOR_NUMBER_LOOKUP_SLOT]);
				chcksum = chcksum ^ tx_data;
				tx_luz(tx_data);
        
      }    	

      
    break;
    
    
		default:
		
		break;

	}
  
	tx_luz(chcksum);
  
}



#else
	

// the original working version --> siz: 79 words

static uint8_t tx_config_data(uint8_t order_id){

	uint8_t ret_value = false;
  uint8_t hlooper = 0;
  uint8_t chcksum = 0;
  uint8_t tx_data = 0;
  

#if 1

#if DB_LUZ_UART
	DB_SWAP;
#endif  

#if REDUCE_MEM_USAGE

	TMR2ON = false;
	TMR2 = 0;
	TMR2IF = false;
	
#else
	
	t2_reset();
	
#endif	
  
  TMR2ON = true;
	wait_for_tmr_expires(20);
  
#if DB_LUZ_UART
	DB_SWAP;
	UWT("\r\n");
	
#endif  	

#endif

	switch(order_id)
	{
		case SEND_FULL_CONFIG:
		
			for(hlooper = 0; hlooper < CHARS_TO_RECEIVE - 1; hlooper++)
			{
				
				tx_data = LeerEeprom(eeprom_addresses[hlooper]);
				chcksum = chcksum ^ tx_data;
				tx_luz(tx_data);
				
#if DB_LUZ_UART 
				UART_int(hlooper);
				UART_int(tx_data);
				UWT("\r\n");
#endif    
			}
		
		break;
		
		case SEND_DETECTOR_NUMBER:
		
			for(hlooper = 0; hlooper < e_WRITE_NUMBER_LEN - 2; hlooper++)
			{
				
				tx_data = LeerEeprom(eeprom_addresses[hlooper + EEPROM_DETECTOR_NUMBER_LOOKUP_SLOT]);
				chcksum = chcksum ^ tx_data;
				tx_luz(tx_data);
				
		#if DB_LUZ_UART 
				UART_int(hlooper);
				UART_int(tx_data);
				UWT("\r\n");
		#endif    
			}		
			
		break;
		
		case SEND_ERROR_MAX_DETECTORES:
			
			tx_data = TX_ERROR_HEADER;	// 'e';
			chcksum = chcksum ^ tx_data;
			tx_luz(tx_data);
			
			tx_data = LeerEeprom(eeprom_addresses[EEPROM_MAX_DETECTORES_LOOKUP_SLOT]);
			chcksum = chcksum ^ tx_data;
			tx_luz(tx_data);
			
			tx_data = TX_DUMMY_BYTE;
			chcksum = chcksum ^ tx_data;
			tx_luz(tx_data);
			
		break;
    
    case SEND_RX_SPEED:
    
      for(hlooper = 0; hlooper < 6; hlooper++)
      {
        
        tx_data = us_to_send[hlooper]; //LeerEeprom(eeprom_addresses[hlooper + EEPROM_DETECTOR_NUMBER_LOOKUP_SLOT]);
				chcksum = chcksum ^ tx_data;
				tx_luz(tx_data);
        
      }    	

      
    break;
    
    
		default:
		
		break;

	}
  
	tx_luz(chcksum);
  
}

#endif


static uint8_t chcksum_checker(uint8_t to_loop_cnt){
  uint8_t ret_value = false;
  uint8_t hlooper = 0;
  uint8_t chcksum = 0;
  
#if DB_LUZ_UART  
  UWT("Chcksum: \r\n");
#endif  
  
  for(hlooper = 0; hlooper < to_loop_cnt; hlooper++)
  {
    
    chcksum = chcksum ^ gEntradaDeTrama.BuferDeEntrada[hlooper];
#if DB_LUZ_UART
	UART_int(gEntradaDeTrama.BuferDeEntrada[hlooper]);
	UART_int(chcksum);
	UWT("\r\n");
#endif
  }
	
#if DB_LUZ_UART&&1

	UWT("chcksum: ");
	UART_int(chcksum);

	
#endif	
	return chcksum;

}


#if USE_VARIABLE_INSTEAD_OF_RETURN_VALUE_FROM_FUNCTION_CALL

// i will look to replace the return value type of function with 
// a variable that i manipulate and test later on if that is than set or not

static void detector_number_is_valid(void){
	
	uint8_t hlooper = 0;
	uint8_t max_detectores = 0;
	uint16_t received_detector_number = 0;
	
	
	
	// get the max. number from eeprom..
	fake_ret_val = true;
	max_detectores = LeerEeprom(eeprom_addresses[EEPROM_MAX_DETECTORES_LOOKUP_SLOT]);
	
	
	
	for(hlooper = 0; hlooper < DETECTORES_MEM_SLOTS_FOR_NUMBER; hlooper++)
	{
		if((gEntradaDeTrama.BuferDeEntrada[hlooper] >= 0x30) && (gEntradaDeTrama.BuferDeEntrada[hlooper] <= 0x39))
		{
			received_detector_number = 10 * received_detector_number + (gEntradaDeTrama.BuferDeEntrada[hlooper] - 0x30);	
		}
		else
		{
			fake_ret_val = false;
		}
		
	}
	
	if(received_detector_number > max_detectores)
	{
		fake_ret_val = false;
	}
	
	
}

#elif 1

// this is a reduced version and seems funcional...
static uint8_t detector_number_is_valid(void){
	
	uint8_t hlooper = 0;
	uint8_t max_detectores = 0;
	uint16_t received_detector_number = 0;
	// uint8_t ret_value = true;
	uint8_t temp_val;
	
	// get the max. number from eeprom..
	max_detectores = LeerEeprom(eeprom_addresses[EEPROM_MAX_DETECTORES_LOOKUP_SLOT]);
	
	
	for(hlooper = 0; hlooper < DETECTORES_MEM_SLOTS_FOR_NUMBER; hlooper++)
	{

		// subtract straight away the .d48 to get the actual number...
		temp_val = gEntradaDeTrama.BuferDeEntrada[hlooper] - 0x30;
		
		// and just test that it is now in the correct range...--> data validated
		if(temp_val < 10)
		{
			received_detector_number = 10 * received_detector_number + temp_val;	// (gEntradaDeTrama.BuferDeEntrada[hlooper] - 0x30);	
		}
		else
		{
			return false;
		}
		
	}
	
	if(received_detector_number > max_detectores)
	{
		return false;
	}
	
	if(received_detector_number < 1)
	{
		return false;
	}
	
	
	
	
	return true;
	
}


#else
	
// this is the original working version
static uint8_t detector_number_is_valid(void){
	
	uint8_t hlooper = 0;
	uint8_t max_detectores = 0;
	uint16_t received_detector_number = 0;
	uint8_t ret_value = true;
	
	
	// get the max. number from eeprom..
	max_detectores = LeerEeprom(eeprom_addresses[EEPROM_MAX_DETECTORES_LOOKUP_SLOT]);
	

	
	for(hlooper = 0; hlooper < DETECTORES_MEM_SLOTS_FOR_NUMBER; hlooper++)
	{
		if((gEntradaDeTrama.BuferDeEntrada[hlooper] >= 0x30) && (gEntradaDeTrama.BuferDeEntrada[hlooper] <= 0x39))
		{
			received_detector_number = 10 * received_detector_number + (gEntradaDeTrama.BuferDeEntrada[hlooper] - 0x30);	
		}
		else
		{
			ret_value = false;
		}
		
	}
	
	if(received_detector_number > max_detectores)
	{
		ret_value = false;
	}
	
	return ret_value;
	
}

#endif

static void write_rx_data_to_eeprom(void){
  // uint8_t ret_value = false;
  uint8_t hlooper = 0;

  
  for(hlooper = 0; hlooper < CHARS_TO_RECEIVE - 1; hlooper++)
  {
    
    write_eeprom(eeprom_addresses[hlooper], gEntradaDeTrama.BuferDeEntrada[hlooper]);
    
  }
  
}

#if !REDUCE_MEM_USAGE

static void write_detector_number_data_to_eeprom(void){
  // uint8_t ret_value = false;
  uint8_t hlooper = 0;

  
  for(hlooper = 0; hlooper < e_WRITE_NUMBER_LEN - 2; hlooper++)
  {
    
    write_eeprom(eeprom_addresses[hlooper + EEPROM_DETECTOR_NUMBER_LOOKUP_SLOT], gEntradaDeTrama.BuferDeEntrada[hlooper]);
    
  }
  
}

#endif

#if INVERTED_LDR_SENSOR


static uint8_t get_next_luz_char(void){
	
	uint8_t rx_byte = 0;
	uint8_t bitlooper = 0;
#if DB_LUZ_UART		
	uint8_t t_val = 0;
#endif
	
#if DB_LUZ_UART&&1
	UWT("\r\nA ");
	
#endif	


#if REDUCE_MEM_USAGE&&NOT_USE_TMR1_RESET_FUNCTION	

	timeout_cnt = TMR1_2_SECOND_OF_CNT;
	TMR1IF = false;
	TMR1H = 0;
	TMR1L = 0;	

#else	
	
	reset_timeout_timer();

#endif
	
	
	TMR2ON = true;
	
	wait_for_tmr_expires(RX_LUZ_HALFBIT_TIME_OF_CNT);
  
#if BAUD_65_DB	
  B65_DB = !B65_DB;
#endif  

	for(bitlooper = 0; bitlooper < BITCNT; bitlooper++)
	{		
		wait_for_tmr_expires(RX_LUZ_FULLBIT_TIME_OF_CNT);
    
#if BAUD_65_DB	    
    B65_DB = !B65_DB;
#endif
    
#if DB_LUZ_UART		
		t_val =  LeerValorLDR();
	
		if(t_val > ADC_threshold)
#else
		if(LeerValorLDR() > ADC_threshold)
#endif				
		{
			rx_byte = rx_byte >> 1;
		}
		else
		{
			rx_byte = (rx_byte >> 1) + 128; 
		}
#if DB_LUZ_UART		
		UART_int(t_val);
#endif		

	}
#if DB_LUZ_UART&&0

	UWT("Byte: ");
	UART_int(rx_byte);
	UART_CRLF;
#endif	
	return rx_byte;
	
}




#else
	


static uint8_t get_next_luz_char(void){
	
	uint8_t rx_byte = 0;
	uint8_t bitlooper = 0;
#if DB_LUZ_UART		
	uint8_t t_val = 0;
#endif
	
#if DB_LUZ_UART&&1
	UWT("\r\nA ");
	
#endif	


#if REDUCE_MEM_USAGE&&NOT_USE_TMR1_RESET_FUNCTION	

	timeout_cnt = TMR1_2_SECOND_OF_CNT;
	TMR1IF = false;
	TMR1H = 0;
	TMR1L = 0;	

#else	
	
	reset_timeout_timer();

#endif
	
	
	TMR2ON = true;
	
	wait_for_tmr_expires(RX_LUZ_HALFBIT_TIME_OF_CNT);
  
#if BAUD_65_DB	
  B65_DB = !B65_DB;
#endif  

	for(bitlooper = 0; bitlooper < BITCNT; bitlooper++)
	{		
		wait_for_tmr_expires(RX_LUZ_FULLBIT_TIME_OF_CNT);
    
#if BAUD_65_DB	    
    B65_DB = !B65_DB;
#endif
    
#if DB_LUZ_UART		
		t_val =  LeerValorLDR();
	
		if(t_val < ADC_threshold)
#else
		if(LeerValorLDR() < ADC_threshold)
#endif				
                            
		{
			rx_byte = rx_byte >> 1;
		}
		else
		{
			rx_byte = (rx_byte >> 1) + 128; 
		}
#if DB_LUZ_UART		
		UART_int(t_val);
#endif		

	}
#if DB_LUZ_UART&&0

	UWT("Byte: ");
	UART_int(rx_byte);
	UART_CRLF;
#endif	
	return rx_byte;
	
}




#endif

#if 1

static uint8_t timeout_checker(void){
	
	uint8_t ret_value = 0;
	
	if(TMR1IF == true)
	{		
		TMR1IF = false;
		timeout_cnt--;
		if(timeout_cnt == 0)
		{	
			ret_value = true;
		}
	}

	return ret_value;
}

#elif 1

static uint8_t timeout_checker(void){
	
	uint8_t ret_value = 0;
	
	if(TMR1IF == true)
	{
		
		TMR1IF = false;
		timeout_cnt--;
		if(timeout_cnt == 0)
		{	
			ret_value = true;
		}
	}

	return ret_value;
}



#else
	

static uint8_t timeout_checker(void){
	
	uint8_t ret_value = 0;
	
	if(TMR1IF == true)
	{
		timeout_cnt--;
		TMR1IF = false;

	}
	if(timeout_cnt == 0)
	{	
		ret_value = true;
	}
	return ret_value;
}

#endif

static void wait_for_tmr_expires(uint8_t loopcnt){
	
	uint8_t hlooper = loopcnt;
	
	while(hlooper > 0)
	{
		while(TMR2IF == false);
		hlooper--;
		TMR2IF = false;
	}
	
	
	
}


#if REDUCE_MEM_USAGE&&NOT_USE_TMR1_RESET_FUNCTION

#else

static void reset_timeout_timer(void){
	
	timeout_cnt = TMR1_2_SECOND_OF_CNT;
	TMR1IF = false;
	TMR1H = 0;
	TMR1L = 0;
	
}

#endif

inline static void t2_reset(void){
	
	TMR2ON = false;
	TMR2 = 0;
	TMR2IF = false;
	
}



static void configure_tmr2(uint8_t timeout_setter){
  
  t2_reset();
  
  T2_PRESCALER = TMR2_04_PRESCALER;
  PR2 = 250;
  
  switch(timeout_setter)
  {
    case TMR_15ms_OF_TIME:
    
      // 15ms
      T2_POSTSCALER = TMR2_15_POSTSCALER;
      
    break;
    
    case TMR_1ms_OF_TIME:
      
      // 1ms
      T2_POSTSCALER = TMR2_01_POSTSCALER;
      
    break;
    case HIGH_SPEED_LUZ_DEBUGGER:
    
      T2_PRESCALER = TMR2_01_PRESCALER;
      PR2 = 250;
      T2_POSTSCALER = TMR2_02_POSTSCALER;
      
    break;
#if 0    
    case BAUD_65_RATE:
    
      T2_PRESCALER = TMR2_04_PRESCALER;
      PR2 = 48;
      T2_POSTSCALER = TMR2_08_POSTSCALER;
    
    break;
#endif    
    case BAUD_HALF_65_RATE:
    
      T2_PRESCALER = TMR2_04_PRESCALER;
      PR2 = 96; // PR2 = 96;
      T2_POSTSCALER = TMR2_08_POSTSCALER;
    
    break;
                           
                        

    case TEST_RATE:
                
                                         
    
      T2_PRESCALER = TMR2_04_PRESCALER;
      PR2 = 96;
      T2_POSTSCALER = TMR2_02_POSTSCALER;
    
    break;
    
    default:
    break;
    
  }

}










#if 1

static void tx_luz(uint8_t tx_data){

	#define BITS_TO_SEND 10
	
	// uint8_t the_bit = false;
	uint8_t hlooper = 0;
	// uint8_t bitwise[BITS_TO_SEND];
	
	
	
	TMR2ON = false;
	TMR2 = 0;
	TMR2IF = false;
	
	TMR2ON = true;
	

	// bitwise[0] = STARTBIT; equals...
	LUZ_ON_OFF = !(STARTBIT);
	
  wait_for_tmr_expires(TX_LUZ_TMR_OF_CNT);
	
	for(hlooper = 0; hlooper < 8; hlooper++)
	{
		
		// bitwise[hlooper + 1] = ((tx_data >> hlooper) & 1);
		LUZ_ON_OFF = !((tx_data >> hlooper) & 1);
		wait_for_tmr_expires(TX_LUZ_TMR_OF_CNT);
		
	}
	LUZ_ON_OFF = !STOPBIT;
	wait_for_tmr_expires(TX_LUZ_TMR_OF_CNT);
	
	
}

#else
	


static void tx_luz(uint8_t tx_data){
	
	#define BITS_TO_SEND 10
	
	uint8_t the_bit = false;
	uint8_t hlooper = 0;
	uint8_t bitwise[BITS_TO_SEND];
	
#if DB_LUZ_UART&&0
	UART_int(tx_data);
#else
	
  bitwise[0] = STARTBIT;
	
	for(hlooper = 0; hlooper < 8; hlooper++)
	{
		
		bitwise[hlooper + 1] = ((tx_data >> hlooper) & 1);
		
	}
	bitwise[9] = STOPBIT;
	// bitwise[10] = STOPBIT;
	send_it(&bitwise[0]);

#endif	

}






#if 1


static void send_it(const uint8_t *bit_arr){
	
	uint8_t hlooper = 0;
	

#if REDUCE_MEM_USAGE

	TMR2ON = false;
	TMR2 = 0;
	TMR2IF = false;
	
#else
	
	t2_reset();
	
#endif	
  
  TMR2ON = true;
  
#if DB_LUZ_UART
	DB_SWAP;
#endif
	
	for(hlooper = 0; hlooper < 10; hlooper++)
	{
    
		
		// this is on word shorter 
		if((*bit_arr) == true)
		{
			LUZ_ON_OFF = false;
		}
		else
		{
			LUZ_ON_OFF = true;
		}
		
	
		// LUZ_ON_OFF = !(*bit_arr);
		
		
		bit_arr++;		
    wait_for_tmr_expires(TX_LUZ_TMR_OF_CNT);

	}
	

	

	// GIE = true;

}


#else
	

// good version here...
static void send_it(const uint8_t *bit_arr){
	
	uint8_t hlooper = 0;
	

#if REDUCE_MEM_USAGE

	TMR2ON = false;
	TMR2 = 0;
	TMR2IF = false;
	
#else
	
	t2_reset();
	
#endif	
  
  TMR2ON = true;
  
#if DB_LUZ_UART
	DB_SWAP;
#endif
	
	for(hlooper = 0; hlooper < 10; hlooper++)
	{
    
		set_bb_uart(*bit_arr);
		bit_arr++;		
    wait_for_tmr_expires(TX_LUZ_TMR_OF_CNT);

	}
	

	

	// GIE = true;

}




static void set_bb_uart(uint8_t b_val){
	
	if(b_val == true)
	{
		LUZ_ON_OFF = false;
	}
	else
	{
		LUZ_ON_OFF = true;
	}

}

#endif

#endif

//   * * * * * *      I S R  - -  H A N D L E R     * * * * * * * * * * * * * *   //



// EOF