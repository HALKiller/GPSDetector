// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //

// this part is getting called after a reset and. there is a timeout timer getting configured
// to return to main if the timer expires.


// the TMR2 is configured to OF every 15ms --> therefore if we have a halfbit_cnt of 5 we wait 75ms.
// therefore a full bit has 150ms length.

//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //



#include "Global.h"
#include "rx_luz.h"

#include "UART.h"
#include "eeprom.h"

#include "timers.h"
#include "detector.h"  // for the sensor ilum
#include "gps_extensions.h" // for the buffer
#include "io_port_sfr_names.h"
#include "generic_union_flgs.h"

// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  
#ifndef FILE_RX_LUZ_DB_ENABLED
#define FILE_RX_LUZ_DB_ENABLED 0
#endif
#if FILE_RX_LUZ_DB_ENABLED
#define DB_PRINT(str) G_DB_PRINT(str)
#define UART_int(var) G_UART_INT(var)
#else
#define DB_PRINT(str)
#define UART_int(var)
#endif
// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  


//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

// if not set we compile only parts from here so that we have enough 
// memory available for debugging reasons...
#define COMPILE_FULL_PROJECT 1

#define REDUCE_MEM_USAGE 1

// #define DB_PRINT_L(str) send_bb_string((const unsigned char *)(str))
#define DB_PRINT_L(str)   // send_bb_string((const unsigned char *)(str))

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


enum {
	
	e_WRITE_ORDER	= 1,
	e_READ_ORDER,  
	e_WRITE_NUMBER,
	e_TIMED_OUT,
	
};

enum d_length{
	e_WRITE_NUMBER_LEN = 5,	// 1 orderbyte + 3 databyte + 1 chcksum
};



#if 0
enum{
  
  TMR_15ms_OF_TIME,
  TMR_1ms_OF_TIME,
  TMR_500us_OF_TIME,
  TMR_3072us_OF_TIME,
  
};

#endif

//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 


const uint8_t ADC_threshold = M_ADC_THRESHOLD; 
char us_to_send[] = M_US_TO_SEND;



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

#define TX_LUZ_TMR_OF_CNT 6	// 150ms



#define TMR1_TIMEOUT 	luz_flgs.timeout
#define AUX_FLG				luz_flgs.aux_flg
#define STARTBIT_FOUND luz_flgs.startbit


#define RX_LUZ_HALFBIT_TIME_OF_CNT 5
#define RX_LUZ_FULLBIT_TIME_OF_CNT 10
#define BITCNT 8

#define WRITE_ORDER	1
#define READ_ORDER  2
#define TIMED_OUT		3

#define STARTBIT false	
#define STOPBIT true

#define CHARS_TO_RECEIVE	35	// 34 config bytes + 1 chcksum


// #define M_RX_LUZ_INIT_TMR2_CFG 


//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //


uint8_t timeout_cnt = 0;

static uint8_t detector_number_is_valid(void);



//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //



static void rx_luz_configure_tmr2(uint8_t timeout_setter);
static uint8_t wait_for_startbyte(void);
static void Inicio_uart_luz(void);
static uint8_t rx_config_data(uint8_t order_id);
static uint8_t tx_config_data(uint8_t order_id);
static void wait_for_tmr_expires(uint8_t loopcnt);
inline static void t2_reset(void);
static uint8_t get_next_luz_char(void);
static uint8_t wait_for_startbit(void);
static uint8_t chcksum_checker(uint8_t to_loop_cnt);
static void write_rx_data_to_eeprom(void);
static void write_detector_number_data_to_eeprom(void);
static void tx_luz(uint8_t tx_data);





//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

#if COMPILE_WITH_RX_LUZ

