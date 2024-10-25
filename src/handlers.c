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
#include "gd_states.h"
// TODO: --> becaseu of tmr4 config move it
#include "Init_all.h"
#include "gps.h"
#include <stdint.h>


#define USE_FUNC_PNT_HANDLER 0

// set_handler_FLG(e_tilt_sensor_h);
static void f_tilt_sensor_to_check(void);



//  **********************  PRIVATE FUNCTIONS PROTOTYPES  ************************  //

static void set_handler_FLG(uint8_t handler_flg_spot);
static void reset_handler_FLG(uint8_t handler_flg_spot);
static uint8_t test_handler_FLG(uint8_t handler_flg_spot);

static void set_tmr_2ms_handler_dependencies_flgs(void);

static void f_gd_off(void);

static void prepare_sleep(void);
static void fn_clock_switching(void);
static void process_next_char_from_input(void);
static void f_gd_on(void);

static void f_prepare_msg(void);

static void set_tmr_25ms_handler_dependencies_flgs(void);
static void set_tmr_200ms_handler_dependencies_flgs(void);
static void set_tmr_1000ms_handler_dependencies_flgs(void);

static void f_gps_on(void);

static void reset_swoff_tmr_of_cnt(void);

static void swoff_tmr_handler(void);

static void err_handler_output(void);
static void empty_function(void);

static void test_handler_array(void);


//  **********************  DATA TYPES, STRUCTS, ENUMS  ************************  //

#if USE_FUNC_PNT_HANDLER

typedef struct {
	HandlerType Handlers;
	void(*func)(void);
}HandlersHandlerType;

static const HandlersHandlerType Handler_arr[] =
{
	
	// { e_sleep_handler,	                  prepare_sleep	},	
  { e_gd_off_h,                         f_gd_off },	  
	{ e_switch_clock_handler,             fn_clock_switching },	
	{ e_ring_buffer_handler,              process_next_char_from_input },
	{ e_2ms_of_handler,	                  set_tmr_2ms_handler_dependencies_flgs	},
	{ e_gd_on_h,                          f_gd_on },	
  
  
	// { e_adc_bateria_handler,	           adc_bat_handler },	
	{ e_tilt_sensor_h,	                  f_tilt_sensor_to_check },	
	{ e_200ms_h,                          set_tmr_200ms_handler_dependencies_flgs },
  { e_gps_on_h,                         f_gps_on },
  { e_prepare_msg_h,                      f_prepare_msg },
  
  
	{ e_rx_luz_com_h,                     empty_function },	
	{ e_reset_swoff_tmr_of_cnt_handler,   reset_swoff_tmr_of_cnt },	
	{ e_startup_h,                        empty_function },
  // { e_swoff_tmr_handler,               swoff_tmr_handler },
  { e_gps_has_full_position_h,               empty_function }, // TODO: write handler
  
	{ e_ertc_handler_start,               empty_function },	

	{ e_errhandler,                       empty_function },		

	
};

#endif



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

static volatile uint16_t Handler_FLGS = 0;


static volatile uint8_t temp_clockspeed_flg = false;


static uint8_t swoff_tmr_cnt = 0;
static uint16_t of_cnt_2000ms = 0;
static uint16_t of_cnt_200ms = 0;
static uint8_t of_cnt_100ms = 0;
static uint16_t of_cnt_seg_7d = 0;

//  **********************  PUBLIC FUNCTIONS BODY  ************************  //




void init_handler_flg(void){
	
	Handler_FLGS = (uint8_t)0u;
	
	test_handler_array();
	
}


#if USE_FUNC_PNT_HANDLER

void get_the_next_handler(void){

uint8_t handler_runs_once_flg = false;	
uint8_t handler_id = 0;	
	
  
  uint16_t temp_handler_FLGS = Handler_FLGS;

  // Create a mask by shifting 1 to the left by n_bit positions
  unsigned int mask = 1U; //  << n_bit;

  // Return whether the specific bit is set
  // return (b_field & mask) != 0; // Returns 1 if the bit is set, 0 otherwise


  while(Handler_FLGS == 0u)
  {
    CLRWDT();
    DB_LED1_SWAP;
  }

  temp_handler_FLGS = Handler_FLGS;

	for(handler_id = 0; handler_id < NUM_HANDLERS; handler_id++)
	{
    
		if((temp_handler_FLGS & mask) != 0)
		{

			(*Handler_arr[handler_id].func)();
			
				// this is a special case and gets reset in the actual function
			// if((handler_id != e_ring_buffer_handler) && (handler_id != e_gd_off_h))
      if(handler_id != e_ring_buffer_handler)
			{
				reset_handler_FLG(handler_id);
			}
      // else
      // {
        // UWT("EXIT!\r\n");
      // }

			handler_id = NUM_HANDLERS;
			
		}
    mask = mask << 1;
	}

}


