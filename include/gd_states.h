#ifndef GPSDETECTOR_STATES_H
#define GPSDETECTOR_STATES_H

#include <stdint.h>

#define GD_STATES_OV 1

#if GD_STATES_OV


typedef enum gpsdet_type{
  
  E_RESET_STATE,
  E_LUZ_COM_STATE,
  E_STARTUP_STATE,
  E_GPS_CHECK_ON_ACTIVATION,
  E_TRANSMISSION_STATE,
  E_SEARCH_POSITION_STATE,
  E_SLEEP_BEFORE_TRANSMISSION_STATE,
  E_SLEEP_BEFORE_SEARCH_STATE,
  E_OFF_STATE,
  E_NUM_STATES,
  
}e_gpsd_states_t;


typedef enum gd_message_type{
  
  E_ACTIVATION_MSG,
  E_NO_HAY_GPS_RADIOGONIO_MSG,
  
}gd_message_t;


void gd_states_initialize(void);

void gd_states_switch_to_next_state(e_gpsd_states_t next_state);

e_gpsd_states_t gd_states_get_state(void);

e_gpsd_states_t gd_states_get_last_state(void);

void gd_states_set_next_state(e_gpsd_states_t next);

e_gpsd_states_t gd_states_get_next_state(void);


#else

typedef enum gpsdet_type{
  
  E_RESET_STATE,
  E_LUZ_COM_STATE,
  E_STARTUP_STATE,
  E_SEARCH_POSITION_STATE,
  E_OFF_STATE,
  E_TRANSMISSION_STATE,
  E_SLEEP_STATE,
  E_NUM_STATES,
  
}e_gpsd_states_t;


typedef enum gpsd_substate_type{
  
  E_GPS_CHECK_ON_ACTIVATION,
  E_GPS_SEARCHES_FOR_POSITION,
  E_TRANSMIT_MESSAGES,
  E_TILT_SENSOR_IS_ON,
  E_TILT_SENSOR_IS_OFF,
  E_CHECK_ON_TILT_SENSOR, // that gets set on initialicacion
  E_NUM_SUBSTATES,
}e_gpsd_substate_t;


typedef enum gd_message_type{
  
  E_ACTIVATION_MSG,
  E_NO_HAY_GPS_RADIOGONIO_MSG,
  
}gd_message_t;



void gd_states_initialize(void);

void gd_states_switch_to_next_state(void);

// void gd_states_set_gpsd_substate(e_gpsd_substate_t substate);

// e_gpsd_substate_t gd_states_get_substate(void);

void gd_state_change_handler(void);

uint8_t gd_states_did_change(void);




#endif






























#endif //  GPSDETECTOR_STATES_H