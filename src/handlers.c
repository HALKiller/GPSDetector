// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


//  **********************  COMMENT BLOCK  ************************  //




//  **********************  INCLUDES BLOCK  ************************  //

#include "handlers.h"

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

#include "e_rtc.h"

#include "pwm_luz.h"

#include "DDS.h"

#include "messages.h"

#include <stdint.h>


#define USE_FUNC_PNT_HANDLER 1


static void f_tilt_sensor_to_check(void);



//  **********************  PRIVATE FUNCTIONS PROTOTYPES  ************************  //

static void set_handler_FLG(uint8_t handler_flg_spot);
static void reset_handler_FLG(uint8_t handler_flg_spot);
static void f_gd_off(void);

static void f_always_transmit(void);
static void fn_clock_switching(void);
static void process_next_char_from_input(void);
static void f_gd_on(void);
static void f_prepare_msg(void);
static void f_rx_luz_com_handler(void);
static void set_tmr_200ms_handler_dependencies_flgs(void);
static void f_gps_on(void);
static void f_gps_has_position(void);
static void err_handler_output(void);
static void f_gps_test_rx(void);
static void empty_function(void);


#if DEBUGGING_IS_ON
static void test_handler_array(void);
#endif


//  **********************  DATA TYPES, STRUCTS, ENUMS  ************************  //

#if USE_FUNC_PNT_HANDLER

typedef struct {
	HandlerType Handlers;
	void(*func)(void);
}HandlersHandlerType;

static const HandlersHandlerType Handler_arr[] =
{
	
  { e_rx_luz_com_h,                     f_rx_luz_com_handler },	
	{ e_always_transmit_handler,          f_always_transmit },	  
  { e_gd_off_h,                         f_gd_off },	  
	{ e_switch_clock_handler,             fn_clock_switching },	
	{ e_ring_buffer_handler,              process_next_char_from_input },
	{ e_tilt_sensor_h,	                  f_tilt_sensor_to_check },	
	{ e_200ms_h,                          set_tmr_200ms_handler_dependencies_flgs },
  { e_gps_on_h,                         f_gps_on },
  { e_prepare_msg_h,                    f_prepare_msg },
  
	
	{ e_startup_h,                        f_gd_on },
  { e_gps_has_full_position_h,          f_gps_has_position },
	{ e_ertc_handler_start,               empty_function },	
	{ e_errhandler,                       empty_function },		
  { e_gps_test_reception,               f_gps_test_rx },
	
};

#endif




//  **********************  CONSTANT EXPRESSIONS  ************************  //

static const uint8_t const_MAXIMUM_HANDLERS = 16;

const uint16_t shifts[16] = {
	
	(1U << 0), (1U << 1), (1U << 2), (1U << 3), 
	(1U << 4), (1U << 5), (1U << 6), (1U << 7), 
  (1U << 8), (1U << 9), (1U << 10), (1U << 11), 
	(1U << 12), (1U << 13), (1U << 14), (1U << 15), 
	
};

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

// #define MAX_SWOFF_TMR_CNT 50	// 50 x 200 = 10000ms

//  **********************  STATIC DATA DECLARATIONS  ************************  //

static volatile uint16_t Handler_FLGS = 0;

static volatile uint8_t temp_clockspeed_flg = false;

static uint16_t of_cnt_200ms = 0;

//  **********************  PUBLIC FUNCTIONS BODY  ************************  //




void init_handler_flg(void){
	
	Handler_FLGS = (uint8_t)0u;
  
  
#if DEBUGGING_IS_ON	
	test_handler_array();
#endif  
	
}




#if USE_FUNC_PNT_HANDLER

