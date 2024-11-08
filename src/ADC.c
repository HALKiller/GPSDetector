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

#define USE_OLD_ADC_IMPLEMENTATION 1

#define ADC_CHANNEL ADCON0bits.CHS

#define ADC_SAMPLES	32

#define START_CONVERSION	ADCON0bits.GO




#if USE_OLD_ADC_IMPLEMENTATION


// this version should be fine now for both chips...
void ConversionAdc(bool JustificacionOrdenBits, uint8_t canal)
{
	

  ADCON1bits.ADCS = 0b001;  // Fosc/8 --> because Errata in this Chip!    0b11; // Reloj RC
	 // Se selecciona la referencia de voltaje
  ADCON1bits.ADPREF = 0; // Se selecciona AVDD = VDD
  ADCON1bits.ADNREF = 0; // Se selecciona AVSS = VSS
	



  // Se selecciona el canal a convertir
  ADCON0bits.CHS = canal;
  // Se selecciona el formato del resultado
  // 0 - Justificación a la izquierda, 1 - Justificación a la derecha
  // Ejemplo de valor de adc de 10 bits obtenido : 0b1100111001
  /* ADFM = 0 para 0b1100111001
   *   ADRESH   |  ADRESL
   * 0b11001110 | 0b01xxxxxx (las x serán 0)
   */
  /* ADFM = 1 para 0b1100111001
   *   ADRESH   |  ADRESL
   * 0bxxxxxx11 | 0b00111001 (las x serán 0)
   */
  ADCON1bits.ADFM = JustificacionOrdenBits;
  // Se enciende el módulo ADC
  ADCON0bits.ADON = 1;
	
	__delay_us( 250 ); // Tiempo de adquisición sobreestimado

	#if 1
	if(canal == BATERIA_ADC_CHANNEL)
	{
		__delay_us( 750 );
	}
	#endif

  ADCON0bits.GO_nDONE = 1;
  //ADCON0bits.GO_DONE = 1;
  // Apaga todas las interrupciones y espera a salir por la conversión ADC
#if 0

#if REDUCE_ROM_USAGE
	GIE = false;
#else	
  RCIE   = false;
  TMR1IE = false;
  TMR2IE = false;
  T0IE   = false;
#endif	
#endif

//  ACTIVAR_WATCHDOG_TIMER();
#if 1

	NOP();
	while(ADCON0bits.GO_nDONE == true)
  {
    // loop until flag is set
  }

#else

  do
  {
    NOP();
  } while ( ADCON0bits.GO_nDONE );
	
	
#endif	
	
#if 0
#if REDUCE_ROM_USAGE
	GIE = true;
#else		
	
  // Tras la conversión, que vuelva a encender el resto de interrupciones
  RCIE   = true;
  TMR1IE = true;
  TMR1IE = true;
  T0IE   = true;

#endif

#endif

 
  ADCON0bits.ADON = 0;

	

}



#else

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

#endif

