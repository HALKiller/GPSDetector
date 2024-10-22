// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //

// this part is getting called after a reset and. there is a timeout timer getting configured
// to return to main if the timer expires.

// Overworking 26072023 --> to be able to get the best out of the changed sensor we can test agaisnt what sensor we are haviong on the GPS side-->
// therefore if there is a LDR we receive as startbyte a different character than when we are with a Phototransistor. we can increase the sending 
// speed and also the reception speed immensely

// Overworking 30102023 --> becasue we have started to work allready on a new GPSDetector which is going to have a fototrasnistor as sensor we can 
// also increase the write speed.


//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //




#include "rx_luz.h"

#include "UART.h"
#include "ADC.h"
#include "io_port_sfr_names.h"
#include "checksumming.h"
#include "btn_number_setter.h"
#include "leds.h"

#include "device_driver_config.h"



#include "Global.h"


#include <stdint.h>

//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //
union udt_flags{
	uint8_t reg;
	
	struct {
		unsigned timeout				: 1;
		unsigned wait_for_order	: 1;
		unsigned aux_flg				: 1;
		unsigned startbit				: 1;
    unsigned wait_for_startcondition :1;
    unsigned order_was_for_write_config :1;
	};
	
	
	
};

union udt_flags luz_flgs;

enum {
	e_READORDER,
	e_WRITE_CONFIG_ORDER,
	e_WRITE_NUMBER,
};

// the base here is the tx_byte_count...
enum d_length{
	e_READORDER_LEN = 1,	// a single byte
	e_WRITE_CONFIG_ORDER_LEN = CHARS_TO_RECEIVE + 1,	// 1 orderbyte + 34 databyte + 1 chcksum
	e_WRITE_NUMBER_LEN = 5,	// 1 orderbyte + 3 databyte + 1 chcksum
};

enum {
  
  E_COM_0_VERSION,  // 16F688 & 16F1936 --> read slow, write slow,
  E_COM_1_VERSION,  // 16F1936 with LDR Sensor --> read slow, writes fast back
  E_COM_2_VERSION,  // The Renesas with fototransistor read fast, write fast
  E_COM_3_VERSION,  // For APDS6009 Fototransistor
  E_COM_4_VERSION,
  E_NUM_VERSION
  
};


enum{
  
  TMR_15ms_OF_TIME,
  TMR_1ms_OF_TIME,
  TMR_600us_OF_TIME,
  BAUD_65_RATE,
  BAUD_HALF_65_RATE,
  
};


//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 
#if USE_REAL_PCB 
const uint8_t ADC_threshold = 45;  
#else
const uint8_t ADC_threshold = 15;  
#endif

const uint8_t eeprom_addresses[] =
  { 0x22,	0x23,	0x24,	0x25,	0x27,
    0x28,	0x29,	0x2A,	0x2C,	0x2D,	0x2E,	0x2F,
    0x31,	0x32,	0x33,	0x34,	0x36,	0x37,
    0x38,	0x39,	0x3A,	0x3B,	0x3C,	0x3D,	0x3E,	0x3F,
    0x40,	0x42,	0x43,	0x44,	0x45,	0x46,	0x47,
    0x48 };

//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 
#define INJECT_ERROR 0 
 
 // these two are not used anymore but i keep them because they might come in handy again...
// #define RX_LUZ_HALFBIT_TIME 1	// that is 25ms OF_time
// #define TX_LUZ_HALFBIT_TIME 2	// that is ...15ms(?) OF time

// the Time base is at the moment 15ms TMR2_OF but --> that is only because the LDR is so slow-->

#define TX_LUZ_TMR_OF_CNT 10



#define TMR1_TIMEOUT 	luz_flgs.timeout
#define AUX_FLG				luz_flgs.aux_flg
#define STARTBIT_FOUND luz_flgs.startbit
#define WAITING_FOR_GPS_STARTCONDITION luz_flgs.wait_for_startcondition
#define ORDER_WAS_WRITE_CONFIG luz_flgs.order_was_for_write_config

#define DEBUG_IMAN_ACTIVATION 0

#if MIPS==1

#define TMR1_HALF_SECOND_OF_CNT 1
#define TMR1_2_SECOND_OF_CNT 4	// 4 = 2seconds, 8 = 4seconds
#define TMR1_GPS_ANSWER_TIME_OF_CNT 15	// 4 = 2seconds, 8 = 4seconds

#elif MIPS==8

#define TMR1_HALF_SECOND_OF_CNT 8
#define TMR1_2_SECOND_OF_CNT 32	
#define TMR1_GPS_ANSWER_TIME_OF_CNT 120	

#else
MISSING
#endif

// that needs to be synced to the GPS_detector OF_time on tx there --> 
// -->set to 15ms OF time and that for each bit 
#define RX_LUZ_HALFBIT_TIME_OF_CNT 3
#define RX_LUZ_FULLBIT_TIME_OF_CNT 6	// OF = 15ms --> 6x15 = 90ms per bit

