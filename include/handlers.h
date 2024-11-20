#ifndef HANDLER_H
#define HANDLER_H

#include <stdint.h>


#define DB_LED_PWM 0
#define USE_DIRECT_CALL 1
void init_handler_flg(void);

void get_the_next_handler(void);

void handlers_generic_set_handler_FLG(uint8_t handler_set);



void reset_ring_buffer_handler_FLG(void);

void set_ring_buff_handler_flg(void);
	
	
#if DB_LED_PWM
void set_led_var(uint8_t set_val);	
#endif	
	
#if 1

typedef enum {
  
  e_rx_luz_com_h, // has to be highest because it always runs after reset, but only once
  e_always_transmit_handler,  // has to be second becasue if set we never exit from there
  e_gd_off_h,   // highest priority --> because when it swoffs, nothing else is important
  e_switch_clock_handler, // and clock swithcing has to be prioritized over other things...         
  e_1000ms_h,
  e_200ms_h, // and then the timing for the rtc to keep a good timing allright
  e_ring_buffer_handler,           
#if !USE_DIRECT_CALL    
  e_tilt_sensor_h,	               
#endif
  e_gps_on_h,  
  e_prepare_msg_h,
  e_startup_h, 
  e_gps_has_full_position_h,  
  e_ertc_handler_start, 
  e_errhandler,   
  e_gps_test_reception,
  NUM_HANDLERS,									//	+1
    
}HandlerType;                           
 
#endif



                 
 
 
 
 
 #endif	// HANDLER_H