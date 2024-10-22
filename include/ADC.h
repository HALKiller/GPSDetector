#ifndef ADC_H
#define ADC_H

#include <stdint.h>



void init_ADC(void);

uint8_t adc_samples_channel(uint8_t channel_to_sample);

// uint16_t get_mv_from_adc_result_buffer_slot(uint8_t buffer_slot);

// void set_mv_from_adc_result_into_result_buffer(uint16_t converted_adc_result, uint8_t buffer_slot);



// uint16_t get_adc_from_adc_result_buffer_slot(uint8_t buffer_slot);

// void set_adc_from_adc_result_into_result_buffer(uint16_t adc_result, uint8_t buffer_slot);


































#endif	// ADC_H