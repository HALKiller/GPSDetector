// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#line 6 "handlers.c"
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
#include "detector.h"
#include "DDS.h"
#include "messages.h"

#include <stdint.h>

// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  
#ifndef FILE_HANDLERS_DB_ENABLED
#define FILE_HANDLERS_DB_ENABLED 0
#endif
#if FILE_HANDLERS_DB_ENABLED
#define DB_PRINT(str) G_DB_PRINT(str)
#define UART_int(var) G_UART_INT(var)
#else
#define DB_PRINT(str)
#define UART_int(var)
#endif
// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  


#define USE_FUNC_PNT_HANDLER 1




//  **********************  PRIVATE FUNCTIONS PROTOTYPES  ************************  //

// static void set_handler_FLG(uint8_t handler_flg_spot);
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
static void set_rtc_alarm(uint16_t settime);

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







//  **********************  STATIC DATA DECLARATIONS  ************************  //

static volatile uint16_t Handler_FLGS = 0;



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
  
  // char handler_string[4];
  // because we are not returning from this function ever we can reset the Stack Pointer to
  // its reset value and have the full 16 level Hardware stack available again
  STKPTR = 0x1Fu;
#if 0  
  handler_string[0] = ' ';
  handler_string[2] = ' ';
  handler_string[3] = NULL_TERMINATOR;
#endif


  while(1)
  {

    mask = 1u;
    
    handler_id = 0u;
    
    // to avoid a WDT in cae there is so much work to do...
    CLRWDT();
    
    while(Handler_FLGS == 0u)
    {
      
      CLRWDT();
  
    }

    temp_handler_FLGS = Handler_FLGS;
    
    while((temp_handler_FLGS & mask) == 0)
    {
      
      mask = mask << 1;
      
      handler_id++;
      
    }
    
    assert(handler_id < NUM_HANDLERS);
    
    (*Handler_arr[handler_id].func)();
#if 0    
    handler_string[1] = handler_id + 65;
    
    DB_PRINT(handler_string);
#endif    
    if(handler_id != e_ring_buffer_handler)
    {
      reset_handler_FLG(handler_id);
    }

  }
}

#else
wat
#endif


void reset_ring_buffer_handler_FLG(void){
	
	reset_handler_FLG(e_ring_buffer_handler);
	
}


void handlers_generic_set_handler_FLG(uint8_t handler_set){
	
  
  bool temp_GIE = GLOBAL_IE;

	GIE = false;

#if 1

  Handler_FLGS = Handler_FLGS | (shifts[handler_set]);	// sets the bit....

#else
	
  set_handler_FLG(handler_set);

#endif 
  // restore GIE	
	GIE = temp_GIE;
  
}

void f_gps_has_time(void){

  DB_PRINT(" H ");

  if(gd_states_get_state() == E_SEARCH_POSITION_STATE)
  {
    
    eRTC_calculate_time_until_tx();
   
    if(gd.seconds_until_next_tx >= (2 * SLEEP_BEFORE_TX_SWAP_BACK_TIME))
    {
      
      set_rtc_alarm(gd.seconds_until_next_tx - SLEEP_BEFORE_TX_SWAP_BACK_TIME); // gd.rtc_alarm = gd.seconds_until_next_tx - SLEEP_BEFORE_TX_SWAP_BACK_TIME;

    }
    else
    {
      
      gps_stop();
      DB_PRINT("GOFF\r\n");
      set_rtc_alarm(gd.seconds_until_next_tx);  // ngd.rtc_alarm = gd.seconds_until_next_tx;
      RTC_ALARM_ON = true;
      set_message_for_tx(e_send_position);
      gd_states_switch_to_next_state(E_SLEEP_BEFORE_TRANSMISSION_STATE);

    }

  }
  
}

//  **********************  PRIVATE FUNCTIONS BODY  ************************  //


// PRIVATE --> these handlers are getting, set, tested and reset 
// locally here and are therefore of private nature



// the two version: 
// with function call: 8MIPS ->  12.1us, 500kHz --> 750us
// with preconditioned bitshifting inside: 8MIPS --> 7.6us, 500kHz -> 463us
// #pragma interrupt_level 1