#elif 1


void get_the_next_handler(void){

uint8_t handler_runs_once_flg = false;	
uint8_t handler_id = 0;	
	
  
  uint16_t temp_handler_FLGS = Handler_FLGS;

  // Create a mask by shifting 1 to the left by n_bit positions
  unsigned int mask = 1U; //  << n_bit;

  // Return whether the specific bit is set
  // return (b_field & mask) != 0; // Returns 1 if the bit is set, 0 otherwise


  while(Handler_FLGS == 0u)
  {
    CLRWDT();
    DB_LED1_SWAP;
  }

  temp_handler_FLGS = Handler_FLGS;

	for(handler_id = 0; handler_id < NUM_HANDLERS; handler_id++)
	{
    
		if((temp_handler_FLGS & mask) != 0)
		{


      switch(handler_id)
      {
        case 0:
f_gd_off ();	  
        break;
        case 1:
fn_clock_switching ();
        break;
        case 2:
process_next_char_from_input ();
        break;
        case 3:
set_tmr_2ms_handler_dependencies_flgs	();
        break;
        case 4:
f_gd_on ();	
        break;
        case 5:
f_tilt_sensor_to_check ();	
        break;
        case 6:
set_tmr_200ms_handler_dependencies_flgs ();
        break;
        case 7:
f_gps_on ();
        break;
        case 8:
f_prepare_msg ();
        break;
        case 9:
empty_function ();	
        break;
        case 10:
reset_swoff_tmr_of_cnt ();	
        break;
        case 11:
empty_function ();
        break;
        case 12:
empty_function ();	
        break;
        case 13:
empty_function ();	
        break;
        case 14:
empty_function ();	
        break;
        case 15:
empty_function ();	
        break;
        
        
      }
			
			
				// this is a special case and gets reset in the actual function
			// if((handler_id != e_ring_buffer_handler) && (handler_id != e_gd_off_h))
      if(handler_id != e_ring_buffer_handler)
			{
				reset_handler_FLG(handler_id);
			}

			handler_id = NUM_HANDLERS;
			
		}
    mask = mask << 1;
	}

}



#else
  
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
			if((handler_id != e_ring_buffer_handler) && (handler_id != e_gd_off_h))
			{
				reset_handler_FLG(handler_id);
			}

			handler_id = NUM_HANDLERS;
			
		}
	}

}

#endif

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
	
  



}



static void process_next_char_from_input(void){
	
	// we reset only here and if the ret_value from the btn_press is true
	
	// set_handler_FLG(e_reset_swoff_tmr_of_cnt_handler);
  
  uint8_t rx_data;

#if 1
// the actual gps input
  get_data_from_buffer_with_pnt(&rx_data);

  values_to_gps_rx_buffer(rx_data);
  
#else
	check_next_char();
#endif

}






static void set_tmr_200ms_handler_dependencies_flgs(void){
	
	
	set_handler_FLG(e_tilt_sensor_h);
  
}



static void f_tilt_sensor_to_check(void){
  
  
  tilt_sensor_get_state();

  
}


static void f_gps_on(void){
  
  
  // set up the GPS for reception --> bla bla, timeout timer, etc...
  // TODO:
  // UART_on
  // TIMEout timer on
  
  
  
  
}



static void f_prepare_msg(void){
  
  
  
  
  
}


#if 1

// this only happens when exiting sleep mode,
// therefore we just need the most basic things to start up, namely tmr4
static void f_gd_on(void){
  
  // reset all handlers because there should not be any allready active
  Handler_FLGS = (uint8_t)0u;
  
  if(FAST_CLOCK == FALSE)
  {
    init_clock();
    configure_tmr4();

  }
  
  TMR4_IF = FALSE;
  TMR4_IE = TRUE;
  
  PERIPHERIC_IE = TRUE;
	GLOBAL_IE = TRUE;
  TMR4_ON = TRUE;
  
  
}



