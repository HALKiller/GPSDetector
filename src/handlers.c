// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


//  **********************  COMMENT BLOCK  ************************  //




//  **********************  INCLUDES BLOCK  ************************  //

#include "handlers.h"
#include "Aux_functions.h"


#include "timers.h"
#include "rx_luz.h"
#include "ring_buffer.h"
#include "Global.h"
#include "UART.h"
#include "io_port_sfr_names.h"

#include "Clock.h"
#include "ADC.h"
#include "Init_all.h"
#include "my_assert.h"
#include "generic_union_flgs.h"

#include "tilt_sensor.h"

// TODO: --> becaseu of tmr4 config move it
#include "Init_all.h"

#include <stdint.h>




// set_handler_FLG(E_TILT_SENSOR_h);
static void f_tilt_sensor_to_check(void);



//  **********************  PRIVATE FUNCTIONS PROTOTYPES  ************************  //

static void set_handler_FLG(uint8_t handler_flg_spot);
static void reset_handler_FLG(uint8_t handler_flg_spot);
static uint8_t test_handler_FLG(uint8_t handler_flg_spot);

static void set_tmr_2ms_handler_dependencies_flgs(void);



static void prepare_sleep(void);
static void fn_clock_switching(void);
static void process_next_char_from_input(void);
static void f_gd_on_off(void); // static void f_gd_on_off(HandlerType the_handler);



static void set_tmr_25ms_handler_dependencies_flgs(void);
static void set_tmr_200ms_handler_dependencies_flgs(void);
static void set_tmr_1000ms_handler_dependencies_flgs(void);



static void reset_swoff_tmr_of_cnt(void);

static void swoff_tmr_handler(void);

static void err_handler_output(void);
static void empty_function(void);

static uint8_t test_handler_array(void);


//  **********************  DATA TYPES, STRUCTS, ENUMS  ************************  //


typedef struct {
	HandlerType Handlers;
	void(*func)(void);
}HandlersHandlerType;

static const HandlersHandlerType Handler_arr[] =
{
	
	{ e_sleep_handler,	                  prepare_sleep	},	
	{ e_switch_clock_handler,             fn_clock_switching },	
	{ e_ring_buffer_handler,              process_next_char_from_input },
	{ e_2ms_of_handler,	                  set_tmr_2ms_handler_dependencies_flgs	},
	{ E_GD_ON_OFF_h,                      f_gd_on_off },	
  
	// { e_adc_bateria_handler,	           adc_bat_handler },	
	{ E_TILT_SENSOR_h,	                  f_tilt_sensor_to_check },	
	{ e_200ms_h,                          set_tmr_200ms_handler_dependencies_flgs },
	// { e_seg_7d_refresh_handler,          seg_7d_refresh },	
	{ e_reset_swoff_tmr_of_cnt_handler,  reset_swoff_tmr_of_cnt },	
	// { e_swoff_tmr_handler,               swoff_tmr_handler },
	// { e_swoff_consumption,               swoff_handler },				
	{ e_errhandler,                      empty_function },		

	
};


union8_t gFLAGS;

//  **********************  CONSTANT EXPRESSIONS  ************************  //

static const uint8_t const_MAXIMUM_HANDLERS = 16;


//  **********************  MACRO DEFINITIONS  ************************  //
#define TIME_BASE 2
#define MS_PER_SECOND 1000
#define DB_HANDLER_CF 0
#define c_ADC_OVERSAMPLING 8
#define cADC_BATERIA_MIMIMUM_THRESHHOLD 127	// these are ADC
#define BATERIE_MINIMUM_mv_LEVEL	6000


#define SEG_7D_REFRESH 2000

#define OF_CNT_100MS 50
#define OF_CNT_200MS 100
#define OF_CNT_1000MS 500
#define OF_CNT_500MS 250
#define OF_CNT_2000MS 1000

#define C_OF_CNT_SEG_7D SEG_7D_REFRESH/TIME_BASE

#define MAX_SWOFF_TMR_CNT 50	// 50 x 200 = 10000ms

//  **********************  STATIC DATA DECLARATIONS  ************************  //

volatile uint16_t Handler_FLGS = 0;


volatile uint8_t temp_clockspeed_flg = false;


uint8_t swoff_tmr_cnt = 0;
uint16_t of_cnt_2000ms = 0;
uint16_t of_cnt_200ms = 0;
uint8_t of_cnt_100ms = 0;
uint16_t of_cnt_seg_7d = 0;

