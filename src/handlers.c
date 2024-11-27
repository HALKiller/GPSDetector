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
// TODO: --> because of tmr4 config move it
#include "Init_all.h"

#include "gps.h"

#include "e_rtc.h"

#include "pwm_luz.h"

#include "DDS.h"

#include "messages.h"

#include <stdint.h>


#define USE_FUNC_PNT_HANDLER 1

#if !USE_DIRECT_CALL 
static void f_tilt_sensor_to_check(void);
#endif



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
static void rtc_200ms_handler(void);
static void rtc_1000ms_handler(void);
static void f_gps_on(void);
static void f_gps_has_position(void);
static void err_handler_output(void);
static void f_gps_test_rx(void);
static void empty_function(void);
static void f_setup_sleep_before_search(void);

static void rtc_alarm_handler(void);

#if DEBUGGING_IS_ON
static void local_up_f1(void);
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
  { e_1000ms_h,                         rtc_1000ms_handler },	
  
  { e_200ms_h,                          rtc_200ms_handler },  
  { e_gps_has_full_position_h,          f_gps_has_position },  
	{ e_ring_buffer_handler,              process_next_char_from_input },
#if !USE_DIRECT_CALL  
	{ e_tilt_sensor_h,	                  f_tilt_sensor_to_check },	
#endif	
  { e_gps_on_h,                         f_gps_on },
  { e_prepare_msg_h,                    f_prepare_msg },
  
	
	{ e_startup_h,                        f_gd_on },
  
	{ e_sleep_before_search,              f_setup_sleep_before_search },	
  { e_ertc_handler_start,               empty_function },	
	{ e_errhandler,                       empty_function },		
  { e_gps_test_reception,               f_gps_test_rx  },
	
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
// #define TIME_BASE 2
#define MS_PER_SECOND 1000
#define DB_HANDLER_CF 0
#define c_ADC_OVERSAMPLING 8
#define cADC_BATERIA_MIMIMUM_THRESHHOLD 127	// these are ADC
#define BATERIE_MINIMUM_mv_LEVEL	6000


// #define SEG_7D_REFRESH 2000

#define OF_CNT_100MS 50
#define OF_CNT_200MS 100
#define OF_CNT_1000MS 500
#define OF_CNT_500MS 250
#define OF_CNT_2000MS 1000

// #define C_OF_CNT_SEG_7D SEG_7D_REFRESH/TIME_BASE

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
      DB_LED1_SWAP;
      
    }

    temp_handler_FLGS = Handler_FLGS;
    
    while((temp_handler_FLGS & mask) == 0)
    {
      
      mask = mask << 1;
      
      handler_id++;
      
    }
    
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
      DB_LED1_SWAP;
      
    }

    temp_handler_FLGS = Handler_FLGS;
    
    while((temp_handler_FLGS & mask) == 0)
    {
      
      mask = mask << 1;
      
      handler_id++;
      
    }
    
    
    
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
          rtc_200ms_handler();
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



#if DEBUGGING_IS_ON
// this type of funciton is always only for debugging purposes...
static void local_up_f1(void){
  
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
  
}

#endif



static void rtc_1000ms_handler(void){
  
  
  // TODO: a flag which indicates if we are at the moment with some kind of doncnt timer
  
  if(RTC_ALARM_ON == true)
  {
    
    gd.rtc_alarm--; // seconds_until_next_tx--;
  
    UART_int(gd.rtc_alarm);
    UART_CRLF;
  
    if(gd.rtc_alarm == 0u)  // seconds_until_next_tx == 0)
    {

      rtc_alarm_handler();
      RTC_ALARM_ON = false;
      
    }
  }
  
#if DEBUGGING_IS_ON      
  if(DEBUG_FLG_PRINT_TIME == TRUE)
  {

    local_up_f1();
  }
#endif    
  
  DB_LED2_SWAP;
  
  
}