void check_on_rx_luz(void){

#if REDUCE_MEM_USAGE
	uint8_t hlooper = 0;
#endif

	Inicio_uart_luz();

#if DB_UART_ON


  DB_PRINT_L("\r\nReset\r\n");

#endif	

  DB_PRINT_L("\r\nReset\r\n");

	tx_luz(SENSOR_LDR_IDENTIFYER);
  __delay_ms(200);
  // this is just an enum into the fuinction basically...
  tx_config_data(SEND_RX_SPEED);

	
  timers_set_tmr1_id(RX_LUZ_TIME_OUT);
  reset_timeout_timer();
  
  TMR1_IE = true;
	TMR1_ON = true;

		switch(wait_for_startbyte())
		{
			case e_WRITE_ORDER:
				if(rx_config_data(CHARS_TO_RECEIVE) == true)
				{
					if(chcksum_checker(CHARS_TO_RECEIVE) == false) 
          {
            write_rx_data_to_eeprom();

						tx_config_data(SEND_FULL_CONFIG);
#if 1            
            RESET();
#endif            
            
          }
				}
      
			break;
#if COMPILE_FULL_PROJECT	
			case e_WRITE_NUMBER:
				if(rx_config_data(e_WRITE_NUMBER_LEN - 1) == true)
				{
					if(chcksum_checker(e_WRITE_NUMBER_LEN - 1) == false)
          {

						if(detector_number_is_valid() == true)
				
						{
							
#if REDUCE_MEM_USAGE			
							// saves 3 words --> function call overhead
							for(hlooper = 0; hlooper < e_WRITE_NUMBER_LEN - 2; hlooper++)
							{
								
								write_eeprom(eeprom_addresses[hlooper + EEPROM_DETECTOR_NUMBER_LOOKUP_SLOT], sentence_buffer.gps_buffer[hlooper]);
								
							}
#else							
							
							write_detector_number_data_to_eeprom();
							
#endif
										
							tx_config_data(SEND_DETECTOR_NUMBER);
#if 1            
              RESET();
#endif                 
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

	TMR1_ON = false;
  TMR1_IE = false;

	TMR2_ON = false;

}

#else
	

// so that we are only making a short init here and than return to main
// that is for a reduction of code and therefore for debugging 
void check_on_rx_luz(void){
	
	Inicio_uart_luz();

#if DEBUGGING_IS_ON

  UART_Write_Text("\r\nReset\r\n");

#endif	
	
	LED = true;
	TMR2ON = true;
	wait_for_tmr_expires(RX_LUZ_HALFBIT_TIME_OF_CNT);	// 45ms
	LED = false;
	
	
	
}

#endif

//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //

#if 1

void rx_luz_tx_db_luz(uint8_t *data){
  
  
  rx_luz_configure_tmr2(TMR_500us_OF_TIME);
  
  while(*data != NULL_TERMINATOR)
  {
    tx_luz(*data);
    data++;
  }
  
}

#endif



static void Inicio_uart_luz(void){
  

	LCDCON = 0;


#if !REDUCE_MEM_USAGE	
	luz_flgs.reg = 0;
#endif	

	ADIE = 0;
  ADIF = 0;
	
// because we are sending now a version byte as identifyer ...

	rx_luz_configure_tmr2(TMR_1ms_OF_TIME);


}



static uint8_t wait_for_startbyte(void){
  
	uint8_t ret_value = 0u;
	uint8_t order_byte = 0u;	

#define GET_BUG_OUT 1

#if GET_BUG_OUT

#define MAX_BAD_RECEIVED_CHARS 5
  uint8_t rnd_cnt = 0u;
#endif


  // once we have received a startcondition we keep on going otherwise we might 
  // just stop when there is no TMR time left

#if 1

  rx_luz_configure_tmr2(DEFAULT_TMR2);

#else

#if SENSOR_LDR_IDENTIFYER==61||SENSOR_LDR_IDENTIFYER==0x85||SENSOR_LDR_IDENTIFYER==BIG_Z

#if (PCB_VERSION==68)||(PCB_VERSION==69)
  
  rx_luz_configure_tmr2(TMR_3072us_OF_TIME);

  
#elif PCB_VERSION==66
  

  rx_luz_configure_tmr2(TMR_15ms_OF_TIME);
  
#else
wat?
#endif  

  
#else
  
  DB_PRINT_L("\r\nBaud_15ms");
	rx_luz_configure_tmr2(TMR_15ms_OF_TIME);
  
#endif

#endif

	TMR2ON = true;

	wait_for_tmr_expires(RX_LUZ_HALFBIT_TIME_OF_CNT);

	t2_reset();

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

      t2_reset();
	
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
			
			if(ret_value != 0u)
			{

				reset_timeout_timer();

			}

		}
		else
		{
			ret_value = e_TIMED_OUT;
		}
	}
  
  return ret_value;
  
}


static uint8_t wait_for_startbit(void){
	
	// waiting for H->L transition (Dark to Light)

	while(TIMEOUT_FLG == false)
	{
    // wait for darkness...
#if INVERTED_LDR_SENSOR    
		if(read_ilum_sensor() < ADC_threshold)
#else
    if(read_ilum_sensor() > ADC_threshold)
#endif  
		{
			while(TIMEOUT_FLG == false)
			{
        // wait for light
#if INVERTED_LDR_SENSOR    
        if(read_ilum_sensor() > ADC_threshold)
#else
        if(read_ilum_sensor() < ADC_threshold)
#endif          
				// if(read_ilum_sensor() > ADC_threshold)
				{
					return true;
				}
			}
		}	
	}
  
  return false;
  
}



static uint8_t rx_config_data(uint8_t to_loop_cnt){
	// we have received a startbyte 'w' and now we are waiting for xy chars to be written inot th eEEPROM if the received 
	// chars are correct --> send with it a single chcksum byte
	uint8_t bytelooper = 0;


	for(bytelooper = 0; bytelooper < to_loop_cnt; bytelooper++)
	{
		if(wait_for_startbit() == true)
		{
      
 #if DB_LUZ_UART&&1
      UART_int(bytelooper);
 #endif
			sentence_buffer.gps_buffer[bytelooper] = get_next_luz_char();

      
		}
		else
		{
		  return false;
		}
	}
	
	return true;
	
}


static uint8_t tx_config_data(uint8_t order_id){

	uint8_t ret_value = false;
  uint8_t hlooper = 0u;
  uint8_t chcksum = 0u;
  uint8_t tx_data = 0u;
  


 // a 300ms waiter here...
	rx_luz_configure_tmr2(TMR_15ms_OF_TIME);

  TMR2ON = true;
  
	wait_for_tmr_expires(20u);

	rx_luz_configure_tmr2(TMR_1ms_OF_TIME);
  

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
				DB_PRINT_L("\r\n");
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
				DB_PRINT_L("\r\n");
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
        
        tx_data = us_to_send[hlooper];
				chcksum = chcksum ^ tx_data;
				tx_luz(tx_data);
        
      }    	

    break;
    
    
		default:
		
		break;

	}
  
	tx_luz(chcksum);
  
}