//  **********************  PUBLIC FUNCTIONS BODY  ************************  //




void init_handler_flg(void){
	
	Handler_FLGS = 0;
	
	test_handler_array();
	
}



void get_the_next_handler(void){

uint8_t handler_runs_once_flg = false;	
uint8_t handler_id = 0;	
	


	assert(NUM_HANDLERS == sizeof(Handler_arr)/sizeof(Handler_arr[0]));


	for(handler_id = 0; handler_id < NUM_HANDLERS; handler_id++)
	{
		if(test_handler_FLG(handler_id) == true)
		{

			(*Handler_arr[handler_id].func)();
			
				// this is a special case and gets reset in the actual function
			if(handler_id != e_ring_buffer_handler)
			{
				reset_handler_FLG(handler_id);
			}

			handler_id = NUM_HANDLERS;
			
		}
	}

}


void reset_ring_buffer_handler_FLG(void){
	
	reset_handler_FLG(e_ring_buffer_handler);
	
}


void handlers_generic_set_handler_FLG(uint8_t handler_set){
	
	set_handler_FLG(handler_set);
	
}


static void reset_swoff_tmr_of_cnt(void){
	
	swoff_tmr_cnt = 0;
	
}

//  **********************  PRIVATE FUNCTIONS BODY  ************************  //


static uint8_t test_handler_FLG(uint8_t handler_flg_spot){

	uint8_t ret_value = false;
	
	if(test_bit_in_int(Handler_FLGS, handler_flg_spot) == true)
	{
		ret_value = true;
	}
	
	return ret_value;
	
}


// PRIVATE --> these handlers are getting, set, tested and reset 
// locally here and are therefore of private nature


static void set_handler_FLG(uint8_t handler_flg_spot){
	
  bool temp_GIE = GLOBAL_IE;

	GIE = false;
	
	Handler_FLGS = set_single_bit_in_int(Handler_FLGS, handler_flg_spot);
	
	GIE = temp_GIE;
	
}


static void reset_handler_FLG(uint8_t handler_flg_spot){
	
  bool temp_GIE = GLOBAL_IE;

	GIE = false;

	Handler_FLGS = clear_single_bit_in_int(Handler_FLGS, handler_flg_spot);
	
	GIE = temp_GIE;	
	

}



static void set_tmr_2ms_handler_dependencies_flgs(void){
	
	// every 2ms we test the btn and depending on theyr state we might do one thing ore another
	// handlers_generic_set_handler_FLG(e_button_state_update_handler);
	
  static uint16_t rndcnt = 0xABCD;
  
	of_cnt_100ms++;
	of_cnt_200ms++;
	of_cnt_2000ms++;
	of_cnt_seg_7d++;
  

	
  // UWT("Ja\r\n");
	
#if USE_REAL_PCB	|| 1
	// refresh buttons --> thats the handler NOT the btn pressed state!
	if(OF_CNT_100MS < of_cnt_100ms)
	{
		
		// handlers_generic_set_handler_FLG(e_button_handler);	
		of_cnt_100ms = 0;
		
	}	
	
  
#if 0  
	// every 1000 ms led and seg7_d refresh
	if(C_OF_CNT_SEG_7D < of_cnt_seg_7d)
	{
		// handlers_generic_set_handler_FLG(e_seg_7d_refresh_handler);
		handlers_generic_set_handler_FLG(e_leds_refresh_handler);
		of_cnt_seg_7d = 0;
    rndcnt++;
    UWT("16bit: ");
    UART_int(rndcnt);
    UART_CRLF;
    
    UWT("32bit: ");
    UART_32_int(rndcnt);
    UART_CRLF;
    
	}
#endif
	// every 200ms updating the swoff cnt...
	if(OF_CNT_200MS < of_cnt_200ms)
	{
		
		// handlers_generic_set_handler_FLG(e_swoff_tmr_handler);
		of_cnt_200ms = 0;
	}


#endif
	
  

	// every 2 seconds --> ADC_BAT_HANDLER
	if(OF_CNT_2000MS < of_cnt_2000ms)
	{

    handlers_generic_set_handler_FLG(e_sleep_handler);  

		of_cnt_2000ms = 0;
		
	}

}



static void process_next_char_from_input(void){
	
	// we reset only here and if the ret_value from the btn_press is true
	
	set_handler_FLG(e_reset_swoff_tmr_of_cnt_handler);

	if(check_next_char() != 0)
	{
#if LANGUAGE_SPANISH
		UWT("\r\nRecibido suma de control desde PC esta mal\r\n");
#else		
		UWT("\r\nBad checksum from pc\r\n");
#endif	
	}

}



