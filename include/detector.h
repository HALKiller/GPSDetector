#ifndef DETECTOR_H
#define DETECTOR_H



#include "Global.h"

#include "gd_states.h"

#define USE_PWM_LUZ_FILE 0

#define PWM_LUZ_DEBUG 0

#if PWM_LUZ_DEBUG


#define LED_SIMUL DB_LED_2
#define LED_SIMUL_ON DB_LED_2_ON
#define LED_SIMUL_OFF DB_LED_2_OFF

#else
  
#define LED_SIMUL_ON  
#define LED_SIMUL_OFF  

#endif



// TODO: that should get into a own cfg.h file i think...
// because there is now a design where the fototransistor is on low side switching...
 

// agaisnt this threshold we cpompare if it is dark or not...
// with the LDR that is set to 50
// with the transistor that is going to be..?? around 230...
// 31052024 --> taking measurtements and YEP --> exactly 230...
#define LUZ_ADC_DARK_THRESHOLD 230
// ... and if the sensor is now high side switching we set this value...
#define LUZ_ADC_DARK_THRESHOLD_INVERTED 70

typedef enum status_led_state_type{
  
  LED_RED_ON,
  LED_GREEN_ON,
  LED_RED_BLINKS,
  LED_GREEN_BLINKS,
  ALL_LED_OFF,
  NUM_LED_STATES,
  
  
}leds_state_t;




struct udt_detector{
  
  uint32_t next_time_tx;
  uint16_t time_between_tx;
  uint16_t seconds_until_next_tx;
  uint16_t rtc_alarm;
  uint16_t db_adc_value;
  
  uint8_t number;
  uint8_t max_detectores;
  uint8_t transmission_duration;
  uint8_t syncro_time;
  uint8_t no_position_cnt;
  uint8_t vbat_low;
  uint8_t vbat_high;
  uint8_t bat_decivolt;
  uint8_t led_time_out_cnt;
  leds_state_t led_state;
};

extern struct udt_detector gd;

void init_detector_config(void);
void increment_detector(void);
void pwm_luz_time_update(void);

uint8_t get_detector_number(void);

uint8_t get_sync_time(void);

uint8_t get_transmission_duration(void);

uint8_t get_max_detectores(void);

uint8_t get_pwm_luz_pwm_value(void);

uint8_t read_ilum_sensor(void);

void detector_init_ilumination_handling(void);

void measure_bat_for_batcnt(e_gpsd_states_t state);
  
uint16_t get_batcnt(void);
  
uint8_t LeerValorBateria(void);

void detector_status_led_handler(void);

void detector_status_led_cnt_on(leds_state_t led_status);
// extern uint8_t baterie_mV;













































#endif // PWM_LUZ_H