void get_the_next_handler(void){

  uint8_t handler_id = 0;	
	
  uint16_t temp_handler_FLGS = 0u;  // Handler_FLGS;

  // Create a mask by shifting 1 to the left by n_bit positions
  uint16_t mask = 1u; //  << n_bit;
  

  // because we are not returning from this function ever we can reset the Stack Pointer to
  // its reset value and have the full 16 level Hardware stack available again
  STKPTR = 0x1Fu;

#if DEBUGGING_IS_ON&&0
  DB_PRINT("\r\nBRGH: ");
  UART_int(SPBRGH);
  DB_PRINT("\r\nBRG: ");
  UART_int(SPBRG);
  UART_CRLF;
#endif

  while(1)
  {

    mask = 1u;
    
    handler_id = 0u;
    
    while(Handler_FLGS == 0u)
    {
      
      CLRWDT();
      DB_LED3_SWAP;
      
    }

    temp_handler_FLGS = Handler_FLGS;
    
    while((temp_handler_FLGS & mask) == 0)
    {
      
      mask = mask << 1;
      
      handler_id++;
      
    }
    
    // DB_LED2_SWAP;
    
    assert(handler_id < NUM_HANDLERS);
    
    (*Handler_arr[handler_id].func)();
    
    if(handler_id != e_ring_buffer_handler)
    {
      reset_handler_FLG(handler_id);
    }

  }
}

#else
  
void get_the_next_handler(void){

  uint8_t handler_id = 0;	
	
  uint16_t temp_handler_FLGS = 0u;  // Handler_FLGS;

  // Create a mask by shifting 1 to the left by n_bit positions
  uint16_t mask = 1u; //  << n_bit;
  

  // because we are not returning from this function ever we can reset the Stack Pointer to
  // its reset value and have the full 16 level Hardware stack available again
  
  // STKPTR = 0x1Fu;

#if DEBUGGING_IS_ON
  DB_PRINT("\r\nBRGH: ");
  UART_int(SPBRGH);
  DB_PRINT("\r\nBRG: ");
  UART_int(SPBRG);
  UART_CRLF;
#endif

  while(1)
  {

    mask = 1u;
    
    handler_id = 0u;
    
    while(Handler_FLGS == 0u)
    {
      
      CLRWDT();
      DB_LED3_SWAP;
      
    }

    temp_handler_FLGS = Handler_FLGS;
    
    while((temp_handler_FLGS & mask) == 0)
    {
      
      mask = mask << 1;
      
      handler_id++;
      
    }
    
    // DB_LED2_SWAP;
    
    assert(handler_id < NUM_HANDLERS);
    
    
    switch(handler_id)
      {
        

        case e_gd_off_h:                      
          f_gd_off();
        break;
        case e_switch_clock_handler:             
          fn_clock_switching();
        break;
        case e_ring_buffer_handler:              
          process_next_char_from_input();
        break;
        case e_tilt_sensor_h:	                  
          f_tilt_sensor_to_check();
        break;
        case e_200ms_h:                          
          set_tmr_200ms_handler_dependencies_flgs();
        break;
        case e_gps_on_h:                         
          f_gps_on();
        break;
        case e_prepare_msg_h:                    
          f_prepare_msg();
        break;
        case e_rx_luz_com_h:                     
          f_rx_luz_com_handler();
        break;
        case e_startup_h:                        
          f_gd_on();
        break;
        case e_gps_has_full_position_h:          
          f_gps_has_position();
        break;
        case e_ertc_handler_start:               
          empty_function();
        break;
        case e_errhandler:                       
          empty_function();
        break;
        case e_gps_test_reception:               
          f_gps_test_rx();
        break;

      }

    // (*Handler_arr[handler_id].func)();
    
    if(handler_id != e_ring_buffer_handler)
    {
      reset_handler_FLG(handler_id);
    }

  }
}



#endif

// DB_PRINT("A\r\n");


void reset_ring_buffer_handler_FLG(void){
	
	reset_handler_FLG(e_ring_buffer_handler);
	
}


void handlers_generic_set_handler_FLG(uint8_t handler_set){
	
  
  bool temp_GIE = GLOBAL_IE;

	GIE = false;
	
  set_handler_FLG(handler_set);
 
  // restore GIE	
	GIE = temp_GIE;
  
}



//  **********************  PRIVATE FUNCTIONS BODY  ************************  //


// PRIVATE --> these handlers are getting, set, tested and reset 
// locally here and are therefore of private nature



// the two version: 
// with function call: 8MIPS ->  12.1us, 500kHz --> 750us
// with preconditioned bitshifting inside: 8MIPS --> 7.6us, 500kHz -> 463us
// #pragma interrupt_level 1

static void set_handler_FLG(uint8_t handler_flg_spot){


	
#if 1
  // this is much faster...
	Handler_FLGS = Handler_FLGS | (shifts[handler_flg_spot]);	// sets the bit....
	
 #else
  
	Handler_FLGS = set_single_bit_in_int(Handler_FLGS, handler_flg_spot);

#endif

	
}

