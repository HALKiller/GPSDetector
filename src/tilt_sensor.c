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

#include "io_port_sfr_names.h"

#include "gd_states.h"

#if DEBUGGING_IS_ON
#include "UART.h"
#endif

//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

struct udt_tilt_sensor_type{
  
  tilt_sensor_states_t detector_is_on;
  uint8_t on_cnt;
  uint8_t off_cnt;
  
};

static struct udt_tilt_sensor_type tilt_sensor;


//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 

 


//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
#define SENSOR_READINGS_PER_SECOND ((uint16_t)5u)
#define CONST_ON_CNT_DEBOUNCED (uint8_t)(TIME_THRESHOLD_FOR_DETECTOR_IS_ON * SENSOR_READINGS_PER_SECOND)
#define CONST_OFF_CNT_DEBOUNCED (uint8_t)(TIME_THRESHOLD_FOR_DETECTOR_IS_OFF * SENSOR_READINGS_PER_SECOND)
 

//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //




//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

// static void update_tilt_sensor_state(uint8_t read_state);


//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //



void tilt_sensor_init(void){
  
  tilt_sensor.detector_is_on = TS_STARTUP_STATE;
  
  tilt_sensor.on_cnt = 0u;
  
  tilt_sensor.off_cnt = 0u;
 
}


#if 0

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

  update_tilt_sensor_state(tilt_state);


  
}

#endif


//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //


#if 1

// a count algorithm in function of the last read state --> therefroe we are changoing the state only on 
// count > than threshold. and there can be two different thresholds for up and downcount.
void update_tilt_sensor_state(void){


  uint8_t read_state = TILT_SENSOR;

	if(read_state == SENSOR_IS_TOP_MOUNTED)
	{
   
   
    if(tilt_sensor.detector_is_on != TS_ON_STATE)
    {
      tilt_sensor.on_cnt++;
      tilt_sensor.off_cnt = (uint8_t)0u;
      
      if(tilt_sensor.on_cnt > CONST_ON_CNT_DEBOUNCED)
      {
        tilt_sensor.detector_is_on = TS_ON_STATE;
        gd_states_switch_to_next_state(E_GPS_CHECK_ON_ACTIVATION);
      }
      
    }

	}
	else
	{
    
    if(tilt_sensor.detector_is_on != TS_OFF_STATE)
    {
      tilt_sensor.off_cnt++;
      tilt_sensor.on_cnt = (uint8_t)0u;
      
      if(tilt_sensor.off_cnt > CONST_OFF_CNT_DEBOUNCED)
      {
        tilt_sensor.detector_is_on = TS_OFF_STATE;
        gd_states_switch_to_next_state(E_OFF_STATE);
      }
      
    }
    

	}
	
	
}




#else
  
// a count algorithm in function of the last read state --> therefroe we are changoing the state only on 
// count > than threshold. and there can be two different thresholds for up and downcount.
static void update_tilt_sensor_state(uint8_t read_state){


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
        
        gd_states_switch_to_next_state(E_GPS_CHECK_ON_ACTIVATION);
        
        // UWT("T_S_ON\r\n");
        
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
        
        gd_states_switch_to_next_state(E_OFF_STATE);
        
        // UWT("T_S_OFF\r\n");
        
      }
		}	
	}
	
	
}


#endif


// EOF