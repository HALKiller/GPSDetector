#ifndef ADC_H
#define ADC_H

#include "Global.h"	// for the adc alternativ int pin channel selection



#define LEFT_JUSTIFIED 0
#define RIGHT_JUSTIFIED 1



#define DEBUG_ADC_RESULTS 0

#if USE_ADC_OVERSAMPLING
uint16_t ConversionAdc(bool JustificacionOrdenBits, uint8_t canal);
extern uint16_t adc_res[8];
#else
void ConversionAdc(bool JustificacionOrdenBits, uint8_t canal);
#endif

void adc_getvref_adc_value(void);
void adc_set_vref_adc_value(void);
uint16_t calculate_mV_from_ADC(uint16_t ADC_value);






























#endif	// ADC_H