static uint8_t chcksum_checker(uint8_t to_loop_cnt){
  uint8_t ret_value = false;
  uint8_t hlooper = 0u;
  uint8_t chcksum = 0u;

  for(hlooper = 0; hlooper < to_loop_cnt; hlooper++)
  {
    
    chcksum = chcksum ^ sentence_buffer.gps_buffer[hlooper];
#if DB_LUZ_UART&&0
	UART_int(sentence_buffer.gps_buffer[hlooper]);
	UART_int(chcksum);
	DB_PRINT_L("\r\n");
#endif
  }
	
#if DB_LUZ_UART&&0

	DB_PRINT_L("chcksum: ");
	UART_int(chcksum);
	
#endif	

	return chcksum;

}


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
		temp_val = sentence_buffer.gps_buffer[hlooper] - 0x30;
		
		// and just test that it is now in the correct range...--> data validated
		if(temp_val < 10)
		{
			received_detector_number = 10 * received_detector_number + temp_val;	// (sentence_buffer.gps_buffer[hlooper] - 0x30);	
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



static void write_rx_data_to_eeprom(void){
  
  uint8_t hlooper = 0;

  
  for(hlooper = 0; hlooper < CHARS_TO_RECEIVE - 1; hlooper++)
  {
    
    write_eeprom(eeprom_addresses[hlooper], sentence_buffer.gps_buffer[hlooper]);
    
  }
  
}


#if !REDUCE_MEM_USAGE

static void write_detector_number_data_to_eeprom(void){
  
  uint8_t hlooper = 0;

  for(hlooper = 0; hlooper < e_WRITE_NUMBER_LEN - 2; hlooper++)
  {
    
    write_eeprom(eeprom_addresses[hlooper + EEPROM_DETECTOR_NUMBER_LOOKUP_SLOT], sentence_buffer.gps_buffer[hlooper]);
    
  }
  
}

#endif



static uint8_t get_next_luz_char(void){
	
	uint8_t rx_byte = 0;
	uint8_t bitlooper = 0;
	uint8_t t_val = 0;

	reset_timeout_timer();
	
	TMR2ON = true;
	
	wait_for_tmr_expires(RX_LUZ_HALFBIT_TIME_OF_CNT);

	for(bitlooper = 0; bitlooper < BITCNT; bitlooper++)
	{		
    

		wait_for_tmr_expires(RX_LUZ_FULLBIT_TIME_OF_CNT);
    
    DB_LED1_SWAP;
		
    t_val =  read_ilum_sensor();

#if INVERTED_LDR_SENSOR	
		if(t_val > ADC_threshold)
#else
		if(t_val < ADC_threshold)
#endif

		{
			rx_byte = rx_byte >> 1;
		}
		else
		{
			rx_byte = (rx_byte >> 1) + 128; 
		}
    
#if DB_LUZ_UART&&1	
		UART_int(t_val);
    UART_CRLF;
#endif		

	}
  
#if DB_LUZ_UART&&1

	DB_PRINT_L("Byte: ");
	UART_int(rx_byte);
	UART_CRLF;
  
#endif	

	return rx_byte;
	
}




static void wait_for_tmr_expires(uint8_t loopcnt){
	
	uint8_t hlooper = loopcnt;

	while(hlooper > 0)
	{
		while(TMR2IF == false);
		hlooper--;
		TMR2IF = false;
	}

}


inline static void t2_reset(void){
	
	TMR2ON = false;
	TMR2 = 0;
	TMR2IF = false;
	
}


static void rx_luz_configure_tmr2(uint8_t timeout_setter){
  
  t2_reset();
  
  
  switch(timeout_setter)
  {
    case TMR_15ms_OF_TIME:
      
      // 15ms
      TMR2_PRESCALER = TMR2_15MS_PRE;
      TMR2_POSTSCALER = TMR2_15MS_POST;
      PR2 = TMR2_15MS_PR;
      
    break;
    
    case TMR_1ms_OF_TIME:
      
      // 1ms
      TMR2_PRESCALER = TMR2_1MS_PRE;
      TMR2_POSTSCALER = TMR2_1MS_POST;
      PR2 = TMR2_1MS_PR;
      
    break;
    case TMR_500us_OF_TIME:
#if 1
      TMR2_PRESCALER = TMR2_500US_PRE;
      PR2 = TMR2_500US_PR;  // 125;
      TMR2_POSTSCALER = TMR2_500US_POST;  // TMR2_01_POSTSCALER;    
    
#else
  
      TMR2_PRESCALER = TMR2_01_PRESCALER;
      PR2 = 250;
      TMR2_POSTSCALER = TMR2_02_POSTSCALER;
      
#endif      
    break;

    case TMR_3072us_OF_TIME:
    
      TMR2_PRESCALER = TMR2_3072US_PRE;
      PR2 = TMR2_3072US_PR;
      TMR2_POSTSCALER = TMR2_3072US_POST;
    
    break;
    
    default:
    break;
    
  }

}


static void tx_luz(uint8_t tx_data){

	#define BITS_TO_SEND 10
	
	
	uint8_t hlooper = 0;

	TMR2ON = false;
	TMR2 = 0;
	TMR2IF = false;
	
	TMR2ON = true;
	

	// bitwise[0] = STARTBIT; equals...
	LED = !(STARTBIT);
	
  wait_for_tmr_expires(TX_LUZ_TMR_OF_CNT);
	
	for(hlooper = 0; hlooper < 8; hlooper++)
	{
		
		// bitwise[hlooper + 1] = ((tx_data >> hlooper) & 1);
		LED = !((tx_data >> hlooper) & 1);
		wait_for_tmr_expires(TX_LUZ_TMR_OF_CNT);
		
	}
	LED = !STOPBIT;
	wait_for_tmr_expires(TX_LUZ_TMR_OF_CNT);
	
	
}



//   * * * * * *      I S R  - -  H A N D L E R     * * * * * * * * * * * * * *   //



// EOF