#define BITCNT 8


#define STARTBIT false	
#define STOPBIT true

#define ERROR_IDENTIFYER_FROM_DETECTOR 'e'
#define MAX_DETECTORES_ANSWER 61	// 60 but to compare against smaller...
#define MIN_DETECTORES_ANSWER 0



#define RET_VAL_ALL_GOOD 0
#define RET_VAL_G_BYTECNT_B_CHCKSUM 1
#define RET_VAL_TIMEOUT 2
#define RET_VAL_SENT_RECEIVED_CONFIG_DIFFERS	3
#define RET_VAL_MAX_DETECTORES 4
#define RET_VAL_OTHER_ERROR 5


#define FIRST_RELEASED_VERSION_RX_BYTE 4 // because of 45ms flash on the first version --> That never got considered at the time to be an identifyer of version
#define SECOND_RELEASED_VERSION_RX_BYTE 49  // '1' makes a lot of sense to me...
#define THIRD_RELEASED_VERSION_RX_BYTE  57  // '9' just to have something there...That is going to be with RA2L1 chip onwards and fototransistor on the pcb
#define APDS_SENSOR_RELEASED_VERSION_RX_BYTE 61


#define FUTURE_VERSION_ID 0x5A  // aka 'BIG_Z' 'Z'


#define TX_SET 1
#define RX_SET 0

#define RECEIVE_BITTIME_BYTE_CNT 6



//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //

uint8_t timeout_cnt = 0;

uint8_t config_bytes[CHARS_TO_RECEIVE];

uint8_t speed_cfg[RECEIVE_BITTIME_BYTE_CNT];

uint8_t the_order = 0;


// this variable holds the version of communication of the detector --> 
// therefore we receive a char from the detectior and based on this
 // value we can decode what kind of communication link we can establish with the detector
uint8_t detector_com_version = 0; 



uint32_t timer_ticks = 1;
uint8_t prescaler_set = 1;
uint8_t postscaler_set = 1;
uint8_t counter_set = 10;



//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

static void configure_tmr2(uint8_t const timeout_setter);
// static uint8_t wait_for_startbyte(void);
// static void Inicio_uart_luz(void);
static void reset_timeout_timer(void);
static uint8_t rx_config_data(void);
static uint8_t tx_config_data(void);
static uint8_t timeout_checker(void);
static void wait_for_tmr_expires(uint8_t loopcnt);
inline static void t2_reset(void);
static uint8_t get_next_luz_char(void);
static uint8_t wait_for_startbit(void);
static uint8_t chcksum_checker(void);
static uint8_t test_rx_for_error_message(void);

static void send_it(const uint8_t *bit_arr);
static void set_bb_uart(uint8_t b_val);
static uint8_t gps_startcondition_is_good(void);
static void set_timeout_tmr(void);
static uint8_t compare_sent_config_data_matches_received_config_data(void);
static uint8_t test_rx_number_for_compatibility(void);
static void configure_tmr_to_version(uint8_t version);
static void set_version_from_version_byte(uint8_t version_byte);
static uint8_t rx_com_speed_data(void);
uint8_t find_timer_settings(void);
uint8_t calculate_bit_time_from_speed_com_data(void); 

#if 0
static void charge_iman(void);
#endif

//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //



// implementing a var. to indicate what kind of order we have sent and 
// therefore we are awaiting a different response depending on the order type
uint8_t check_on_rx_luz(void){

	uint8_t ret_value = 0;
  uint8_t chcksum_len = 0;
	uint8_t *d_pnt;
	uint8_t err_msg = 0;
	// State Machine should have these states:
	// waiting for order from PC	--> not accepting anything else
	// waiting for RTR signal from GPS
	// transmitting towards GPS
	// waiting for an answer
	// transmitting answer towards PC
	

  // look at the time necessary to 
  // give here because of faster reception
	reset_timeout_timer();
	

	switch (the_order)
	{
		case e_READORDER:
			chcksum_len = CHARS_TO_RECEIVE;
		break;
		case e_WRITE_CONFIG_ORDER:
			chcksum_len = CHARS_TO_RECEIVE;
		break;
		case e_WRITE_NUMBER:
			chcksum_len = e_WRITE_NUMBER_LEN - 1;
		break;
		default:
			// errhandler !!
			return false;			
		break;
	}

#if DB_LUZ_UART&&0
UWT("Rx_LUZ_ON\r\n");
#endif	


	TMR1ON = true;

	

	
	if(rx_config_data() == true)
	{
		// we received the right amount of readback data
		if(checksumming_chcksum_checker(&config_bytes[0], chcksum_len) == false)
		// if(chcksum_checker() == false)  
		{
#if 1			
			ret_value = RET_VAL_SENT_RECEIVED_CONFIG_DIFFERS;
#endif			
			if((e_WRITE_CONFIG_ORDER == the_order) || (e_WRITE_NUMBER == the_order))
			// if(ORDER_WAS_WRITE_CONFIG == true)
			{
#if INJECT_ERROR
UWT("ORDER WAS_TRUE");
#endif          
				
				if(compare_sent_config_data_matches_received_config_data() == true)
				{
					ret_value = RET_VAL_ALL_GOOD;
				}
				else
				{
					// test if we can extract an err. message from the received answer....
					err_msg = test_rx_for_error_message();
					switch(err_msg)
					{
						case 0:
							ret_value = RET_VAL_SENT_RECEIVED_CONFIG_DIFFERS;
						break;
						case 1:
							ret_value = RET_VAL_OTHER_ERROR;
						break;
						default:
							ret_value = RET_VAL_MAX_DETECTORES;
						break;
					}
					
				}
			}
			else
			{

#if INJECT_ERROR
UWT("ORDER WAS_FALSE");
#endif  
				ret_value = RET_VAL_ALL_GOOD;
			}
			

		}
		else
		{
			// indicate that something went wrong --> Bad chcksum
			ret_value = RET_VAL_G_BYTECNT_B_CHCKSUM;
			
		}
	}
	else
	{
		// a TIME out ocurred
		ret_value = RET_VAL_TIMEOUT;
		
	}


	
  if(TMR1_TIMEOUT == true)
  {
    ret_value = RET_VAL_TIMEOUT;
  }
	
	TMR1_ON = false;
	TMR2_ON = false;
	
  return ret_value;
  
  
}