#if 0
static void set_handler_FLG(uint8_t handler_flg_spot){


	Handler_FLGS = Handler_FLGS | (shifts[handler_flg_spot]);	// sets the bit....

}

#endif

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

  ertc_convert_to_real_time(eRTC_get_second_cnt());

  ertc_convert_to_str();
  
  DB_PRINT("G: ");
  ertc_convert_to_real_time(gps_rtc_get_second_cnt());
  ertc_convert_to_str();
  
}

#endif



static void rtc_1000ms_handler(void){
  
#if DEBUGGING_IS_ON||DEBUGGING_BB_IS_ON      
  static uint8_t sec_cnt = 10u;
#endif  

  
  if(RTC_ALARM_ON == true)
  {
    
    gd.rtc_alarm--;
    
#if DEBUGGING_IS_ON||DEBUGGING_BB_IS_ON    
  
    DB_PRINT(".");
    sec_cnt--;
    if(sec_cnt == 0u)
    {
      UART_CRLF;
      sec_cnt = 10;

      UART_int(gd.rtc_alarm);
    }

#endif  

    if(gd.rtc_alarm == 0u)
    {
      
      RTC_ALARM_ON = false;
      rtc_alarm_handler();
      
    }
  }
  
  
  if(STATUS_LED_ON == true)
  {
    detector_status_led_handler();
  }
  
#if OV_PWM_LUZ
#if COMPILE_WITH_PWM_LUZ
  if(LUZ_ENABLED == TRUE)
  {
    pwm_luz_time_update();
  }
#endif
#endif  
#if DEBUGGING_IS_ON // ||DEBUGGING_BB_IS_ON    
  if(DEBUG_FLG_PRINT_TIME == TRUE)
  {

    local_up_f1();
  }
#endif    
  
  
}


static void rtc_alarm_handler(void){
  
  DB_PRINT("\r\n");
  switch(gd_states_get_state())
  {
    case E_SLEEP_BEFORE_SEARCH_STATE:
 
      DB_PRINT("C1");
      SWITCH_CLOCK = true;

      gd_states_switch_to_next_state(E_SEARCH_POSITION_STATE);
      
    break;
    case E_SEARCH_POSITION_STATE:
    
      // well, we did not find a position in time it seems --> we are transmitting now what??
      // retransmit last position and all the other thigns from here...
      // when we get here the tx_moment is still GPS_OFF_TIME_SAFE_SYNC seconds away -->
      // becasue of that we are adding here the next rtc_alarm because that has to be the correct time for tx
      // messages_before_transmission();
      // TODO: we still would need to switch off all the stuff we dont need...
      
      
      gps_stop();
            
#if NO_SLEEP_TILL_BROOKLYN   
   
      if(VALID_POS_RECEIVED == true)
      {
        
        // gps_calculate_lock_time();
        copy_position_from_to(RECOVERPOSITION);
        set_message_for_tx(e_send_position);
        
      }
      else
      {
        set_max_lock_time();
      
        gps_calculate_lock_time();
     
        if((gd.no_position_cnt < MAXIMUM_RESENT_SAME_POSITION) && (COPY_POS_IS_VALID == true))
        {
          
          // copy old position and create set it up for transmission...
          copy_position_from_to(RECOVERPOSITION);
          set_message_for_tx(e_resend_position);
        }
        else
        {
          set_message_for_tx(e_No_gps);
          COPY_POS_IS_VALID = false;
        }
        
        gd.no_position_cnt++;
      
      }
      
      
#else 

  

      
      set_max_lock_time();
      
      gps_calculate_lock_time();
   
      if((gd.no_position_cnt < MAXIMUM_RESENT_SAME_POSITION) && (COPY_POS_IS_VALID == true))
      {
        
        // copy old position and create set it up for transmission...
        copy_position_from_to(RECOVERPOSITION);
        set_message_for_tx(e_resend_position);
      }
      else
      {
        set_message_for_tx(e_No_gps);
        COPY_POS_IS_VALID = false;
      }
      
      gd.no_position_cnt++;
      
      
      
#endif      
#if GPS_OFF_BEFORE_TX
#if NO_SLEEP_TILL_BROOKLYN

      set_rtc_alarm(GPS_OFF_TIME_SAFE_SYNC);  // gd.rtc_alarm = GPS_OFF_TIME_SAFE_SYNC;
      
      gd_states_switch_to_next_state(E_SLEEP_BEFORE_TRANSMISSION_STATE);
      
      RTC_ALARM_ON = true;
      
#else      
  
      set_rtc_alarm(GPS_OFF_TIME_SAFE_SYNC);  // gd.rtc_alarm = GPS_OFF_TIME_SAFE_SYNC;
      
      gd_states_switch_to_next_state(E_SLEEP_BEFORE_TRANSMISSION_STATE);
      
      RTC_ALARM_ON = true;
      
#endif
#else         
      gd_states_switch_to_next_state(E_TRANSMISSION_STATE);
#endif      

      DB_PRINT("NP");


    break;
    case E_SLEEP_BEFORE_TRANSMISSION_STATE:
    // when we get here there can be two states be happening:
    // the clock can be slow or fast
    // if fast --> times up actually and we are going to transmission state
    // if slow --> we set the clock switcher and the alarm gets set again for beeping in the 
    // define Threshold time
#if NO_SLEEP_TILL_BROOKLYN

      gd_states_switch_to_next_state(E_TRANSMISSION_STATE); 
      
#else
  
      if(FAST_CLOCK == true)
      {
        gd_states_switch_to_next_state(E_TRANSMISSION_STATE); 
      }
      else
      {
        DB_PRINT("C2");
        SWITCH_CLOCK = true;
        set_rtc_alarm(SLEEP_BEFORE_TX_SWAP_BACK_TIME);  // gd.rtc_alarm = SLEEP_BEFORE_TX_SWAP_BACK_TIME;
        RTC_ALARM_ON = true;
      }
#endif
      
    break;
    default:
      assert(false);
    break;
    
    
  }

  DB_PRINT("\r\n");
  
  
}