static void f_gd_on_off(void){
  
  // reset_handler_FLG(the_handler);
  
  DB_PRINT("Tilt Sensor state change!\r\n");
  
  if(tilt_sensor_get_detector_state() == TS_ON_STATE)
  {
    
    // we can do that becaue that is always the highest level of all things 
    // gd_states_set_gpsd_substate(E_TILT_SENSOR_IS_ON);
    DB_PRINT("DETECTOR IS ON\r\n");
    
  }
  else
  {
    
    // gd_states_set_gpsd_substate(E_TILT_SENSOR_IS_OFF);
    DB_PRINT("DETECTOR IS OFF\r\n");
    
  }
  
  // gd_states_switch_to_next_state();
  
  
}



static void set_tmr_200ms_handler_dependencies_flgs(void){
	
	DB_LED2_SWAP;
	set_handler_FLG(E_TILT_SENSOR_h);
  
}



static void f_tilt_sensor_to_check(void){
  
  
  tilt_sensor_get_state();

  
}




static void prepare_sleep(void){
	
  static uint8_t rndcnt = 0;
  
  
  if(temp_clockspeed_flg == false)
  {
    UWT("Prepare_sleep\r\n");  
    while(!PIR1bits.TXIF)
    {
      // empty_loop
    }
    
  }
  
 
  
  #if 1    
  
  LATC &= 0b11011011;
  LATB &= 0b00100011;
  LATA |= 0b01000000;
  LATA &= 0b11101000;
  
  


  // FVRCONbits.FVREN = 0;
  // FVRCONbits.ADFVR = 0;
  
  RCSTAbits.CREN = false;
	TXSTAbits.TXEN = false;
  GIE = false;
  
  // TRISA = 0x00;
  // TRISB = 0x00;
  // TRISC = 0x00;
  
  // LATA = 0x00;
  // LATB = 0x00;
  // LATC = 0x00;
  
  // clock_slowdown();
  
  WDTCONbits.SWDTEN = 1;
  
 #endif 
  
    // DB_LED_1 = true;
    __delay_ms(2);
    // DB_LED_1 = false;
  
  if(rndcnt == 1)
  {
    // DB_LED_2 = true;
    __delay_ms(1);
    // DB_LED_2 = false;
    // set_clock_speed(SLOW_CLOCK_OSC);
    // clock_slowdown();
    rndcnt = 0;
    temp_clockspeed_flg = true;
    TMR0IE = false;
    TMR4IE = true;
    TMR4_ON = true;

    
  }
  
  rndcnt++;
#if 0  
  if(rndcnt == 5)
  {
    
    
    set_clock_speed(FAST_CLOCK_OSC);
    temp_clockspeed_flg = false;
    TMR0IE = true;
    TMR4IE = false;
    LUZ_TX = true;
    __delay_ms(200);
    LUZ_TX = false;
  }
#endif
  // OPTION_REGbits.PS = TMR0_002_PRESCALER;

  

  SLEEP();

  RCSTAbits.CREN = true;
	TXSTAbits.TXEN = true;
  GIE = true;
  
  // init_all();
 


	
}






static void fn_clock_switching(void){
  
  
  // once we enter here we are actually switching the clock
  // therefore the eRTC TMR stopped allready
  if(FAST_CLOCK == true)
  {
    
    configure_tmr4();
    set_slow_clock();
    
  }
  else
  {
    
    init_clock ();
    configure_tmr4();
    
  }
  
  TMR4_ON = true;
  
}

















static void err_handler_output(void){
	
	
	UWT("\r\n  AN UNDEFINED ERROR OCURRED!\r\n");
	while(1);
	
}


static void empty_function(void){
	return;
}




// we can test that the array is synced with the enumeration
// that is actually quite important!!
// TODO: that should get only in DEBUGGING
static uint8_t test_handler_array(void){
	
	uint8_t hlooper = 0;
	
	
	assert(NUM_HANDLERS == (sizeof(Handler_arr)/sizeof(Handler_arr[0])));

	for(hlooper = 0; hlooper < NUM_HANDLERS; hlooper++)
	{
		assert(hlooper == Handler_arr[hlooper].Handlers)
	}
	return true;
}






#if 0