//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //




static uint8_t wait_for_startbit(void){
	
	uint8_t ret_value = 0;
	
	// waiting for H->L transition (Dark to Light)
	AUX_FLG = false;

	while((AUX_FLG == false) && (TMR1_TIMEOUT == false))
	{
		adc_samples_channel(LDR_ANALOG_CHANNEL);

		if(ADRESH > ADC_threshold)
		{		
			AUX_FLG = true;		
		}
		else
		{
			if(timeout_checker() == true)
			{
				TMR1_TIMEOUT = true;
			}
		}
	}
	
	
	if(AUX_FLG == true)
	{
	
		AUX_FLG = false;
		
		while((AUX_FLG == false) && (TMR1_TIMEOUT == false))
		{		
			adc_samples_channel(LDR_ANALOG_CHANNEL);

			if(ADRESH < ADC_threshold)
			{
				AUX_FLG = true;
				ret_value = true;
			}
			else
			{
				if(timeout_checker() == true)
				{
					TMR1_TIMEOUT = true;
				}
			}
		}
	}
	
	return ret_value;
	
}


// we are receiving here the answer from the detector -->
// therefore than can be that we get the full config_bytes// or just a parcial 
static uint8_t rx_config_data(void){

	uint8_t bytelooper = 0;
	uint8_t ret_value = true;
	uint8_t to_loop_to = 0;
	
	
#if LANGUAGE_SPANISH
	UWT("\r\nRECEPCION: ");
	UART_CRLF;
	UART_CRLF;
#else    
	UWT("\r\nRECEIVE: ");
	UART_CRLF;
	UART_CRLF;
#endif
  


  configure_tmr_to_version(RX_SET);


  
	switch (the_order)
	{
		case e_READORDER:
			to_loop_to = CHARS_TO_RECEIVE;
		break;
		case e_WRITE_CONFIG_ORDER:
			to_loop_to = CHARS_TO_RECEIVE;
		break;
		case e_WRITE_NUMBER:
			to_loop_to = e_WRITE_NUMBER_LEN - 1;
		break;
		default:
			// errhandler !!
			return false;			
		break;
	}
	
	for(bytelooper = 0; bytelooper < to_loop_to; bytelooper++)
	{

		if(wait_for_startbit() == true)
		{

			config_bytes[bytelooper] = get_next_luz_char();
      
#if INJECT_ERROR
      if(bytelooper == CHARS_TO_RECEIVE - 2)
      {
        config_bytes[bytelooper] = 0xFF;
      }
#endif      
      
      
 // #if DB_LUZ_UART
  UART_int(bytelooper);
  UART_int(config_bytes[bytelooper]);
  UART_CRLF
	
// #endif	      
		}
		else
		{
			// this is timeout
#if LANGUAGE_SPANISH
			UWT("TMR overflow recepcion byte: ");
			UART_int(bytelooper);
			UART_CRLF;
#else
			UWT("TMR of on byte: ");
			UART_int(bytelooper);
			UART_CRLF;
#endif			
			bytelooper = CHARS_TO_RECEIVE;
			ret_value = false;
		}

	}
	
#if 0	
	// --> reset the first few bytes in the received config_bytes 
	// from the detector to avoid to using an old received value
	 // --> but not here because i will have an chcksum checking afterwards
	 // and if i set htese values allready here the thing has to fail of course...
	config_bytes[0] = 0xFF;
	config_bytes[1] = 0xFF;
	config_bytes[2] = 0xFF;
	config_bytes[3] = 0xFF;
#endif	
	return ret_value;
	
}