static void rtc_200ms_handler(void){

  static uint8_t db_cnt = 0;

#if !OV_PWM_LUZ
#if COMPILE_WITH_PWM_LUZ
  if(LUZ_ENABLED == TRUE)
  {
    pwm_luz_time_update();
  }
#endif
#endif

#if 0
  db_cnt++;
  if(db_cnt == 5)
  {
    db_cnt = 0;
    update_tilt_sensor_state();
  }
#else
  
  update_tilt_sensor_state();
  
#endif

 
}


static void process_next_char_from_input(void){
	
  uint8_t rx_data;

// the actual gps input
  get_data_from_buffer_with_pnt(&rx_data);

  values_to_gps_rx_buffer(rx_data);

}



#if 1

// VALID_POS_RECEIVED

// E_SEARCH_POSITION_STATE handler here
static void f_gps_on(void){
  
  // DB_PRINT("GPS_ON\r\n");
  VALID_POS_RECEIVED = false;
 
  if(RTC_TIME_IS_GOOD == true)
  {
    eRTC_calculate_time_until_tx();
    
    if(gd.seconds_until_next_tx > GPS_OFF_TIME_SAFE_SYNC)
    {
      set_rtc_alarm(gd.seconds_until_next_tx - GPS_OFF_TIME_SAFE_SYNC);  // gd.rtc_alarm = gd.seconds_until_next_tx - GPS_OFF_TIME_SAFE_SYNC;
    }
    else
    {
      set_rtc_alarm(gd.time_between_tx + gd.seconds_until_next_tx - GPS_OFF_TIME_SAFE_SYNC);
    }
    // TODO: we need to set here somthing...
    RTC_ALARM_ON = true;
  }
  else
  {
#if 1 // DB_07102026
    gd.seconds_until_next_tx = 120u;  //gd.time_between_tx;
    set_rtc_alarm(gd.seconds_until_next_tx - GPS_OFF_TIME_SAFE_SYNC); // gd.rtc_alarm = gd.seconds_until_next_tx - GPS_OFF_TIME_SAFE_SYNC;
    RTC_ALARM_ON = true;
    
     DB_PRINT(" G24 ");
    
#elif DB_V69_PCB
    gd.seconds_until_next_tx = 240u;  //gd.time_between_tx;
    set_rtc_alarm(gd.seconds_until_next_tx - GPS_OFF_TIME_SAFE_SYNC); // gd.rtc_alarm = gd.seconds_until_next_tx - GPS_OFF_TIME_SAFE_SYNC;
    RTC_ALARM_ON = true;
    DB_PRINT(" GPS_ON 240\r\n");
#else   
    gd.seconds_until_next_tx = 600u;  //gd.time_between_tx;
    set_rtc_alarm(gd.seconds_until_next_tx - GPS_OFF_TIME_SAFE_SYNC); // gd.rtc_alarm = gd.seconds_until_next_tx - GPS_OFF_TIME_SAFE_SYNC;
    RTC_ALARM_ON = true;
    DB_PRINT(" G600\r\n");
#endif    
  }
  
  gps_startup_initializer();

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

      if(TILT_SENSOR_ERR == true)
      {
        detector_status_led_cnt_on(LED_RED_BLINKS);
      }
      else
      {
        detector_status_led_cnt_on(LED_GREEN_ON);
      }
 
    }
    else
    {
      set_message_for_tx(e_No_gps);

      detector_status_led_cnt_on(LED_RED_BLINKS); // LED_RED_ON

    }
    
    // added: 16072025:
    gps_stop();

    gd_states_switch_to_next_state(E_TRANSMISSION_STATE);

  }
  
}