static void init_handlers(void){
	
	uint8_t hlooper = 0;
	
	for(hlooper = 0; hlooper < NUM_HANDLERS_E; hlooper++)
	{
		
		if(hlooper != Handler_arr[hlooper].Handlers)
		{
			while(1)
			{
				UWT("Handler Array Error!");
				delayms(500);
			}
			
		}
		
	}
	
	UWT("\r\nHandlers O.K\r\n");
	
	
	
}

#endif









//  * * * * * * * * * * * * * * * * * * *     U N U S E D   S T U F F     * * * * * * * * * * * * * * * * * * * * * * * * 
//  * * * * * * * * * * * * * * * * * * *     U N U S E D   S T U F F     * * * * * * * * * * * * * * * * * * * * * * * * 
//  * * * * * * * * * * * * * * * * * * *     U N U S E D   S T U F F     * * * * * * * * * * * * * * * * * * * * * * * * 



#if 0


static void set_tmr_25ms_handler_dependencies_flgs(void){
#if 0
	set_handler_FLG(e_update_dac_handler);
	
	// because i am using this outputs as SPI UART emulator...
	set_handler_FLG(e_digital_output_handler);
#endif 	
	
	
}







static void set_tmr_1000ms_handler_dependencies_flgs(void){
static uint8_t rnd_cnt = 0;	

#if !RELEASE_THIS_VERSION
	set_handler_FLG(e_debugging_handler);
#endif	
	handlers_generic_set_handler_FLG(e_spi_uart_emul_handler);
}


#endif







#if 0

static uint8_t parameter_exists_on_array_id(uint8_t parameter_ID){
	
	uint8_t soz = sizeof(Handler_arr) / sizeof(Handler_arr[0]);
	uint8_t parameter_found_flg = false;
	uint8_t hlooper = 0;
	uint8_t ret_value = 0xFF;
	
	do
	{
		if(parameter_ID == Handler_arr[hlooper].Handlers)
		{
			parameter_found_flg = true;
			ret_value = hlooper;
		}
		
		hlooper++;
	}while((parameter_found_flg == false) && (hlooper < soz));
	
	return ret_value;
}





static uint8_t parameter_exists_on_array_id(uint8_t parameter_ID){
	
	uint8_t soz = sizeof(Handler_arr) / sizeof(Handler_arr[0]);
	uint8_t parameter_found_flg = false;
	uint8_t hlooper = 0;
	uint8_t ret_value = 0xFF;
	
	do
	{
		if(parameter_ID == Handler_arr[hlooper].Handlers)
		{
			parameter_found_flg = true;
			ret_value = hlooper;
		}
		
		hlooper++;
	}while((parameter_found_flg == false) && (hlooper < soz));
	
	return ret_value;
}


#endif


#if 0

static void btn_update_handler(void){
	
	// this is the HW...
	update_button_handler();
	
#if 0
	UWT("Luz: ");
	UART_int(adc_samples_channel(1));
#endif
	
}


// this receives only the debounced value...
static void btn_state_handler(void){
	
#if DB_BTN_ERR
	
	btn_number_setter_btn_states();
		
#else
		
	
	if(btn_number_setter_btn_states() == true)
	{
		set_handler_FLG(e_reset_swoff_tmr_of_cnt_handler);
	}
	
#endif
}

#endif

#if 0
static void leds_refresh_output(void){
	

	leds_move_shadow_leds_to_real_leds();


}



static void seg_7d_refresh(void){
	
	refresh_7Sd();
	
}


static void swoff_tmr_handler(void){
	
#if DISPLAY_IS_NOT_ALWAYS_ON
	// g_vat because we reset this variable from 
	// different parts in form of a function call
	swoff_tmr_cnt++;
	
	if(MAX_SWOFF_TMR_CNT < swoff_tmr_cnt)
	{
#if DEBUGGING_IS_ON&&0	
	UWT("\r\nSwoff handler gets set ...\r\n");
#endif
		handlers_generic_set_handler_FLG(e_swoff_consumption);
		// avoid verflow
		swoff_tmr_cnt = MAX_SWOFF_TMR_CNT;
	}		
#endif	
	
}

#endif

#if 0