#if 1

static void reset_handler_FLG(uint8_t handler_flg_spot){
	
  bool temp_GIE = GLOBAL_IE;

	GIE = false;

	Handler_FLGS = Handler_FLGS & ~(shifts[handler_flg_spot]);
  
	GIE = temp_GIE;	
	

}

#else
  

static void reset_handler_FLG(uint8_t handler_flg_spot){
	
  bool temp_GIE = GLOBAL_IE;

	GIE = false;

	Handler_FLGS = clear_single_bit_in_int(Handler_FLGS, handler_flg_spot);
	
	GIE = temp_GIE;	
	

}

#endif






#if 0


static void set_tmr_200ms_handler_dependencies_flgs(void){


  handlers_generic_set_handler_FLG(e_tilt_sensor_h);
 
}

#elif 1


static void set_tmr_200ms_handler_dependencies_flgs(void){

#if TEST_ERTC_SLOW_CLOCK
	
  static uint16_t s_cnt = 0;
  
  s_cnt++;
  
#endif
  

    if(s_cnt >= 5)
    {
      
      if(DEBUG_FLG_PRINT_TIME == TRUE)
      {
        DB_PRINT("\r\nE: ");
        
#if USE_FULL_SECONDS_FOR_RTC      
        ertc_convert_to_real_time(eRTC_get_second_cnt());
#else        
        ertc_convert_to_real_time(eRTC_get_second_cnt() / 10u);
        
#endif      

        ertc_convert_to_str();
        
        DB_PRINT("G: ");
        ertc_convert_to_real_time(gps_rtc_get_second_cnt());
        ertc_convert_to_str();
        
        
        // AD9954Configura();
        
      }
      
      

      s_cnt = 0;

    }

#if COMPILE_WITH_PWM_LUZ
  if(LUZ_ENABLED == TRUE)
  {
    pwm_luz_time_update();
  }
#endif
  
// measure the setting time...

  
  // f_tilt_sensor_to_check();
  
 
	handlers_generic_set_handler_FLG(e_tilt_sensor_h);
 
 
}



#else
  

static void set_tmr_200ms_handler_dependencies_flgs(void){

#if TEST_ERTC_SLOW_CLOCK
	
  static uint16_t s_cnt = 0;
  
  s_cnt++;
  
#endif
  
  // DB_PRINT("JA\r\n");
  if(s_cnt >= 5)
  {
    if(FAST_CLOCK == TRUE)
    {
     
      ertc_convert_to_real_time();
      ertc_convert_to_str();
    }
    else
    {
      uart_init_slow_clock();
    }
    // DB_PRINT("O.k.\r\n");
    s_cnt = 0;
    SWITCH_CLOCK = TRUE;
  }
	
	handlers_generic_set_handler_FLG(e_tilt_sensor_h);
  
}

#endif




static void process_next_char_from_input(void){
	
  uint8_t rx_data;

// the actual gps input
  get_data_from_buffer_with_pnt(&rx_data);

  values_to_gps_rx_buffer(rx_data);
  
  

}

#if 1

static void f_tilt_sensor_to_check(void){
  
  update_detector_position_state_handler();

}


static void f_gps_on(void){
  
  DB_PRINT("GPS_ON\r\n");
  
  gps_startup_initializer();
  
  // set up the GPS for reception --> bla bla, timeout timer, etc...
  // TODO:
  // UART_on
  // TIMEout timer on
  
  
  
  
}

static void f_gps_test_rx(void){
  
  gps_state_t gps_state;
  // depending on the gd_state we decide what is goping to happen -->
  // if there is the activation state --> well send depending on that
  gps_state = gps_check_gps_error_status();
  
  if(gd_states_get_state() == E_GPS_CHECK_ON_ACTIVATION)
  {
    if(gps_state == GPS_SENTENCE_RECEIVING)
    {
      messages_before_transmission(e_Activation);
    }
    else
    {
      messages_before_transmission(e_No_gps);
    }
    
    gd_states_switch_to_next_state(E_TRANSMISSION_STATE);
    
#if DEBUGGING_IS_ON
  UART_CRLF;
  DB_PRINT(&sentence_buffer.gps_buffer[0]);
  UART_CRLF;
#endif    
    
  }
  
  
}