#if NO_SLEEP_TILL_BROOKLYN

static void f_gps_has_position(void){

  DB_PRINT(" F ");

  if(gd_states_get_state() == E_SEARCH_POSITION_STATE)
  {
    
    eRTC_calculate_time_until_tx();

    gd.no_position_cnt = 0;
  
    copy_position_from_to(SAVEPOSITION);
    
    VALID_POS_RECEIVED = true;
    
   // TODO: check on 0 and transmit state i think actually
    if(gd.seconds_until_next_tx > (GPS_OFF_TIME_SAFE_SYNC))
    {
      
      set_rtc_alarm(gd.seconds_until_next_tx - GPS_OFF_TIME_SAFE_SYNC); // gd.rtc_alarm = gd.seconds_until_next_tx - SLEEP_BEFORE_TX_SWAP_BACK_TIME;

    }
    else
    {
      
      gps_stop();
      DB_PRINT("GOFF\r\n");
      if(gd.seconds_until_next_tx == 0)
      {
        RTC_ALARM_ON = false;
        set_message_for_tx(e_send_position);
        gd_states_switch_to_next_state(E_TRANSMISSION_STATE);        
      }
      else
      {
        set_rtc_alarm(gd.seconds_until_next_tx);  // ngd.rtc_alarm = gd.seconds_until_next_tx;
        RTC_ALARM_ON = true;
        set_message_for_tx(e_send_position);
        gd_states_switch_to_next_state(E_SLEEP_BEFORE_TRANSMISSION_STATE);        
      }
    }
  }
}


#elif RUN_GPS_TILL_TX

static void f_gps_has_position(void){

  DB_PRINT(" F ");

  if(gd_states_get_state() == E_SEARCH_POSITION_STATE)
  {
    
    eRTC_calculate_time_until_tx();

    gd.no_position_cnt = 0;
  
    copy_position_from_to(SAVEPOSITION);
   
    if(gd.seconds_until_next_tx >= (2 * SLEEP_BEFORE_TX_SWAP_BACK_TIME))
    {
      
      set_rtc_alarm(gd.seconds_until_next_tx - SLEEP_BEFORE_TX_SWAP_BACK_TIME); // gd.rtc_alarm = gd.seconds_until_next_tx - SLEEP_BEFORE_TX_SWAP_BACK_TIME;

    }
    else
    {
      
      gps_stop();
      DB_PRINT("GOFF\r\n");
      set_rtc_alarm(gd.seconds_until_next_tx);  // ngd.rtc_alarm = gd.seconds_until_next_tx;
      RTC_ALARM_ON = true;
      set_message_for_tx(e_send_position);
      gd_states_switch_to_next_state(E_SLEEP_BEFORE_TRANSMISSION_STATE);

    }

  }
  
}


#else
  
