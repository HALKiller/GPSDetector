// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// the functions for the init of the adc and the actual sampling of a channel
// declared here the array which stores the result of the adc channels --> exception for
// the Vref channel --> that holds an independent file



#include "ADC.h"
#include "Global.h"
#include "io_port_sfr_names.h"
// #include "xc.h"
// #include <stdint.h>

#include "UART.h"



#define ADC_CHANNEL ADCON0bits.CHS

#define ADC_SAMPLES	32

#define START_CONVERSION	ADCON0bits.GO




void init_ADC(void){
	
	ADC_ON = FALSE;
#if MIPS==1	
	ADCON1bits.ADCS = 5u;	// FOSC/16 --> 4MHz Fosc Internal	1TAD = 4us(datasheet)
#elif MIPS==8
	ADCON1bits.ADCS = 6u;	// FOSC/64 --> 32MHz Fosc Internal	1TAD = 2us(datasheet)
#else
	MISSING
#endif	
	ADCON1bits.ADFM = FALSE;	// left justified in taht case...
	ADCON1bits.ADNREF = FALSE;	// Vss = -Vref
	ADCON1bits.ADPREF = FALSE;	// Vdd = +Vref
	
  
	FVRCONbits.ADFVR = 3u;	// 0 = off, 1 = 1024mV, 2 = 2048mV, 3 = 4096mV
	FVRCONbits.FVREN = TRUE;
	while(FVRCONbits.FVRRDY == FALSE)
  {
    // empty loop
  }
	
	ADC_ON = TRUE;
	
	
	
}


uint8_t adc_samples_channel(uint8_t channel_to_sample){

	uint8_t temp_val = 0;


	ADC_CHANNEL = channel_to_sample;
	
	__delay_us(20);	// sampling the Input
	
	START_CONVERSION = true;
	
	while(START_CONVERSION == true);	// waiting until the Conversion has finished...
	
	temp_val = ADRESH;
#if 0	
	UWT("A: ");
	UART_int(temp_val);
#endif	
	return temp_val;
	
	
}