static void rtc_alarm_handler(void){
  
  
  switch(gd_states_get_state())
  {
    case E_SLEEP_BEFORE_SEARCH_STATE:
      handlers_generic_set_handler_FLG(e_switch_clock_handler);
      gd_states_switch_to_next_state(E_SEARCH_POSITION_STATE);
    break;
    case E_SEARCH_POSITION_STATE:
    
      // well, we did not find a position in time it seems --> we are transmitting now what??
      // retransmit last position and all the other thigns from here...
      // messages_before_transmission();
      // TODO: we still would need to switch off all the stuff we dont need...
      
      
      
      gps_stop();
      
      set_max_lock_time();
      // stop_gps_lock_time_cnt();
      
      gps_calculate_lock_time();
      
      if((gd.no_position_cnt < MAXIMUM_RESENT_SAME_POSITION) && (COPY_POS_IS_VALID == true))
      {
        // todo: check if there exists a copy of position to use...
        // copy old position and create set it up for transmission...
        copy_position_from_to(RECOVERPOSITION);
        set_message_for_tx(e_send_position);
      }
      else
      {
        set_message_for_tx(e_No_gps);
        COPY_POS_IS_VALID = false;
      }
      
      gd.no_position_cnt++;
      
      gd_states_switch_to_next_state(E_TRANSMISSION_STATE);
      DB_PRINT("No position found\r\n");
      
    break;
    case E_SLEEP_BEFORE_TRANSMISSION_STATE:
      gd_states_switch_to_next_state(E_TRANSMISSION_STATE);
    break;
    default:
      assert(false);
    break;
    
    
  }

  
  
}



#if 0


static void rtc_200ms_handler(void){


  handlers_generic_set_handler_FLG(e_tilt_sensor_h);
 
}

#elif USE_FULL_SECONDS_FOR_RTC




static void rtc_200ms_handler(void){


#if COMPILE_WITH_PWM_LUZ
  if(LUZ_ENABLED == TRUE)
  {
    pwm_luz_time_update();
  }
#endif


#if USE_DIRECT_CALL  
  
  update_tilt_sensor_state();
  
#else  
  
// measure the setting time...
  handlers_generic_set_handler_FLG(e_tilt_sensor_h);
  
#endif 
 
}


#else
  

static void rtc_200ms_handler(void){

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
      uart_init_cfg(B9600_low_clk);
      // uart_init_slow_clock();
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


#if !USE_DIRECT_CALL 
static void f_tilt_sensor_to_check(void){
  
  update_tilt_sensor_state();

}
#endif


// E_SEARCH_POSITION_STATE handler here
static void f_gps_on(void){
  
  DB_PRINT("GPS_ON\r\n");
  
  if(RTC_TIME_IS_GOOD == true)
  {
    eRTC_calculate_time_until_tx();
    gd.rtc_alarm = gd.seconds_until_next_tx;
    RTC_ALARM_ON = true;
  }
  else
  {
    // TODO: setup a starting rtc time...
  }
  
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
      set_message_for_tx(e_Activation);
      // messages_before_transmission(e_Activation);
    }
    else
    {
      set_message_for_tx(e_No_gps);
      // messages_before_transmission(e_No_gps);
    }
    
    // gd_states_set_next_state(E_SEARCH_POSITION_STATE);
    gd_states_switch_to_next_state(E_TRANSMISSION_STATE);

  }
  
  
}


static void f_gps_has_position(void){
  
  
  // well, then we need to do all the things blablabla..
  
  // TODO:
  // Calculate the lock time and move that into the array, 
  // based on that calculate the minimum gps_on time...
  
  gps_stop();
  
  gps_calculate_lock_time();
  
  // and what to do next? well, depends on what we were doinfgin first place....
  
  
  eRTC_calculate_time_until_tx();
  
  DB_PRINT("G_off\r\n");
  gd.no_position_cnt = 0;
  
  if(gd_states_get_state() == E_SEARCH_POSITION_STATE)
  {
    copy_position_from_to(SAVEPOSITION);
    DB_PRINT("G_off\r\n");
    gd.rtc_alarm = gd.seconds_until_next_tx;
    RTC_ALARM_ON = true;
    set_message_for_tx(e_send_position);
    gd_states_switch_to_next_state(E_SLEEP_BEFORE_TRANSMISSION_STATE);
    
  }
  

  
}

#if 1 

// transmit state
static void f_prepare_msg(void){

#if SEND_ALL_MESSAGES_FOR_TESTING   
  uint8_t hlooper = 0;
  
  for(hlooper = 0; hlooper < 3; hlooper++)
  {
    
    set_message_for_tx(hlooper);
    
    CLRWDT();
    
      // well, what are the possibilitys here actually --> 
  // we would need to know what message and that would depend on where we are coming from
    messages_before_transmission();
    
    Transmite(false);
    
    
  }
  CLRWDT();
  
#endif  
  
  // well, what are the possibilitys here actually --> 
  // we would need to know what message and that would depend on where we are coming from
  messages_before_transmission();
  
  Transmite(false);
  
  if(gd_states_get_last_state() == E_GPS_CHECK_ON_ACTIVATION)
  {
    gd_states_switch_to_next_state(E_SEARCH_POSITION_STATE);
  }
  else
  {
    gd_states_switch_to_next_state(E_SLEEP_BEFORE_SEARCH_STATE);
    // TODO: that might be different if we did not get a valid lock on the position the last time!
    // calculate the sleep before search time depending on alöl the possible things and then set it up
  }
  ertc_convert_to_real_time(eRTC_get_second_cnt());
  ertc_convert_to_str();
  // and on return we should look if we can go to sleep or if we are going to search position
  DB_PRINT("\r\nTx_done\r\n");

  
}