static void f_gps_has_position(void){

  gps_stop();
  
  gps_calculate_lock_time();
  
  eRTC_calculate_time_until_tx();
  
  DB_PRINT("GPS_OFF\r\n");
  
  gd.no_position_cnt = 0;
  
  if(gd_states_get_state() == E_SEARCH_POSITION_STATE)
  {
    
    copy_position_from_to(SAVEPOSITION);
   
    if(gd.seconds_until_next_tx >= (2u * SLEEP_BEFORE_TX_SWAP_BACK_TIME))
    {
      
      set_rtc_alarm(gd.seconds_until_next_tx - SLEEP_BEFORE_TX_SWAP_BACK_TIME);  // gd.rtc_alarm = gd.seconds_until_next_tx - SLEEP_BEFORE_TX_SWAP_BACK_TIME;
      DB_PRINT("\r\nCLCK_3\r\n");
      SWITCH_CLOCK = true;
    }
    else
    {
      set_rtc_alarm(gd.seconds_until_next_tx);  // gd.rtc_alarm = gd.seconds_until_next_tx;
    }
    
    RTC_ALARM_ON = true;
    set_message_for_tx(e_send_position);
    gd_states_switch_to_next_state(E_SLEEP_BEFORE_TRANSMISSION_STATE);

  }
  
}


#endif



// transmit state
static void f_prepare_msg(void){

#if SEND_ALL_MESSAGES_FOR_TESTING   
  uint8_t hlooper = 0;
  
  for(hlooper = 0; hlooper < e_NUM_MSG; hlooper++)
  {
    
    set_message_for_tx(hlooper);
    
    CLRWDT();
    
      // well, what are the possibilitys here actually --> 
  // we would need to know what message and that would depend on where we are coming from
#if !CREATE_TX_MESSAGE_AFTER_DDS_CFG    
    messages_before_transmission();
#endif


    Transmite(false);
    
    
  }
  CLRWDT();
  
#endif  
  
  // well, what are the possibilitys here actually --> 
  // we would need to know what message and that would depend on where we are coming from
#if !CREATE_TX_MESSAGE_AFTER_DDS_CFG    
    messages_before_transmission();
#endif
  
  WDTCONbits.SWDTEN = FALSE;

  Transmite(false);
 
#if DEBUGGING_BB_IS_ON&&0  
  DB_PRINT("Bat_ADC: ");
  UART_int(gd.db_adc_value);
#endif  

  measure_bat_for_batcnt(E_TRANSMISSION_STATE);
  
  WDTCONbits.SWDTEN = TRUE;
 
  if(gd_states_get_last_state() == E_GPS_CHECK_ON_ACTIVATION)
  {
    // TODO --> at some stage we would need to transmit something...
    gd_states_switch_to_next_state(E_SEARCH_POSITION_STATE);
  }
  else
  {
    gd_states_switch_to_next_state(E_SLEEP_BEFORE_SEARCH_STATE);

    set_lpm_ioports();


    
    // TODO: that might be different if we did not get a valid lock on the position the last time!
    // calculate the sleep before search time depending on alöl the possible things and then set it up
  }

#if DEBUGGING_BB_IS_ON&&0 

  ertc_convert_to_real_time(eRTC_get_second_cnt());
  ertc_convert_to_str();
  
  // and on return we should look if we can go to sleep or if we are going to search position
  DB_PRINT("\r\nTx_done\r\n");

#endif
  
}


#if COMPILE_SINGLE_FUNCTION_FOR_TESTING

static void f_rx_luz_com_handler(void){
 

#if 1
  // testing all IO_PORTS and its related functions on single
  // activation --> Debugging RF Transmission
  
  uint8_t port_setter = 0u;
  uint8_t pin_setter = 0u;
  uint8_t setting = 0;
  while(1)
  {
    
    while(MCLR == 1)
    {
      CLRWDT();
      // DB_PRINT("High\r\n");
      // my_delay_ms(1000);
    }
    DB_PRINT("LOW\r\n");
    my_delay_ms(2000);
    
    
    
    if(pin_setter >= 8)
    {
      pin_setter = 0;
      port_setter++;
      if(port_setter > 2)
      {
        port_setter = 0;
      }
    }
    switch(port_setter)
    {
      case 0:
      
        LATA = 1 << pin_setter;
        DB_PRINT("PORT A: PIN: ");
        uart_hex(LATA);
      break;
      case 1:
      
        setting = 1 << pin_setter;
        setting = setting | 0x40;
        LATB = setting;
        DB_PRINT("PORT B: PIN: ");
        uart_hex(LATB);      
      break;
      case 2:
        LATC = 1 << pin_setter;
        DB_PRINT("PORT C: PIN: ");
        uart_hex(LATC);      
      break;
      

    }
    UART_CRLF;
    
    pin_setter++;
    
    
    
    
  }








#elif 0 
// testing the Green and red led
  while(1)
  {
    
    detector_status_led_cnt_on(LED_RED_ON);
    CLRWDT();
    my_delay_ms(1000);
    
    
    detector_status_led_cnt_on(LED_GREEN_ON);
    CLRWDT();
    my_delay_ms(1000);
    
      detector_status_led_cnt_on(ALL_LED_OFF);
    CLRWDT();
    my_delay_ms(1000);

  }
#endif


  
}



