#ifndef ADC_H
#define ADC_H

#include "Global.h"	// for the adc alternativ int pin channel selection



#define LEFT_JUSTIFIED 0
#define RIGHT_JUSTIFIED 1





void ConversionAdc(bool JustificacionOrdenBits, uint8_t canal);




// void init_ADC(void);

// uint8_t adc_samples_channel(uint8_t channel_to_sample);


























#endif	// ADC_H