static void f_gps_has_position(void){
  
  
  // well, then we need to do all the things blablabla..
  
  
  gps_stop();
  // we have a lock! great now ...
  
  // * and then extract all the importan tinformation towards the necessary structures
  // * calculate the sleep time
  // * prepare the message allready as far as possible
  // * 
// ("GPS has position!\r\n");
// #endif    
  // TODO swap state to --> Pre tx wait or sleep
  // gd_states_switch_to_next_state();

  
}


static void f_prepare_msg(void){
  
  // well, what are the possibilitys here actually --> 
  // we would need to know what message and that would depend on where we are coming from
  
  
  
  
}



static void f_rx_luz_com_handler(void){
  
#if COMPILE_WITH_RX_LUZ  
  check_on_rx_luz();
#endif
  
  gd_states_switch_to_next_state(E_STARTUP_STATE);
  
}


// this only happens when exiting sleep mode,
// therefore we just need the most basic things to start up, namely tmr4
static void f_gd_on(void){
  
  // reset all handlers because there should not be any allready active
  Handler_FLGS = (uint8_t)0u;
  
  // tilt_sensor_init();
  
  if(FAST_CLOCK == FALSE)
  {
    
    init_clock();
    configure_tmr4();
    
  }
  
  startup();
  
#if DEBUGGING_IS_ON  

  uart_init_cfg(B57600);
  RX_IF = FALSE;
	RX_IE = TRUE;
#endif
  
  // TMR4_IF = FALSE;
  // TMR4_IE = TRUE;
  
  // PERIPHERIC_IE = TRUE;
	// GLOBAL_IE = TRUE;
  // TMR4_ON = TRUE;
  
  eRTC_clock_reset();
  
  DB_PRINT("f_gd_on");
  
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

#if DEBUGGING_IS_ON
  DB_PRINT("DETECTOR IS OFF\r\n");
  while(TXSTAbits.TRMT == FALSE)
  {
    // waiting loop for finisheg the transmission
  }  
#endif  
  
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

    SLEEP();


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

  // TODO: 
  // wait for clck to stailize before tx_DP_PRINT info...
  
  gd_states_switch_to_next_state(E_STARTUP_STATE);
  // DB_PRINT("SUP\r\n");

}


static void f_always_transmit(void){
  
  while(1)
  {
    
    messages_before_transmission(e_Activation);
    
    Transmite(false);
  
  }
  
}

static void fn_clock_switching(void){
  
  
  // once we enter here we are actually switching the clock
  // therefore the eRTC TMR stopped allready
  if(FAST_CLOCK == true)
  {
    DB_PRINT("C0\r\n");
    uart_init_slow_clock();
    set_slow_clock();
    configure_tmr4();
    
  }
  else
  {
    DB_PRINT("C1\r\n");
    init_clock ();
    configure_tmr4();
    // init_UART();
#if DEBUGGING_IS_ON
    uart_init_cfg(B57600);
#endif    
  }
  
  TMR4_ON = true;
  
}


#else


// static void process_next_char_from_input(void){
// }

static void f_tilt_sensor_to_check(void){
  
  

  
}



static void f_gps_on(void){
  
  
 
  
  
}

static void f_gps_has_position(void){
  

  
}



static void f_prepare_msg(void){
  
  
  
  
  
}





// this only happens when exiting sleep mode,
// therefore we just need the most basic things to start up, namely tmr4
static void f_gd_on(void){
  

  
}



// this one is (almost) working
static void f_gd_off(void){
 
  
}




static void fn_clock_switching(void){
  
  
  
}

#endif








static void err_handler_output(void){
	
	
	DB_PRINT("\r\n UD ERROR !\r\n");
	while(1);
	
}


static void empty_function(void){
  
#if DEBUGGING_IS_ON  
  
	return;
  
#else
  
return; //  wat?
  
#endif  

}




#if DEBUGGING_IS_ON

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


#endif













//  * * * * * * * * * * * * * * * * * * *     U N U S E D   S T U F F     * * * * * * * * * * * * * * * * * * * * * * * * 
//  * * * * * * * * * * * * * * * * * * *     U N U S E D   S T U F F     * * * * * * * * * * * * * * * * * * * * * * * * 
//  * * * * * * * * * * * * * * * * * * *     U N U S E D   S T U F F     * * * * * * * * * * * * * * * * * * * * * * * * 




// EOF