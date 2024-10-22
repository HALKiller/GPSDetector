// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //
// this is the api for the handling of the tilt sensor ->
// the sensor needs thes input output  and periferics to make it work:
// Comparator
// * Vref comparador
// * Vsignal tilt_sensor 
// * activate tilt_sensor

// timing characteristics:
// we need to have a few different timing setups:
// how often do we test the sensor? 
// for how long do we need the sensor to be active to get a stable reading?



//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //

#include <stdint.h>

#include "Global.h"

#include "tilt_sensor.h"

#include "device_driver_config.h"
// #include "my_debugger.h"

// #include "gd_config.h"

#include "handlers.h"

//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

struct udt_tilt_sensor_type{
  
  tilt_sensor_states_t detector_is_on;
  tilt_sensor_states_t last_state;
  uint8_t on_cnt;
  uint8_t off_cnt;
  
};

static struct udt_tilt_sensor_type tilt_sensor;


//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 
 #if 0
 // TODO: that should get a setting so that it depends on the TMR4 OF time
const uint8_t sensor_readings_per_second = 5;
const uint8_t const_on_cnt_debounced = TIME_THRESHOLD_FOR_DETECTOR_IS_ON * sensor_readings_per_second;
const uint8_t const_off_cnt_debounced = TIME_THRESHOLD_FOR_DETECTOR_IS_OFF * sensor_readings_per_second;
 
 #endif
 


//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
#define SENSOR_READINGS_PER_SECOND ((uint16_t)5u)
#define CONST_ON_CNT_DEBOUNCED (uint8_t)(TIME_THRESHOLD_FOR_DETECTOR_IS_ON * SENSOR_READINGS_PER_SECOND)
#define CONST_OFF_CNT_DEBOUNCED (uint8_t)(TIME_THRESHOLD_FOR_DETECTOR_IS_OFF * SENSOR_READINGS_PER_SECOND)
 
 
 
 
 #define ON_TIME_TILT_SENSOR 65  // 75 changed to 100 because of stabilization for ACMPL module

//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //




//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

// static void deinit_acmplp(void);

// static fsp_err_t init_acmplp(void);

static void update_detector_position_state_handler(uint8_t read_state);

//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //



void tilt_sensor_init(void){
  
  
  tilt_sensor.last_state = TS_STARTUP_STATE;
  
  tilt_sensor.detector_is_on = TS_STARTUP_STATE;
 
 
}

tilt_sensor_states_t tilt_sensor_get_detector_state(void){
  
  
  return tilt_sensor.detector_is_on;
  
}



// this only gets the state of the PORT PIN, that is not the debounced routine...
void tilt_sensor_get_state(void){
  

  
#if USE_DEVICE_DRIVER
  
  uint8_t tilt_state = IO_Read_channel(IO_TILT_SENSOR);
  
#else
  
  uint8_t tilt_state = TILT_SENSOR;

#endif

  update_detector_position_state_handler(tilt_state);


  
}




//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //



// a count algorithm in function of the last read state --> therefroe we are changoing the state only on 
// count > than threshold. and there can be two different thresholds for up and downcount.
static void update_detector_position_state_handler(uint8_t read_state){


	if(read_state == SENSOR_IS_TOP_MOUNTED)
	{
   
		tilt_sensor.on_cnt++;
    // tilt_sensor.on_cnt = tilt_sensor.on_cnt + 1u;
		
		tilt_sensor.off_cnt = (uint8_t)0u;
		
		if(tilt_sensor.on_cnt > CONST_ON_CNT_DEBOUNCED)
		{
      
      tilt_sensor.detector_is_on = TS_ON_STATE;
			
			tilt_sensor.on_cnt = CONST_ON_CNT_DEBOUNCED;
      
      if(tilt_sensor.detector_is_on != tilt_sensor.last_state)
      {
        
        tilt_sensor.last_state = tilt_sensor.detector_is_on;
        
        handlers_generic_set_handler_FLG(e_gd_on_h);
        
      }
      
		}

	}
	else
	{

		tilt_sensor.off_cnt++;
    
		tilt_sensor.on_cnt = (uint8_t)0u;
				
		if(tilt_sensor.off_cnt > CONST_OFF_CNT_DEBOUNCED)
		{
      
			tilt_sensor.detector_is_on = TS_OFF_STATE;
			
			tilt_sensor.off_cnt = CONST_OFF_CNT_DEBOUNCED;
			
      if(tilt_sensor.detector_is_on != tilt_sensor.last_state)
      {
        
        tilt_sensor.last_state = tilt_sensor.detector_is_on;
        
        handlers_generic_set_handler_FLG(e_gd_off_h);
        
      }
		}	
	}
	
	
}





// EOF