static uint8_t chcksum_checker(void){
  uint8_t ret_value = false;
  uint8_t hlooper = 0;
  uint8_t chcksum = 0;
 
 #if DB_LUZ_UART
	UWT("Chcksum readout:\r\n");
#endif	
	
  for(hlooper = 0; hlooper < CHARS_TO_RECEIVE; hlooper++)
  {
    
    chcksum = chcksum ^ config_bytes[hlooper];
#if DB_LUZ_UART		
    UART_int(config_bytes[hlooper]);
#endif		
		
  }
#if INJECT_ERROR

	UWT("chcksum: ");
	UART_int(chcksum);

	return false;
	
#else		
	
	return chcksum;
#endif	
}


static uint8_t compare_sent_config_data_matches_received_config_data(void){
  uint8_t *config_pnt;
  uint8_t *tx_data_pnt;
  uint8_t ret_value = true;
  uint8_t hlooper = 0;
	uint8_t to_loop_to = 0;
	
	
	
  tx_data_pnt = get_pnt_to_uart_rx_buffer();
	
  // becasue in the first slot is the order byte --> w or r or n
  tx_data_pnt++;  
  config_pnt = &config_bytes[0];
 
 
 
#if INJECT_ERROR
  UWT("\r\nComparer:\r\n");

#endif



	switch (the_order)
	{
		case e_WRITE_CONFIG_ORDER:
			to_loop_to = CHARS_TO_RECEIVE - 1;
		break;
		case e_WRITE_NUMBER:
			to_loop_to = e_WRITE_NUMBER_LEN - 2;
		break;
		default:
			// errhandler !!
			return false;			
		break;
	}
 
	for(hlooper = 0; hlooper < to_loop_to; hlooper++)
	{

		if(*tx_data_pnt != *config_pnt)
		{
			ret_value = false; 
			hlooper = to_loop_to;    
		}
		tx_data_pnt++;
		config_pnt++;
	}

	return ret_value;
  
}




// if there was some kind of problem on the reception we 
// take a look if there might have been an answer
// with an error information

static uint8_t test_rx_for_error_message(void){
	
	uint8_t ret_value = false;
	
	if(ERROR_IDENTIFYER_FROM_DETECTOR == config_bytes[0])
	{

		if(test_rx_number_for_compatibility() == true)
		{
			btn_number_setter_set_new_maximum_detectors(config_bytes[1]);
			ret_value = config_bytes[1];
		}
		else
		{
			ret_value = true;
		}
	}

}

static uint8_t test_rx_number_for_compatibility(void){
	
	uint8_t ret_value = false;
	
	if((MAX_DETECTORES_ANSWER > config_bytes[1]) && (MIN_DETECTORES_ANSWER < config_bytes[1]))
	{
		ret_value = true;
	}
	
	return ret_value;
	
	
}


static uint8_t get_next_luz_char(void){
	
	uint8_t rx_byte = 0;
	uint8_t bitlooper = 0;
	
#if DB_LUZ_UART&&0
	UWT("ADC: ");
	
#endif	
	reset_timeout_timer();
	
	TMR2ON = true;
	
	wait_for_tmr_expires(RX_LUZ_HALFBIT_TIME_OF_CNT);
	
	for(bitlooper = 0; bitlooper < BITCNT; bitlooper++)
	{		
		wait_for_tmr_expires(RX_LUZ_FULLBIT_TIME_OF_CNT);
	DB_SWAP;  // 3. --> each char
		adc_samples_channel(LDR_ANALOG_CHANNEL);
		
		
#if DB_LUZ_UART&&0
	UART_int(ADRESH);
#endif			
		if(ADRESH < ADC_threshold)
		{
			rx_byte = rx_byte >> 1;
		}
		else
		{
			rx_byte = (rx_byte >> 1) + 128; 
		}

	}
#if DB_LUZ_UART&&0

	UWT("Byte: ");
	UART_int(rx_byte);
	UART_CRLF;
#endif	
	return rx_byte;
	
}


