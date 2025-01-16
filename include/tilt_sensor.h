#ifndef TILT_SENSOR_H
#define TILT_SENSOR_H

#include "Global.h"




typedef enum tilt_sensor_states_type{
  
  TS_ON_STATE,
  TS_OFF_STATE,
  TS_STARTUP_STATE,
  
}tilt_sensor_states_t;


void tilt_sensor_init(void);

uint8_t update_tilt_sensor_state(void);












































#endif //TILT_SENSOR_H