// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //






//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //

#include "gd_states.h"

#include "my_assert.h"

#include "handlers.h"

#include "UART.h"

#include "stddef.h"

#include "Global.h"

#include <stdint.h>


// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  
#ifndef FILE_GD_STATES_DB_ENABLED
#define FILE_GD_STATES_DB_ENABLED 0
#endif
#if FILE_GD_STATES_DB_ENABLED
#define DB_PRINT(str) G_DB_PRINT(str)
#define UART_int(var) G_UART_INT(var)
#else
#define DB_PRINT(str)
#define UART_int(var)
#endif
// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  

//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

#if 1

typedef struct udt_state_m_type{
  
  // uint8_t detector_is_on; // it is turned over or not ;
  e_gpsd_states_t actual_state;
  e_gpsd_states_t last_state;
  e_gpsd_states_t next_state;
  
  
}gpsd_state_t;


#else
  


typedef struct udt_state_m_type{
  
  uint8_t detector_is_on; // it is turned over or not ;
  uint8_t actual_state;
  uint8_t last_state;
  // e_gpsd_substate_t gd_substate;
  
  
}gpsd_state_t;

#endif

//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 
static gpsd_state_t detector_state;
 
 
//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 
#define USE_LOCAL_F_PNT 1

#define STANDARD_SLEEP_TIME_DEBUGGING 1 // 4 seconds --> nice!!

#define SEND_APP_STRINGS 1


//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //


#if 0

  static const char *app_txt[] = {
    
    "E_RESET_STATE\r\n",
    "E_LUZ_COM_STATE\r\n",
    "E_STARTUP_STATE\r\n",
    "E_GPS_CHECK_ON_ACTIVATION\r\n",
    "E_TRANSMISSION_STATE\r\n",
    "E_SEARCH_POSITION_STATE\r\n",
    "E_SLEEP_BEFORE_TRANSMISSION_STATE\r\n",
    "E_SLEEP_BEFORE_SEARCH_STATE\r\n",
    "E_OFF_STATE\r\n",
    "E_>\r\n",
    
  };

#elif 1

  const char * const app_txt[] = {
    
    "E0",
    "E1",
    "E2",
    "E3",
    "E4",
    "E5",
    "E6",
    "E7",
    "E8",
    "E9",
    
  };

#else
  

#endif



//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //


// Array functions
static void f_E_RESET_STATE_handler(void);
static void f_E_LUZ_COM_STATE_handler(void);
static void f_E_STARTUP_STATE_handler(void);
static void f_E_TRANSMISSION_STATE_handler(void);
static void f_E_SEARCH_POSITION_STATE_handler(void);
static void f_E_GPS_CHECK_ON_ACTIVATION_handler(void);
static void f_E_SLEEP_BEFORE_TRANSMISSION_STATE_handler(void);
static void f_E_SLEEP_BEFORE_SEARCH_STATE_handler(void);
static void f_E_OFF_STATE_handler(void);



// they take up space in RAM!!!
// Array of function pointers
static void (*stateHandlers[E_NUM_STATES])() = {
  
    f_E_RESET_STATE_handler,
    f_E_LUZ_COM_STATE_handler,
    f_E_STARTUP_STATE_handler,
    f_E_GPS_CHECK_ON_ACTIVATION_handler,
    f_E_TRANSMISSION_STATE_handler,
    f_E_SEARCH_POSITION_STATE_handler,
    f_E_SLEEP_BEFORE_TRANSMISSION_STATE_handler,
    f_E_SLEEP_BEFORE_SEARCH_STATE_handler,
    f_E_OFF_STATE_handler,
    
};

//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

#if GD_STATES_OV

void gd_states_initialize(void){
  
  
  gd_states_switch_to_next_state(E_RESET_STATE);
  
  
}


e_gpsd_states_t gd_states_get_state(void){
  
  return detector_state.actual_state;
  
}

e_gpsd_states_t gd_states_get_last_state(void){
  
  return detector_state.last_state;
  
}

void gd_states_set_next_state(e_gpsd_states_t next){
  
  assert(next <= E_NUM_STATES);
  detector_state.next_state = next;
  
}

e_gpsd_states_t gd_states_get_next_state(void){
  
  return detector_state.next_state;
  
}


#if USE_LOCAL_F_PNT


