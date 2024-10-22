// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //

//  The high level and inremediate state levels
//  // the on_exit functions are setting the next state --> therefore they are handling to where the program flows to -->
// therefore all the decisions are taken here in refereence to the high level states
//
//
//
//
//
//
//
//


//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //

#include "gd_states.h"

#include "assert.h"

#include "handlers.h"

// #include "tmr_handlers.h"

#include "stddef.h"

// #include "common_utils.h"

// #include "sleep_handler.h"

#include "Global.h"

// #include "rtc.h"

#include <stdint.h>



//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

typedef struct udt_state_m_type{
  
  uint8_t detector_is_on; // it is turned over or not ;
  uint8_t actual_state;
  uint8_t last_state;
  e_gpsd_substate_t gd_substate;
  
  
}gpsd_state_t;


//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 
static gpsd_state_t detector_state;
 
 
//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 

#define STANDARD_SLEEP_TIME_DEBUGGING 1 // 4 seconds --> nice!!




//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //

const char *app_txt[] = {
  
  "E_RESET_STATE",
  "E_LUZ_COM_STATE",
  "E_STARTUP_STATE",
  "E_SEARCH_POSITION_STATE",
  "E_OFF_STATE",
  "E_TRANSMISSION_STATE",
  "E_SLEEP_STATE",
  "E_>",
  
};

const char *app_txt_2[] = {
  
  "E_GPS_CHECK_ON_ACTIVATION",
  "E_GPS_SEARCHES_FOR_POSITION",
  "E_TRANSMIT_MESSAGES",
  "E_TILT_SENSOR_IS_ON",
  "E_TILT_SENSOR_IS_OFF",
  "E_CHECK_ON_TILT_SENSOR",
  "E_NUM_SUB",
  
  
};


//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //
// static void put_gd_detector_into_off_state(void);
// static void wake_detector_from_off_state(void);


// Array functions
static void f_E_RESET_STATE_handler(void);
static void f_E_LUZ_COM_STATE_handler(void);
static void f_E_STARTUP_STATE_handler(void);
static void f_E_SEARCH_POSITION_STATE_handler(void);
static void f_E_OFF_STATE_handler(void);
static void f_E_TRANSMISSION_STATE_handler(void);
static void f_E_SLEEP_STATE_handler(void);
// Array functions
static void f_on_exit_E_RESET_STATE_handler(void);
static void f_on_exit_E_LUZ_COM_STATE_handler(void);
static void f_on_exit_E_STARTUP_STATE_handler(void);
static void f_on_exit_E_SEARCH_POSITION_STATE_handler(void);
static void f_on_exit_E_OFF_STATE_handler(void);
static void f_on_exit_E_TRANSMISSION_STATE_handler(void);
static void f_on_exit_E_SLEEP_STATE_handler(void);

// Array of function pointers
void (*exitstateHandlers[E_NUM_STATES])() = {
  f_on_exit_E_RESET_STATE_handler,
  f_on_exit_E_LUZ_COM_STATE_handler,
  f_on_exit_E_STARTUP_STATE_handler,
  f_on_exit_E_SEARCH_POSITION_STATE_handler,
  f_on_exit_E_OFF_STATE_handler,
  f_on_exit_E_TRANSMISSION_STATE_handler,
  f_on_exit_E_SLEEP_STATE_handler,
};

// Array of function pointers
void (*stateHandlers[E_NUM_STATES])() = {
    f_E_RESET_STATE_handler,
    f_E_LUZ_COM_STATE_handler,
    f_E_STARTUP_STATE_handler,
    f_E_SEARCH_POSITION_STATE_handler,
    f_E_OFF_STATE_handler,
    f_E_TRANSMISSION_STATE_handler,
    f_E_SLEEP_STATE_handler
};

//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //


void gd_states_initialize(void){
  
  detector_state.last_state = E_RESET_STATE,
  detector_state.actual_state = E_RESET_STATE,
  detector_state.detector_is_on = false;
  gd_states_set_gpsd_substate(E_CHECK_ON_TILT_SENSOR);
  
}