static void f_gd_off(void){
  
  // when we enter here we do NOT need to take care of the oscillator timing related switch over
  // because the WDT clock is so unreliable that we are not further bothered...
  // --> that is going to be taken care of by the watch dog timer and sleep instruction...

  if(FAST_CLOCK == false)
  {
    
    init_clock();
    configure_tmr4();
    
  }

  DB_PRINT("DETECTOR IS OFF\r\n");
  
  
  PERIPHERIC_IE = FALSE;
	GLOBAL_IE = FALSE;
  TMR4_IE = FALSE;
  TMR4_ON = FALSE;
  
  LATC &= 0b11011011;
  LATB &= 0b00100011;
  LATA |= 0b01000000;
  LATA &= 0b11101000;
  
  // And we need to swoff all the periferic ISR IE
  OPTION_REGbits.INTEDG = TRUE;
  INTCONbits.INTE = TRUE;
  INTCONbits.INTF = FALSE;
  
  // and now set all the super low power things so that there is almost no consumption...
  // and jsut checking the Input pin for activation
  
  WDTCONbits.WDTPS = WDT_TIMEOUT_256s_timeout;
  WDTCONbits.SWDTEN = 0x01u;  // wdton = true
  

  while(INTCONbits.INTF == 0u)
  { 
    DB_LED_1_ON;
    
    SLEEP();
    
    DB_LED_1_OFF;

  }
  WDTCONbits.SWDTEN = 0x00u;  // wdton = true
  
  
  
#if DEBUGGING_IS_ON
// becasue in debugging we are sending ,sg and for that we will need speed in the clock
  if(FAST_CLOCK == FALSE)
  {
    init_clock();
    configure_tmr4();
    // __delay_ms(5);
    // DB_PRINT("DETECTOR IS ON\r\n"); 
  }
#endif
 
  INTCONbits.INTE = FALSE;

  gd_states_switch_to_next_state(E_STARTUP_STATE);
  
  
}





#else

// this one is (almost) working
static void f_gd_off(void){
  
  // when we enter here we do NOT need to take care of the oscillator timing related switch over
  // because the WDT clock is so unreliable that we are not further bothered...
  // --> that is going to be taken care of by the watch dog timer and sleep instruction...
  
  if(FAST_CLOCK == true)
  {
    // gd_states_set_gpsd_substate(E_TILT_SENSOR_IS_OFF);
    DB_PRINT("DETECTOR IS OFF\r\n");
    set_slow_clock();
    configure_tmr4();
    
  }
  
  PERIPHERIC_IE = FALSE;
	GLOBAL_IE = FALSE;
  TMR4_IE = FALSE;
  TMR4_ON = FALSE;
  
  LATC &= 0b11011011;
  LATB &= 0b00100011;
  LATA |= 0b01000000;
  LATA &= 0b11101000;
  
  // And we need to swoff all the periferic ISR IE
  
  // and now set all the super low power things so that there is almost no consumption...
  // and jsut checking the Input pin for activation
  WDTCONbits.WDTPS = 0x0Au;
  WDTCONbits.SWDTEN = 0x01u;  // wdton = true
  OPTION_REGbits.INTEDG = TRUE;
  INTCONbits.INTE = TRUE;
  INTCONbits.INTF = FALSE;
  while(INTCONbits.INTF == FALSE)
  {
    SLEEP();  // 512ms sleep
    
  }
  
  
  set_handler_FLG(e_tilt_sensor_h);
  
}


#endif


static void fn_clock_switching(void){
  
  
  // once we enter here we are actually switching the clock
  // therefore the eRTC TMR stopped allready
  if(FAST_CLOCK == true)
  {
    
    
    set_slow_clock();
    configure_tmr4();
    
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
  
#if DEBUGGING_IS_ON  
  
	return;
  
#else
  
return; //  wat?
  
#endif  

}




// we can test that the array is synced with the enumeration
// that is actually quite important!!
// TODO: that should get only in DEBUGGING
static void test_handler_array(void){

#if USE_FUNC_PNT_HANDLER	
	int8_t hlooper = 0;
	
	
	assert(NUM_HANDLERS == (sizeof(Handler_arr)/sizeof(Handler_arr[0])));

	for(hlooper = 0; hlooper < NUM_HANDLERS; hlooper++)
	{
		assert(hlooper == Handler_arr[hlooper].Handlers)
	}
#endif
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


#if 0
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

  // OPTION_REGbits.PS = TMR0_002_PRESCALER;

  

  SLEEP();

  RCSTAbits.CREN = true;
	TXSTAbits.TXEN = true;
  GIE = true;
  
  // init_all();
 


	
}

#endif
// EOF