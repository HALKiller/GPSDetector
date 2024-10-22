#ifndef HANDLER_H
#define HANDLER_H

#include <stdint.h>


#define DB_LED_PWM 0

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
  e_sleep_handler,      						//	0
  e_switch_clock_handler,      	//	1
	e_ring_buffer_handler,				//	2 
	e_2ms_of_handler,							// 	3
	E_GD_ON_OFF_h,
	// e_adc_bateria_handler,
	E_TILT_SENSOR_h,
	e_200ms_h,
	// e_seg_7d_refresh_handler,
	e_reset_swoff_tmr_of_cnt_handler,
	// e_swoff_tmr_handler,	
	// e_swoff_consumption,
	e_errhandler,
	NUM_HANDLERS,									//	+1
 }HandlerType;                           
 
#endif


















#endif	// HANDLER_H