// this function just sets the next state...
// threfore it worries only about the actual state and which is going to be the next one...
void gd_states_switch_to_next_state(void){
  
  detector_state.last_state = detector_state.actual_state;
#if SEND_APP_STRINGS  
  APP_PRINT("The last state was: %s \r\n", (const char*)app_txt[detector_state.actual_state]);
#endif


  if (detector_state.actual_state < E_NUM_STATES)
  {
    exitstateHandlers[detector_state.actual_state]();
  }
  else
  {
    assert(false);
  }
#if SEND_APP_STRINGS    
  APP_PRINT("The actual state is: %s \r\n", (const char*)app_txt[detector_state.actual_state]);
  APP_PRINT("The actual sub state is: %s \r\n", (const char*)app_txt_2[detector_state.gd_substate]);
#endif  
}


// Becasue transitioning from the different states depend on some substates --> 
// therefore we are creating this one here to help keep track of these changes
void gd_states_set_gpsd_substate(e_gpsd_substate_t substate){
  
  detector_state.gd_substate = substate;
#if SEND_APP_STRINGS   
  APP_PRINT("Sub state is: %s \r\n", (const char*)app_txt_2[detector_state.gd_substate]);
#endif 
}


e_gpsd_substate_t gd_states_get_substate(void){
  
  return detector_state.gd_substate;
  
}

// and this function takes care than to set, reset configure everything for the new state
// basicall the first time we get into the main loop under the new state setting...
void gd_state_change_handler(void){
  
  if (detector_state.actual_state < E_NUM_STATES)

  {
    stateHandlers[detector_state.actual_state]();
  }
  else
  {
    assert(false);
  }
  
  detector_state.last_state = detector_state.actual_state;

}


uint8_t gd_states_did_change(void){
    
  return !(detector_state.last_state == detector_state.actual_state);
  
}
//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //

// ---------------------  the on_exit functions ----------------------------------

// E_RESET_STATE
static void f_on_exit_E_RESET_STATE_handler(void){
  
  detector_state.actual_state = E_LUZ_COM_STATE;
  
}

// E_LUZ_COM_STATE
static void f_on_exit_E_LUZ_COM_STATE_handler(void){
  
  detector_state.actual_state = E_STARTUP_STATE;
  
  
#if USE_RTC
  set_rtc_periodic_rate();
#else  
  // this timer gets switched on only once --> here !
  tmr_handlers_start(SLEEP_TIMER);
#endif  


}

// E_STARTUP_STATE
static void f_on_exit_E_STARTUP_STATE_handler(void){

  if(detector_state.gd_substate == E_TILT_SENSOR_IS_ON)
  {
    
    detector_state.actual_state = E_SEARCH_POSITION_STATE;
    gd_states_set_gpsd_substate(E_GPS_CHECK_ON_ACTIVATION);

  }
  else if(detector_state.gd_substate == E_TILT_SENSOR_IS_OFF)
  {
    
    detector_state.actual_state = E_OFF_STATE;
    
  }
  else
  {
    
    assert(false);  // errhandler!!! that should be impossible!
    
  }

}

// E_SEARCH_POSITION_STATE
static void f_on_exit_E_SEARCH_POSITION_STATE_handler(void){
  
  if(detector_state.gd_substate == E_TILT_SENSOR_IS_OFF)
  {
    
    detector_state.actual_state = E_OFF_STATE;
    
  }
  
  
  else if(detector_state.gd_substate == E_GPS_CHECK_ON_ACTIVATION)
  {
    // therefore the GPS is still on --> switch it off !
    // set_handler_FLG(E_GPS_OFF_h);
    detector_state.actual_state = E_TRANSMISSION_STATE;
    
    
  }
  else
  {
    
    detector_state.actual_state = E_SLEEP_STATE;
    gd_states_set_gpsd_substate(E_TRANSMIT_MESSAGES);
  }
  
}

// E_OFF_STATE
static void f_on_exit_E_OFF_STATE_handler(void){
  
  detector_state.actual_state = E_STARTUP_STATE;
  APP_PRINT("DETECTOR_IS_ON!\r\n");
}