#else

// transmit state
static void f_prepare_msg(void){
  
  // well, what are the possibilitys here actually --> 
  // we would need to know what message and that would depend on where we are coming from
  messages_before_transmission();
  
  Transmite(false);
  
  if(gd_states_get_last_state() == E_GPS_CHECK_ON_ACTIVATION)
  {
    gd_states_switch_to_next_state(E_SEARCH_POSITION_STATE);
  }
  else
  {
    gd_states_switch_to_next_state(E_SLEEP_BEFORE_SEARCH_STATE);
    // TODO: that might be different if we did not get a valid lock on the position the last time!
    // calculate the sleep before search time depending on alöl the possible things and then set it up
  }
  ertc_convert_to_real_time(eRTC_get_second_cnt());
  ertc_convert_to_str();
  // and on return we should look if we can go to sleep or if we are going to search position
  DB_PRINT("\r\nTx_done\r\n");

  
}

#endif

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
  
  if(FAST_CLOCK == FALSE)
  {
    
    init_clock();
    configure_tmr4();
    
  }
  
  startup();
  
#if DEBUGGING_IS_ON  

  uart_init_cfg(DEBUG_BAUDRATE);
  RX_IF = FALSE;
	RX_IE = TRUE;
  
  
  DB_PRINT("GD_on");
#endif
  
  
  
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
  DB_PRINT("GD OFF\r\n");
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
  
  init_wdt();
  
  // WDTCONbits.SWDTEN = 0x00u;  // wdton = true
  
  
  
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
    
    set_message_for_tx(e_Activation);
    messages_before_transmission();
    
    Transmite(false);
  
  }
  
}


static void fn_clock_switching(void){
  
  
  // once we enter here we are actually switching the clock
  // therefore the eRTC TMR stopped allready
  if(FAST_CLOCK == true)
  {
    
#if DEBUGGING_IS_ON    
    // uart_init_slow_clock();

    uart_init_cfg(B9600_low_clk);
#endif       
    set_slow_clock();
    configure_tmr4();
    
  }
  else
  {
    
    init_clock ();
    configure_tmr4();
    // init_UART();
#if DEBUGGING_IS_ON
    uart_init_cfg(DEBUG_BAUDRATE);
#endif    
  }
  
  TMR4_ON = true;
  
}


#endif


#if DEBUGGING_IS_ON

// the sleep before search state
static void f_setup_sleep_before_search(void){
  

  
  uint16_t locker = gps_get_average_lock_time();

  locker = locker * (10u + GPS_LOCK_TIME_DECIMO_PERCENTAGER) / 10u;
  
  // well --> lets calculate the time for sleep, 
  // set it up and clock down the baby...
  eRTC_calculate_time_until_tx();
  
  
  // because on low bat we wait longer...
  if(BAT_IS_LOW_FLG == true)
  {
    gd.seconds_until_next_tx = gd.seconds_until_next_tx + gd.time_between_tx;
  }
  
  if(gd.seconds_until_next_tx > locker)
  {
    
    gd.rtc_alarm = gd.seconds_until_next_tx - locker; 
  
    gd_states_set_next_state(E_SEARCH_POSITION_STATE);
  
    handlers_generic_set_handler_FLG(e_switch_clock_handler);

  }
  else
  {
    gd.rtc_alarm = gd.seconds_until_next_tx;
    gd_states_switch_to_next_state(E_SEARCH_POSITION_STATE);
  }
  
  DB_PRINT("S_till_tx: ");
  UART_int(gd.seconds_until_next_tx);
  UART_CRLF;
  UART_int(locker);
  UART_CRLF;
  UART_int(gd.rtc_alarm);
  UART_CRLF;
  
  
  
  RTC_ALARM_ON = true;
  


}

#else
  
  // the sleep before search state
static void f_setup_sleep_before_search(void){
  
  // well --> lets calculate the time for sleep, 
  // set it up and clock down the baby...
  eRTC_calculate_time_until_tx();

  
  // TODO: this is just some value at the moment for debugging
  if(gd.seconds_until_next_tx > 20)
  {
    gd.rtc_alarm = gd.seconds_until_next_tx - 20;  
  }
  
  RTC_ALARM_ON = true;
  
  gd_states_set_next_state(E_SEARCH_POSITION_STATE);
  
  handlers_generic_set_handler_FLG(e_switch_clock_handler);

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