#else
  
static void f_rx_luz_com_handler(void){
  
#if COMPILE_WITH_RX_LUZ  
  check_on_rx_luz();
#endif
  
  WDTCONbits.SWDTEN = TRUE;
  
  gd_states_switch_to_next_state(E_STARTUP_STATE);
#if EMULATE_GPS_TIME_POSITION    
  RTC_TIME_IS_GOOD = false;
#endif  
  
}

#endif

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

  DB_PRINT("GD_s\r\n");
  

}




#if USE_SPI_TILT

 // looking how low the consumption would be with low clck...

static void f_gd_off(void){
  
  // when we enter here we do NOT need to take care of the oscillator timing related switch over
  // because the WDT clock is so unreliable that we are not further bothered...
  // --> that is going to be taken care of by the watch dog timer and sleep instruction...

  DB_PRINT("GD_O\r\n");

  if(FAST_CLOCK == true)
  {
    fn_clock_switching();
  }


#if 1
 
  gps_stop();
  
  PERIPHERIC_IE = FALSE;
	GLOBAL_IE = FALSE;
  TMR4_IE = FALSE;
  TMR4_ON = FALSE;
  TMR2_ON = FALSE;
  TMR6_ON = FALSE;
  TMR1_ON = FALSE;
  
#endif


  set_lpm_ioports();
  

  // and now set all the super low power things so that there is almost no consumption...
  // and jsut checking the Input pin for activation
  
  
  // TODO: a direct call on wake up to the inclination sensor and afterwards test the state --> 
  // if the state changed to activation we exit and start that 
  
  while(update_tilt_sensor_state() == false)
  {
    
    WDTCONbits.WDTPS =  WDT_TIMEOUT_001s_timeout;  // WDT_TIMEOUT_512ms_timeout; //

    WDTCONbits.SWDTEN = 0x01u;  // wdton = true

    SLEEP();
  
  }

  init_wdt();


// startup state and for that we will need speed in the clock
// but we do not need to consider the time keeping becaue we
// are not having a valid rtc here
  if(FAST_CLOCK == FALSE)
  {
    
    init_clock();
    configure_tmr4();
    
  }
  
  gd_states_switch_to_next_state(E_STARTUP_STATE);
  
  DB_PRINT("SUP\r\n");

}


#else
  

static void f_gd_off(void){
  
  // when we enter here we do NOT need to take care of the oscillator timing related switch over
  // because the WDT clock is so unreliable that we are not further bothered...
  // --> that is going to be taken care of by the watch dog timer and sleep instruction...

  if(FAST_CLOCK == false)
  {
    
    init_clock();
    configure_tmr4();
    
  }

  DB_PRINT("GD OFF\r\n");

  gps_stop();
  
  PERIPHERIC_IE = FALSE;
	GLOBAL_IE = FALSE;
  TMR4_IE = FALSE;
  TMR4_ON = FALSE;
  
  set_lpm_ioports();

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
  
  INTCONbits.INTE = FALSE;

  gd_states_switch_to_next_state(E_STARTUP_STATE);
  
  DB_PRINT("SUP\r\n");

}

#endif



#if COMPILE_FOR_RELEASE