// implementing the measurement of the internal vref for the calculation of the vbat value//
static void adc_bat_handler(void){
	
	uint8_t sample_looper = 0;
	uint16_t sum = 0;
	uint8_t temp_val = 0;
	
	uint8_t vref_adc = 0;
	const uint32_t V_ref_uV= 4096000;
	const uint16_t const_diode_drop_mV = 700;
	uint16_t uV_per_adc = 0;
	uint32_t Vbat_mV = 0;
	uint32_t adc_vbat = 0;
	
  
  ADC_ON = true;
  
	vref_adc = adc_samples_channel(VREF_ADC_CHANNEL);
	
	uV_per_adc = V_ref_uV / vref_adc;
	
	
#if 0	
  UWT("ADC_Vref: ");
  UART_int(vref_adc);
  UART_CRLF;	
#endif

#if 1

	for(sample_looper = 0; sample_looper < c_ADC_OVERSAMPLING; sample_looper++)
	{
		temp_val = adc_samples_channel(BATERIA_ADC_CHANNEL);
		sum = sum + temp_val;
		
	}
	
	adc_vbat = sum / c_ADC_OVERSAMPLING;
	
	Vbat_mV = adc_vbat * uV_per_adc / 500;	// divide by 500 becaue R_Divider = 2:1 and therefore only by 400
	Vbat_mV = Vbat_mV + const_diode_drop_mV;
	
#if DEBUGGING_IS_ON&&ADC_BAT_DB_IS_ON
  UWT("ADC ");
  UART_int(sum / c_ADC_OVERSAMPLING);
  UART_CRLF;

  UWT("Bat: ");
  UART_int(Vbat_mV);
#endif



	// TODO --> i still need here the R_divider_values and the actuall thresshold voltages etc...
	if(Vbat_mV < BATERIE_MINIMUM_mv_LEVEL)
		// if((sum / c_ADC_OVERSAMPLING) < cADC_BATERIA_MIMIMUM_THRESHHOLD)
	{
		
		// handler flag for low batery
#if DEBUGGING_IS_ON&&0
		UWT("\r\nBAT is BAD!\r\n");
#endif    
		leds_update_shadow_led(e_LED_ON_OFF, e_LC_RED);
	}
	else
	{
#if DEBUGGING_IS_ON&&0
		UWT("\r\nBAT is GOOD!\r\n");
#endif    
		leds_update_shadow_led(e_LED_ON_OFF, e_LC_GREEN);
	}
	
#else	
	
  // btn_nr_setter_get_swoff_state

	for(sample_looper = 0; sample_looper < c_ADC_OVERSAMPLING; sample_looper++)
	{
		
		sum = sum + adc_samples_channel(BATERIA_ADC_CHANNEL);
		
	}
	
	// TODO --> i still need here the R_divider_values and the actuall thresshold voltages etc...
	if((sum / c_ADC_OVERSAMPLING) < cADC_BATERIA_MIMIMUM_THRESHHOLD)
	{
		
		// handler flag for low batery
		leds_update_shadow_led(e_LED_ON_OFF, e_LC_RED);
		
	}
	else
	{
		leds_update_shadow_led(e_LED_ON_OFF, e_LC_GREEN);
	}
#endif	

  ADC_ON = false;

}






static void swoff_handler(void){
	
	leds_update_shadow_led(e_LED_STATE, e_LED_OFF);
  
	leds_update_shadow_led(e_LED_ON_OFF, e_LED_OFF);

	segment_7d_swoff_seg_7d();
	
  BCD_ACTIVE = false;
  
	btn_nr_setter_set_swoff_flg();
#if REDUCE_CONSUMPTION	
  // set_clock_speed(SLOW_CLOCK_OSC);
  // clock_slowdown();
#endif  
  
}


#endif





#if 0


#if 0 // Shorteinging messages for ROM use...--> DEBUGGING only at the moment
	
static void rx_response_handler(void){
uint8_t ret_val = 0;  
  
  ADC_ON = true;
  
  ret_val = check_on_rx_luz();
  switch(ret_val)
  {
    case 0:
      // all good
      leds_update_shadow_led(e_LED_STATE, e_LC_GREEN);
#if LANGUAGE_SPANISH
      UWT("\r\nLa respuesta estaba correcta\r\n");
      UWT("El config se grabo correctamente\r\n");
#else
      UWT("\r\nReadback was correct\r\n");
#endif  

    break;
    
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
       
      leds_update_shadow_led(e_LED_STATE, e_LC_RED);

      UWT("\r\nERROR: "); 
      UART_int(ret_val);
        
    break;
 					
    default:
    
    
    
			// should be impossible
			leds_update_shadow_led(e_LED_STATE, e_LC_RED);
#if LANGUAGE_SPANISH
      UWT("\r\nUn error no definido ha pasado\r\n");     
#else
      UWT("\r\nAn undefined error ocurrd\r\n");     
#endif  
			
    break;
    
  }
	
  ADC_ON = false;
  
	flush_ring_buffer();
  
}