// E_TRANSMISSION_STATE
static void f_on_exit_E_TRANSMISSION_STATE_handler(void){
  
  if(detector_state.gd_substate == E_TILT_SENSOR_IS_OFF)
  {
    
    detector_state.actual_state = E_OFF_STATE;
    
  }
  
  
  else if(detector_state.gd_substate == E_GPS_CHECK_ON_ACTIVATION)
  {

    detector_state.actual_state = E_SEARCH_POSITION_STATE;
    
  }
  else
  {
    
    detector_state.actual_state = E_SLEEP_STATE;
    
  }
  
  gd_states_set_gpsd_substate(E_GPS_SEARCHES_FOR_POSITION);
  
  
}

// E_SLEEP_STATE
static void f_on_exit_E_SLEEP_STATE_handler(void){
  
  if(detector_state.gd_substate == E_TILT_SENSOR_IS_OFF)
  {
    
    detector_state.actual_state = E_OFF_STATE;
    
  }
  
  else if(detector_state.gd_substate == E_GPS_SEARCHES_FOR_POSITION)
  {
    detector_state.actual_state = E_SEARCH_POSITION_STATE;
  }
  else
  {
    detector_state.actual_state = E_TRANSMISSION_STATE;
  }
  
  
  
}



// ---------------------  the on_entrance functions ----------------------------------

//  E_RESET_STATE,
static void f_E_RESET_STATE_handler(void){

  
  
}

//  E_LUZ_COM_STATE,
static void f_E_LUZ_COM_STATE_handler(void){
  
  set_handler_FLG(E_RX_LUZ_COM_h);
  
}

//  E_STARTUP_STATE,
static void f_E_STARTUP_STATE_handler(void){

  set_handler_FLG(E_STARTUP_h);
  // tmr_handlers_start(STANDARD_TIMER);

}

//  E_SEARCH_POSITION_STATE,
static void f_E_SEARCH_POSITION_STATE_handler(void){

  // set handler flag for gps_initialization
  set_handler_FLG(E_GPS_ON_h);
  
  
}

//  E_OFF_STATE,
static void f_E_OFF_STATE_handler(void){
  
  APP_PRINT("DETECTOR_IS_OFF!\r\n");
  
  set_handler_FLG(E_GD_OFF_h);
  // therefore we need to set the lpm_enter_handler...

}


//  E_TRANSMISSION_STATE,
static void f_E_TRANSMISSION_STATE_handler(void){

  // What we need to do:
  // * create the message
  // * check on time to transmit
  // * and on_time --> transmit!
  if(detector_state.gd_substate == E_GPS_CHECK_ON_ACTIVATION)
  {
    // Activation or no hay position
    // APP_PRINT
  }
  else
  {
    // All other messages
  }


  set_handler_FLG(E_PREPARE_MESSAGE_h);

}

//  E_SLEEP_STATE,
static void f_E_SLEEP_STATE_handler(void){
  
  // we needto know for how long
  // and that depedns if we are coming now from the transmission state or 
  // if we are coming from the search state...
 #if 0  // becaue we are setting that up on_exit from  wherever...
 if(detector_state.gd_substate == E_GPS_SEARCHES_FOR_POSITION)
  {
    
    gd_states_set_gpsd_substate(E_TRANSMIT_MESSAGES);
    
  }
  else
  {
    
    gd_states_set_gpsd_substate(E_GPS_SEARCHES_FOR_POSITION);
    
  }
#endif
  // TODO:
  // create the real calculation at some point but for the moment 
  // use the fixed timeout for the sleep timer
  // r_sleep_handler_set_sleep_time_counter(STANDARD_SLEEP_TIME_DEBUGGING);
  
  set_handler_FLG(E_LPM_START_h);
  
  set_rtc_calendar_alarm(30u);
  
}

































#if 0
static void put_gd_detector_into_off_state(void){
  
  
  
  
  
  
  
}


static void wake_detector_from_off_state(void){
  
  // get up the clock
  
  // get up the gps --> that might never have been active so far! -->
  // becasue we might had a reset condition and the detector was turned over and therefore there is no
  // valid config inside the gps handlers!!
  
  
  
}

#endif

//   * * * * * *      I S R  - -  H A N D E L E R     * * * * * * * * * * * * * *   //



// EOF