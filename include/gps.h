#ifndef GPS_H
#define GPS_H

#include "Global.h"

#include <stdint.h>

#include "gps_extensions.h"


enum gps_data_slots{
	e_TIME,
	e_VALIDEZ,
	e_CURRENT_LATITUDE,
	e_NORTH_SOUTH,
	e_CURRENT_LONGITUDE,
	e_CL_EAST_WEST,
	e_SPEED_IN_KNOTS,
	e_REAL_HEADING,
	e_DATE,
	e_VARIATION,
	e_VAR_EAST_WEST,
};


typedef enum gps_state_type {

  GPS_SENTENCE_RECEIVING,
  NOT_RECEIVING_CORRECTLY,
  NOT_RECEIVING,
  GPS_NUM_STATES,
  
  // NO_BAUDSETTING_WORKS, // Unahnaled error so far!!
  // STATE_OFF,
  // GPS_ALL_GOOD,
  
}gps_state_t;



typedef enum fromto_cpy_type{
  
  SAVEPOSITION,
  RECOVERPOSITION,
  
}fromto_t;


void gps_init(void);

void gps_first_run(void);

uint32_t gps_rtc_get_second_cnt(void);

uint16_t gps_get_average_lock_time(void);

void stop_gps_lock_time_cnt(void);

void set_max_lock_time(void);

void gps_startup_initializer(void);

void gps_reinit(void);

void gps_stop(void);

void gps_calculate_lock_time(void);

gps_state_t gps_check_gps_error_status(void);

void values_to_gps_rx_buffer(uint8_t n_char);

void load_GPS_struct();

unsigned char *gps_get_pnt_to_gps_data_member(uint8_t member_id);

void toggle_uart_readout_gps_sentence(void);

RMC_sentence_t *get_pointer_to_rmc(void);

uint8_t gps_rmc_time_is_good_test(void);

void gps_set_debbugging_sync_time_flg(void);

uint8_t gps_buffer_get_len(void);

void copy_position_from_to(fromto_t fromto);





























#endif // GPS_H

// EOF