#elif 1

	
static void rx_response_handler(void){
  
  ADC_ON = true;
  
  switch(check_on_rx_luz())
  {
    case 0:
      // all good
      leds_update_shadow_led(e_LED_STATE, e_LC_GREEN);
#if LANGUAGE_SPANISH
      UWT("\r\nLa respuesta estaba correcta\r\n");
      UWT("El config se grabo correctamente\r\n");
#else
      UWT("\r\nReadback was correct\r\n");
#endif  

    break;
    case 1:
    
      // bad chcksum but correct amount of chars
	  leds_update_shadow_led(e_LED_STATE, e_LC_RED);
#if LANGUAGE_SPANISH
      UWT("\r\nLa respuesta desde el detector ha dado un mal checksum\r\n");
#else
      UWT("\r\nERROR 1\r\n"); // UWT("\r\nReadback gave correct byte count but bad checksum\r\n");
#endif      
			
    break;
    case 2:
		   // 
		   leds_update_shadow_led(e_LED_STATE, e_LC_RED);
#if LANGUAGE_SPANISH
      UWT("\r\nTIMEOUT en la recepcion desde el detector\r\n");
#else
      UWT("\r\nERROR 2\r\n"); // UWT("\r\nTIME OUT on reception from detector GPS\r\n");
#endif         
			
    break;
    case 3:
		   // 
		   leds_update_shadow_led(e_LED_STATE, e_LC_RED);
#if LANGUAGE_SPANISH
      UWT("\r\nRecibido config data no ha coincido con la transmitida\r\n");
#else
      UWT("\r\nERROR 3\r\n"); // UWT("\r\nReceived config data did not coincide with sent data\r\n");
#endif  
			
    break;
	  case 4:
		   // 
		   leds_update_shadow_led(e_LED_STATE, e_LC_RED);
#if LANGUAGE_SPANISH
      UWT("\r\nNumero enviado conflictivo con maximum permitidos\r\n");
#else
      UWT("\r\nERROR 4\r\n"); // UWT("\r\nSent detector number was higher than maxmum permitted on this series\r\n");
#endif         
			
    break;
	  case 5:
		   // 
		   leds_update_shadow_led(e_LED_STATE, e_LC_RED);
#if LANGUAGE_SPANISH
      UWT("\r\nDetector respondio con error mensaje no definido\r\n");
#else
      UWT("\r\nERROR 5\r\n"); // UWT("\r\nDetector answered with a not defined error\r\n");
#endif         
			
    break;						
    default:
			// should be impossible
			leds_update_shadow_led(e_LED_STATE, e_LC_RED);
#if LANGUAGE_SPANISH
      UWT("\r\nUn error no definido ha pasado\r\n");     
#else
      UWT("\r\nAn undefined error ocurrd\r\n");     
#endif  
			
    break;
    
  }
	
  ADC_ON = false;
  
	flush_ring_buffer();
  
}

#else
	
static void rx_response_handler(void){
  
  ADC_ON = true;
  
  switch(check_on_rx_luz())
  {
    case 0:
      // all good
	  leds_update_shadow_led(e_LED_STATE, e_LC_GREEN);
			UWT("\r\nReadback was correct\r\n");
    break;
    case 1:
      // bad chcksum but correct amount of chars
	  leds_update_shadow_led(e_LED_STATE, e_LC_RED);
			UWT("\r\nReadback gave correct byte count but bad checksum\r\n");
    break;
    case 2:
		   // 
		   leds_update_shadow_led(e_LED_STATE, e_LC_RED);
			UWT("\r\nTIME OUT on reception from detector GPS\r\n");
    break;
    case 3:
		   // 
		   leds_update_shadow_led(e_LED_STATE, e_LC_RED);
			UWT("\r\nReceived config data did not coincide with sent data\r\n");
    break;
	case 4:
		   // 
		   leds_update_shadow_led(e_LED_STATE, e_LC_RED);
			UWT("\r\nSent detector number was higher than maxmum permitted on this series\r\n");
    break;
	case 5:
		   // 
		   leds_update_shadow_led(e_LED_STATE, e_LC_RED);
			UWT("\r\nDetector answered with a not defined error\r\n");
    break;						
    default:
			// should be impossible
			leds_update_shadow_led(e_LED_STATE, e_LC_RED);
			UWT("\r\nAn undefined error ocurrd\r\n");     
    break;
    
  }
	
  ADC_ON = false;
  
	flush_ring_buffer();
  
}


#endif
#endif


// EOF