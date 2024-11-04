#ifndef PWM_LUZ_H
#define PWM_LUZ_H



#include "Global.h"

// because there is now a design where the fototransistor is on low side switching...
#define INVERTED_LDR_SENSOR 1  

// agaisnt this threshold we cpompare if it is dark or not...
// with the LDR that is set to 50
// with the transistor that is going to be..?? around 230...
// 31052024 --> taking measurtements and YEP --> exactly 230...
#define LUZ_ADC_DARK_THRESHOLD 230
// ... and if the sensor is now high side switching we set this value...
#define LUZ_ADC_DARK_THRESHOLD_INVERTED 70



void init_detector_config(void);

void pwm_luz_time_update(void);

uint8_t get_pwm_luz_pwm_value(void);

uint8_t read_ilum_sensor(void);


#if DEBUGGING_IS_ON
void swap_luz_on_off(void);
#endif













































#endif // PWM_LUZ_H