static uint8_t timeout_checker(void){
	
	uint8_t ret_value = 0;
	
	if(TMR1IF == true)
	{
		timeout_cnt--;
		TMR1IF = false;
#if DB_LUZ_UART&&0
		if(WAITING_FOR_GPS_STARTCONDITION == true)
		{
			UWT("\r\nTimeout en: ");   
			UART_int(timeout_cnt);
			UART_CRLF;
		}
#endif
	}
	if(timeout_cnt == 0)
	{	
		ret_value = true;
	}
	return ret_value;
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


static void reset_timeout_timer(void){
	
	TMR1_TIMEOUT = false;
	
	timeout_cnt = TMR1_2_SECOND_OF_CNT;
	
  if(WAITING_FOR_GPS_STARTCONDITION == true)
  {
    timeout_cnt = TMR1_GPS_ANSWER_TIME_OF_CNT;
  }
  
	TMR1IF = false;
	TMR1H = 0;
	TMR1L = 0;
	
}

static void set_timeout_tmr(void){
	
	
	TMR1ON = false;
	TMR1IF = false;
	TMR1H = 0;
	TMR1L = 0;
	
	
}

inline static void t2_reset(void){
	
	TMR2ON = false;
	TMR2 = 0;
	TMR2IF = false;
	
}



  
	
uint8_t send_string_to_gps_detector(void){
  
  uint8_t *data_pnt = NULL;
  uint8_t tx_cnt = 0;
  uint8_t ret_value = false;
  uint8_t to_loop_to = CHARS_TO_RECEIVE + 1;
  uint8_t hlooper = 0;  
  uint8_t tx_data = 0;
  
	
	data_pnt = get_pnt_to_uart_rx_buffer();
   
   
#if 0

  charge_iman();
 
#endif  

 
  if(gps_startcondition_is_good() == true)
  {
		leds_update_shadow_led(e_LED_STATE, e_LC_MAGENTA);
    // presetting it and therefore avoiding an else statement
    ORDER_WAS_WRITE_CONFIG = true;
#if LANGUAGE_SPANISH
		UWT("\r\TRANSMITIENDO:\r\n\n");
#else    
    UWT("\r\nTRANSMITTING:\r\n\n");
#endif
    
    if((*data_pnt) == 'r')
    {
      ORDER_WAS_WRITE_CONFIG = false;
			the_order = e_READORDER;
      to_loop_to = e_READORDER_LEN;
    }
		else if((*data_pnt) == 'n')
    {
      ORDER_WAS_WRITE_CONFIG = false;
			the_order = e_WRITE_NUMBER;
      to_loop_to = e_WRITE_NUMBER_LEN;
    }
		else
		{
			the_order = e_WRITE_CONFIG_ORDER;
			to_loop_to = e_WRITE_CONFIG_ORDER_LEN;
		}
			
		
		
		
    while(tx_cnt <  to_loop_to)
    {
#if 1    
			UART_int(tx_cnt);
			UART_int(*data_pnt);
			UWT("\r\n");
#endif     
      tx_luz(*data_pnt);    
      data_pnt++;
      tx_cnt++;
      
      // chcksum = chcksum ^ tx_data;
      
    }
		#if 0
    if(to_loop_to == CHARS_TO_RECEIVE)
    {
      tx_luz(chcksum); 
    }
    #endif
    
    ret_value = true;

  }
  
#if 0  
  IO_Clear_channel(IO_ACTIVATE_IMAN);
#endif
  
  return ret_value;

}




void tx_luz(uint8_t tx_data){
	
	uint8_t the_bit = false;
	uint8_t hlooper = 0;
	uint8_t bitwise[10];
	

  
#if FAST_PROGRAMMER

  T2_PRESCALER = TMR2_64_PRESCALER;
	T2_POSTSCALER = TMR2_15_POSTSCALER;
	PR2 = 125;	
  
#endif
	
  bitwise[0] = STARTBIT;
	
	for(hlooper = 0; hlooper < 8; hlooper++)
	{
		
		bitwise[hlooper + 1] = ((tx_data >> hlooper) & 1);
		
	}
  
	bitwise[9] = STOPBIT;
  
	send_it(&bitwise[0]);


}


#if 1


static uint8_t gps_startcondition_is_good(void){
 
 // basically we are waiting for a H --> L transition...
 // and therefore we are waiting actually 
 // for a startbit and use therefroe the existing function
 uint8_t ret_value = false;
 uint8_t version_byte = 0;
	// we use that to get a longer OF_cnt...
	WAITING_FOR_GPS_STARTCONDITION = true;

#if LANGUAGE_SPANISH
	UWT("\r\nEsperar condicion de inicio desde detector. . .\r\n\n");
#else
	UWT("\r\nWait Startcondition . . .\r\n\n");
#endif


  // because inicially all detectors get checked with this speed...
  // because only after that startbyte we know with whom we are communicating
  configure_tmr2(TMR_1ms_OF_TIME);

	reset_timeout_timer();
  
	TMR1ON = true;
  
#if 0  
  IO_Set_channel(IO_ACTIVATE_IMAN);
#endif  


  if(wait_for_startbit() == true)
  {
#if DB_LUZ_UART&&0
  UWT("Startbit found\r\n");
#endif

		version_byte = get_next_luz_char();
#if DEBUGGING_IS_ON     
    UWT("\r\nVersion byte: ");
    UART_int(version_byte);
#endif    
    set_version_from_version_byte(version_byte);
    DB_SWAP;  // 1. 
#if 1		
    // added an earlier change for the timeout...
    WAITING_FOR_GPS_STARTCONDITION = false;   
    reset_timeout_timer();
  
#endif
  
// and if the version byte is now the new FUTURE_ID--->
// than we change the complete setup here in this way:                     

    if(version_byte == FUTURE_VERSION_ID)
    {
      // now we are waiting for another six bytes...
      if(rx_com_speed_data() == false)
      {
        // BAD Rececpcion happened!
        return false;
      }
      else
      {
        if(calculate_bit_time_from_speed_com_data() == false)
        {
          return false;
        }
        // correct recepcion --> decode the timer stuff!
        if(find_timer_settings() == true)
        {
          T2_PRESCALER = prescaler_set;
          T2_POSTSCALER = postscaler_set;
          PR2 = counter_set;
        }
        else
        {
          UWT("Bad Timer settings from reception\r\n");
          return false;
        }
        
      }
    }
    else
    {
      
      
      configure_tmr_to_version(TX_SET);
      
    }


		
    // yep high low transition ocurred...or in the newer models the starting bytes got sent...
    // Therefore we need a short loop to give the GPS 
    // now time to get to its rx_position in code...
    reset_timeout_timer();
    
    timeout_cnt = TMR1_HALF_SECOND_OF_CNT;	// 1;  // that should than be 500ms
		
    while(timeout_checker() == false);
      
    ret_value = true;
    
  }
 
  WAITING_FOR_GPS_STARTCONDITION = false;
  
  return ret_value;
  
}





#else



static uint8_t gps_startcondition_is_good(void){
 
 // basically we are waiting for a H --> L transition...
 // and therefore we are waiting actually 
 // for a startbit and use therefroe the existing function
 uint8_t ret_value = false;
 
	// we use that to get a longer OF_cnt...
	WAITING_FOR_GPS_STARTCONDITION = true;

#if LANGUAGE_SPANISH
	UWT("\r\nEsperar condicion de inicio desde detector. . .\r\n\n");
#else
	UWT("\r\nAwait Startcondition . . .\r\n\n");
#endif


	reset_timeout_timer();
  
	TMR1ON = true;

  

  if(wait_for_startbit() == true)
  {
#if DB_LUZ_UART&&0
  UWT("Startbit found\r\n");
#endif
    // yep high low transition ocurred...
    // Therefore we need a short loop to gove the GPS 
    // now time to get to its rx_position in code...
    reset_timeout_timer();
    
    timeout_cnt = TMR1_HALF_SECOND_OF_CNT;	// 1;  // that should than be 500ms
		
    while(timeout_checker() == false);
      
    ret_value = true;
    
  }
 
  WAITING_FOR_GPS_STARTCONDITION = false;
  
  return ret_value;
  
}

#endif



// the newer detectores send the bittime in us towards the GdL and therefore
// once we are here we should at max the whole thing ready in 2 seconds...
// we can ajust the tmr overflow times in the way
static uint8_t rx_com_speed_data(void){

	uint8_t bytelooper = 0;
	uint8_t ret_value = true;
	uint8_t to_loop_to = RECEIVE_BITTIME_BYTE_CNT;
	
	
#if LANGUAGE_SPANISH
	UWT("\r\nRECEPCION: ");
	UART_CRLF;
	UART_CRLF;
#else    
	UWT("\r\nRECEIVE: ");
	UART_CRLF;
	UART_CRLF;
#endif
  


  configure_tmr_to_version(RX_SET);

	for(bytelooper = 0; bytelooper < to_loop_to; bytelooper++)
	{

		if(wait_for_startbit() == true)
		{
DB_SWAP;  // 2.
			speed_cfg[bytelooper] = get_next_luz_char();
      
#if INJECT_ERROR
      if(bytelooper == CHARS_TO_RECEIVE - 2)
      {
        speed_cfg[bytelooper] = 0xFF;
      }
#endif      
      
      

  // UART_int(bytelooper);
  // UART_int(speed_cfg[bytelooper]);
  // UART_CRLF
     
		}
		else
		{
			// this is timeout
#if LANGUAGE_SPANISH
			UWT("TMR overflow recepcion speed data byte: ");
			UART_int(bytelooper);
			UART_CRLF;
#else
			UWT("TMR of on recepcion speed byte: ");
			UART_int(bytelooper);
			UART_CRLF;
#endif			
			bytelooper = CHARS_TO_RECEIVE;
			ret_value = false;
		}

	}
	
#if 0	
	//  --> reset the first few bytes in the received config_bytes 
	// from the detector to avoid to using an old received value
	 // --> but not here because i will have an chcksum checking afterwards
	 // and if i set htese values allready here the thing has to fail of course...
	speed_cfg[0] = 0xFF;
	speed_cfg[1] = 0xFF;
	speed_cfg[2] = 0xFF;
	speed_cfg[3] = 0xFF;
#endif	
	return ret_value;
	
}

uint8_t calculate_bit_time_from_speed_com_data(void){
  
  uint8_t hlooper = 0;
  uint32_t tick_multiplier = 100000; // we start with that and than divie every time by 10
  const uint8_t divider = 10;
  uint32_t ticker = 0;
  uint8_t number = 0;
  
  
  UWT("\r\nB_Time: ");
  
  
  for(hlooper = 0; hlooper < RECEIVE_BITTIME_BYTE_CNT; hlooper++)
  {
    
    if( (speed_cfg[hlooper] > 0x2F) && (speed_cfg[hlooper] < 0x3A) )
    {
      number = speed_cfg[hlooper] - 0x30;
      UART_int(number);
    }
    else
    {
      UWT("\r\nBad number!\r\n");
      return false;
    }
    
    ticker = ticker + (number * tick_multiplier);
    tick_multiplier = tick_multiplier / divider;

  }
  // now we have the actual us in the ticker --> but we need clock ticks!!
  // therefore -->
  timer_ticks = (ticker * MIPS) / TX_LUZ_TMR_OF_CNT;
  UWT("\r\nTicker: ");
  UART_int(ticker);
  UWT("\r\nT_Ticks: ");
  UART_int(timer_ticks);
  
  return true;
  
  
}
uint8_t find_timer_settings(void) {
  // Define the possible values for prescaler
  uint8_t prescalers[] = {1, 4, 16, 64};
  uint8_t prescaler_setting[] = {0, 1, 2, 3};
  uint8_t num_prescalers = sizeof(prescalers) / sizeof(prescalers[0]);
  uint8_t current_prescaler = 1;
  uint8_t current_postscaler = 1;
  uint32_t temp_counter = 1;
  uint8_t prescaler_looper = 0;
  uint8_t postscaler_looper = 0;
  
  // Iterate over each possible prescaler value
  for (prescaler_looper = 0; prescaler_looper < num_prescalers; prescaler_looper++) 
  {
    current_prescaler = prescalers[prescaler_looper];

    // Iterate over each possible postscaler value
    for (postscaler_looper = 1; postscaler_looper <= 16; postscaler_looper++) 
    {
      current_postscaler = postscaler_looper;

      // Calculate the counter value
      temp_counter = timer_ticks / (current_prescaler * current_postscaler);

      // Check if the counter value is within the valid range (1 to 255)
      if ( ((timer_ticks % (current_prescaler * current_postscaler)) == 0)
        && ((temp_counter >= 1) && (temp_counter <= 255)) )
      {
        
        prescaler_set = prescaler_looper; // prescaler_setting[prescaler_looper];
        postscaler_set = current_postscaler - 1;  // becasue of 0 Base in the register
        counter_set = temp_counter;
        
        
#if DEBUGGING_IS_ON       
        // UWT("\r\nTMR_calc: ");
        UWT("\r\nPSA: ");
        UART_int(prescaler_set);
        UWT("\r\nPOST: ");
        UART_int(postscaler_set);
        UWT("\r\nPR: ");
        UART_int(counter_set);
#endif        
        return 1; // Found valid settings
        
      }
    }
  }

  // If no valid combination is found, return 0
  return 0;
  
}

static void set_version_from_version_byte(uint8_t version_byte){


  if(version_byte < FIRST_RELEASED_VERSION_RX_BYTE)
  {
    detector_com_version = E_COM_0_VERSION;
  }
  else
  {
    switch(version_byte)
    {
      case SECOND_RELEASED_VERSION_RX_BYTE:
        detector_com_version = E_COM_1_VERSION;
      break;
      case THIRD_RELEASED_VERSION_RX_BYTE:
        detector_com_version = E_COM_2_VERSION;
      break;
      case APDS_SENSOR_RELEASED_VERSION_RX_BYTE:
        detector_com_version = E_COM_3_VERSION;
      break;
      case FUTURE_VERSION_ID:
        detector_com_version = E_COM_4_VERSION;
      break;


    }
  }

  
}

static void configure_tmr_to_version(uint8_t rx_tx_setter){

#if 0	
  UWT("cttv: ");
  UART_int(detector_com_version);
#endif
  
	if(rx_tx_setter == TX_SET)
  {
    switch(detector_com_version)
    {
      
      case E_COM_0_VERSION:
      case E_COM_1_VERSION:
        configure_tmr2(TMR_15ms_OF_TIME);
      break;
      case E_COM_2_VERSION:
        configure_tmr2(TMR_600us_OF_TIME);
      break;
      case E_COM_3_VERSION:
      
        configure_tmr2(BAUD_HALF_65_RATE);
        // configure_tmr2(BAUD_65_RATE);
      break;
      default:
      UWT("Errx");
        // TODO: errhandler
      break;
      
    }
  }
  else
  {
    switch(detector_com_version)
    {
      case E_COM_0_VERSION:
        configure_tmr2(TMR_15ms_OF_TIME);
      break;
      case E_COM_1_VERSION:
      case E_COM_2_VERSION:
      case E_COM_3_VERSION:
      case E_COM_4_VERSION:
        configure_tmr2(TMR_1ms_OF_TIME);
      break;
      default:
      UWT("Erry");
        //  errhandler
      break;
      
    }
  }

}

static void configure_tmr2(uint8_t timeout_setter){
  
  t2_reset();
  
  UWT("T2: ");
  UART_int(timeout_setter);
  UART_CRLF;
  
  switch(timeout_setter)
  {
    case TMR_15ms_OF_TIME:
    
      // 15ms
      T2_POSTSCALER = TMR2_15_POSTSCALER;
      T2_PRESCALER = TMR2_64_PRESCALER;
      PR2 = 125;

    break;
    
    case TMR_1ms_OF_TIME:
      
      // 1ms
      T2_POSTSCALER = TMR2_01_POSTSCALER;
      T2_PRESCALER = TMR2_64_PRESCALER;
      PR2 = 125;

    break;
    case TMR_600us_OF_TIME:
      
      // 600us
      T2_POSTSCALER = TMR2_01_POSTSCALER;
      T2_PRESCALER = TMR2_64_PRESCALER;
      PR2 = 75;

    break;
    case BAUD_65_RATE:
      // 2560us
      T2_POSTSCALER = TMR2_01_POSTSCALER;
      T2_PRESCALER = TMR2_64_PRESCALER;
      PR2 = 192;
      
    break;
    case BAUD_HALF_65_RATE:
      // 2560us
      T2_POSTSCALER = TMR2_02_POSTSCALER;
      T2_PRESCALER = TMR2_64_PRESCALER;
      PR2 = 192;
      
    break;
    default:
    break;
    
  }

}

static void send_it(const uint8_t *bit_arr){
	
	uint8_t hlooper = 0;
	

  t2_reset();
  
  TMR2ON = true;
  
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
		LUZ_TX = false;
	}
	else
	{
		LUZ_TX = true;
	}

}