static void f_always_transmit(void){
  
  
  
  while(1)
  {
    
    set_message_for_tx(e_Activation);
    
#if !CREATE_TX_MESSAGE_AFTER_DDS_CFG    
    messages_before_transmission();
#endif
    
    
    if(STATUS_LED_ON == false)
    {
      STATUS_LED_GREEN_ON();
      STATUS_LED_RED_OFF();
    }
    else
    {
      STATUS_LED_RED_ON();    
      STATUS_LED_GREEN_OFF();
    }

    STATUS_LED_ON = !STATUS_LED_ON;
    
    Transmite(false);
    
    CLRWDT();
    
    
  }
  
}


#elif SEND_ONLY_ADC_VALUE

static void f_always_transmit(void){
  
  while(1)
  {
    

    
    measure_bat_for_batcnt(E_TRANSMISSION_STATE);
   
    CLRWDT();
    my_delay_ms(100);
    
    measure_bat_for_batcnt(E_STARTUP_STATE);
    
  }
  
}

#else

static void f_always_transmit(void){
  
  while(1)
  {
    
    set_message_for_tx(e_Activation);
    
#if !CREATE_TX_MESSAGE_AFTER_DDS_CFG    
    messages_before_transmission();
#endif

    Transmite(false);
    
    CLRWDT();
    
    my_delay_ms(1000);
    
    measure_bat_for_batcnt(E_TRANSMISSION_STATE);
    
    
    set_message_for_tx(e_No_gps);
    
#if !CREATE_TX_MESSAGE_AFTER_DDS_CFG    
    messages_before_transmission();
#endif

    Transmite(false);
    
    CLRWDT();
    
    my_delay_ms(1000);
    
    measure_bat_for_batcnt(E_STARTUP_STATE);
    
  }
  
}

  
#endif


static void fn_clock_switching(void){
  

  // once we enter here we are actually switching the clock
  // therefore the eRTC TMR stopped allready
  if(FAST_CLOCK == true)
  {
  
#if DEBUGGING_BB_IS_ON
    DB_PRINT("\r\nC3\r\n");
#endif       

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


#endif



// the sleep before search state
static void f_setup_sleep_before_search(void){

  uint16_t locker = gps_get_average_lock_time();

  locker = locker * (10u + GPS_LOCK_TIME_DECIMO_PERCENTAGER) / 10u;
  
  // to avoid that the gps_on before transmission gets to low we check if it smaller than the
  // minimum time we have set in the config.... 
  if(locker < MINIMUM_GPS_ON_BEFORE_TRANSMISSION)
  {
    locker = MINIMUM_GPS_ON_BEFORE_TRANSMISSION;
  }
  
  // well --> lets calculate the time for sleep, 
  // set it up and clock down...
  eRTC_calculate_time_until_tx();
  
  
  // because on low bat we wait longer, and if it is not set for Tfijo
  if((BAT_IS_LOW_FLG == true) && (DOUBLE_PERIOD == true))
  {
    gd.seconds_until_next_tx = gd.seconds_until_next_tx + gd.time_between_tx;
  }
  
 
  if(BAT_IS_TOO_LOW == true)
  {
    gd.seconds_until_next_tx = SECONDS_PER_HOUR - (2u * locker);
  }
  
   
  if(gd.seconds_until_next_tx > locker)
  {
    
    set_rtc_alarm(gd.seconds_until_next_tx - locker);  //  gd.rtc_alarm = gd.seconds_until_next_tx - locker; 

    DB_PRINT("\r\nC4\r\n");
    SWITCH_CLOCK = true;
    
    gd_states_set_next_state(E_SEARCH_POSITION_STATE);

  }
  else
  {
    set_rtc_alarm(gd.seconds_until_next_tx);  // gd.rtc_alarm = gd.seconds_until_next_tx;
    gd_states_switch_to_next_state(E_SEARCH_POSITION_STATE);
  }

#if DEBUGGING_BB_IS_ON  

  DB_PRINT("S2tx: ");
  UART_int(gd.seconds_until_next_tx);
  UART_CRLF;
  DB_PRINT("S_lock: ");
  UART_int(locker);
  UART_CRLF;
  DB_PRINT("rtc_alarm: ");
  UART_int(gd.rtc_alarm);
  UART_CRLF;
  
#endif  
  
  RTC_ALARM_ON = true;
  


}

static void set_rtc_alarm(uint16_t settime){
  
  
  gd.rtc_alarm = settime;
  
  DB_PRINT("\r\nAS: ");
  UART_int(settime);
  
}

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