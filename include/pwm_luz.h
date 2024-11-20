#ifndef PWM_LUZ_H
#define PWM_LUZ_H



#include "Global.h"

#define USE_PWM_LUZ_FILE 0

#define PWM_LUZ_DEBUG 0

#if PWM_LUZ_DEBUG

#define LED_SIMUL_ON DB_LED_2_ON
#define LED_SIMUL_OFF DB_LED_2_OFF

#else
  
#define LED_SIMUL_ON  
#define LED_SIMUL_OFF  

#endif



// TODO: that should get into a own cfg.h file i think...
// because there is now a design where the fototransistor is on low side switching...
#define INVERTED_LDR_SENSOR 1  

// agaisnt this threshold we cpompare if it is dark or not...
// with the LDR that is set to 50
// with the transistor that is going to be..?? around 230...
// 31052024 --> taking measurtements and YEP --> exactly 230...
#define LUZ_ADC_DARK_THRESHOLD 230
// ... and if the sensor is now high side switching we set this value...
#define LUZ_ADC_DARK_THRESHOLD_INVERTED 70


struct udt_detector{
  
  uint8_t number;
  uint8_t max_detectores;
  uint8_t transmission_duration;
  uint8_t syncro_time;
  uint16_t time_between_tx;
  uint16_t seconds_until_next_tx;
  
};

extern struct udt_detector gd;

void init_detector_config(void);

void pwm_luz_time_update(void);

uint8_t get_detector_number(void);

uint8_t get_sync_time(void);

uint8_t get_transmission_duration(void);

uint8_t get_max_detectores(void);

uint8_t get_pwm_luz_pwm_value(void);

uint8_t read_ilum_sensor(void);

void LeerValorBateria(bool AntesDeTransmitir);

extern uint8_t baterie_mV;













































#endif // PWM_LUZ_H