#if 0

static void charge_iman(void){


  UWT("\r\nCharging\r\n");
  
  IO_Set_channel(IO_CHARGE_IMAN);
  
  __delay_ms(800);
  
  IO_Clear_channel(IO_CHARGE_IMAN);
  
}




static void activate_iman(void){
  
  IO_Set_channel(IO_ACTIVATE_IMAN);
  
}

#endif






//   * * * * * *      I S R  - -  H A N D L E R     * * * * * * * * * * * * * *   //


//   * * * * * *      O L D   C O D E   O B S O L E T E     * * * * * * * * * * * * * *   //


#if 0

static uint8_t write_rx_data_to_eeprom(void){
  uint8_t ret_value = false;
  uint8_t hlooper = 0;

  
  for(hlooper = 0; hlooper < CHARS_TO_RECEIVE - 1; hlooper++)
  {
    
    write_eeprom(eeprom_addresses[hlooper], config_bytes[hlooper]);
    
  }
  
}

#endif












#if 0

static uint8_t wait_for_startbyte(void){
  
	uint8_t ret_value = 0;
	uint8_t order_byte = 0;	

  // therefore we start a long timer to see if there is no more reception
  // --> during the development it should not matter but in the end i need that one-->
  // thereforedevelop it straight away correctly
  
  // the long TMR is perhaps TMR1 --> lets check on the max possible timeout...
  // on max. timeout we +- 500ms therefore we could give a cnt to 4 for a maximum of 2 second timeout OF-->
  
  // once we have received a startcondition we keep on going otherwise we might 
  // just stop when there is no TMR time left
  	

	t2_reset();
	
	TMR2ON = true;
		
	wait_for_tmr_expires(RX_LUZ_HALFBIT_TIME_OF_CNT);
	
	t2_reset();
    
		
	while(ret_value == 0)
	{

		if(wait_for_startbit() == true)	
		{

			order_byte = get_next_luz_char();

			t2_reset();
			
			switch(order_byte)
			{
				case 'r':
					reset_timeout_timer();
					ret_value = READ_ORDER;
				break;
				case 'w':
					reset_timeout_timer();
					ret_value = WRITE_ORDER;
				break;
				default:
				break;

			}
			
		}
		else
		{
			ret_value = TIMED_OUT;
		}
	}
  
  return ret_value;
  
  
  
}





#if DETECTOR

static uint8_t tx_config_data(void){
	uint8_t ret_value = false;
  uint8_t hlooper = 0;
  uint8_t chcksum = 0;
  uint8_t tx_data = 0;
  
  // configure_tmr2(TX_LUZ_HALFBIT_TIME);
  
  for(hlooper = 0; hlooper < CHARS_TO_RECEIVE - 1; hlooper++)
  {
    
    tx_data = LeerEeprom(eeprom_addresses[hlooper]);
    chcksum = chcksum ^ tx_data;
    tx_luz(tx_data);
    
  }
	tx_luz(chcksum);
  
}

#endif

#endif




// EOF