// this function just sets the next state...
// threfore it worries only about the actual state and which is going to be the next one...
void gd_states_switch_to_next_state(e_gpsd_states_t next_state){
  
  assert(next_state < E_NUM_STATES);
  
  detector_state.last_state = detector_state.actual_state;
  
#if 0
  DB_PRINT("\r\nLS: ");
  DB_PRINT(app_txt[detector_state.actual_state]);
  UART_CRLF;
#endif


  stateHandlers[next_state]();
  
  detector_state.actual_state = next_state;
  
#if 0
  DB_PRINT("NS: ");
  DB_PRINT(app_txt[detector_state.actual_state]);
  UART_CRLF;
#endif  

  
}

#else

void gd_states_switch_to_next_state(e_gpsd_states_t next_state){
  
  assert(next_state < E_NUM_STATES);
  
  detector_state.last_state = detector_state.actual_state;
  
#if 0
  DB_PRINT("l_switch state: ");
  DB_PRINT(app_txt[detector_state.actual_state]);
#endif

  switch(next_state)
  {

    case E_RESET_STATE:
     f_E_RESET_STATE_handler();
    break;
    case E_LUZ_COM_STATE:
    f_E_LUZ_COM_STATE_handler();
    break;
    case E_STARTUP_STATE:
    f_E_STARTUP_STATE_handler();
    break;
    case E_GPS_CHECK_ON_ACTIVATION:
    f_E_GPS_CHECK_ON_ACTIVATION_handler();
    break;
    case E_TRANSMISSION_STATE:
    f_E_TRANSMISSION_STATE_handler();
    break;
    case E_SEARCH_POSITION_STATE:
    f_E_SEARCH_POSITION_STATE_handler();
    break;
    case E_SLEEP_BEFORE_TRANSMISSION_STATE:
    f_E_SLEEP_BEFORE_TRANSMISSION_STATE_handler();
    break;
    case E_SLEEP_BEFORE_SEARCH_STATE:
    f_E_SLEEP_BEFORE_SEARCH_STATE_handler();
    break;
    case E_OFF_STATE:
      f_E_OFF_STATE_handler();
    break;
    
    
  }

  detector_state.actual_state = next_state;
  


  
}



#endif


// ---------------------  the on_entrance functions ----------------------------------

//  E_RESET_STATE,
static void f_E_RESET_STATE_handler(void){

  detector_state.last_state = E_RESET_STATE;
  detector_state.actual_state = E_RESET_STATE;
  // detector_state.detector_is_on = FALSE;
  // gd_states_set_gpsd_substate(E_CHECK_ON_TILT_SENSOR);
  
  
}

//  E_LUZ_COM_STATE,
static void f_E_LUZ_COM_STATE_handler(void){
  
  handlers_generic_set_handler_FLG(e_rx_luz_com_h);
  
}

//  E_STARTUP_STATE,
static void f_E_STARTUP_STATE_handler(void){


  handlers_generic_set_handler_FLG(e_startup_h);


}

//  E_SEARCH_POSITION_STATE,
static void f_E_SEARCH_POSITION_STATE_handler(void){

  // set handler flag for gps_initialization
  handlers_generic_set_handler_FLG(e_gps_on_h);
  
  
}


static void f_E_GPS_CHECK_ON_ACTIVATION_handler(void){
  
  // TODO: 
  // set flag indicating that we are on activation
  
  handlers_generic_set_handler_FLG(e_gps_on_h);
  
}

static void f_E_SLEEP_BEFORE_TRANSMISSION_STATE_handler(void){
  // TODO:
  // get into LP Mode and set flag indicating where to go from there
  handlers_generic_set_handler_FLG(e_ertc_handler_start);

  set_lpm_ioports();    
 
}


static void f_E_SLEEP_BEFORE_SEARCH_STATE_handler(void){
  // TODO:
  // get into LP Mode and set flag indicating where to go from there
  handlers_generic_set_handler_FLG(e_sleep_before_search);
}


//  E_OFF_STATE,
static void f_E_OFF_STATE_handler(void){
  
  // DB_PRINT("DETECTOR_IS_OFF!\r\n");
  
  handlers_generic_set_handler_FLG(e_gd_off_h);
  

}


//  E_TRANSMISSION_STATE,
static void f_E_TRANSMISSION_STATE_handler(void){

  // TODO:
  // What we need to do:
  // * create the message
  // * check on time to transmit
  // * and on_time --> transmit!
#if 0  
  if(detector_state.gd_substate == E_GPS_CHECK_ON_ACTIVATION)
  {
    // Activation or no hay position
    // DB_PRINT
  }
  else
  {
    // All other messages
  }
#endif

  handlers_generic_set_handler_FLG(e_prepare_msg_h);

}


#endif


// OVERWORKING **********************************************************************
// OVERWORKING **********************************************************************
// OVERWORKING **********************************************************************
// OVERWORKING **********************************************************************
// OVERWORKING **********************************************************************





// EOF