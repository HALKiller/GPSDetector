// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// the functions for the init of the adc and the actual sampling of a channel
// declared here the array which stores the result of the adc channels --> exception for
// the Vref channel --> that holds an independent file



#include "ADC.h"

#include "Global.h"

#include "io_port_sfr_names.h"

#include "UART.h"


// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  
#ifndef FILE_ADC_DB_ENABLED
#define FILE_ADC_DB_ENABLED 0
#endif
#if FILE_ADC_DB_ENABLED
#define DB_PRINT(str) G_DB_PRINT(str)
#define UART_int(var) G_UART_INT(var)
#else
#define DB_PRINT(str)
#define UART_int(var)
#endif
// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  


#define USE_OLD_ADC_IMPLEMENTATION 1

#define ADC_CHANNEL ADCON0bits.CHS

#define ADC_SAMPLES	18

#define START_CONVERSION	ADCON0bits.GO




#if USE_OLD_ADC_IMPLEMENTATION


// lets try oversampling...
#if USE_ADC_OVERSAMPLING

static uint16_t ADC_Vref = 0;
#if DEBUG_ADC_RESULTS
uint16_t adc_res[8];
#endif

#if 1

// OV 22042026: highest lowest reject, oversampling rate = 18; reduce Ton time
// was: 250u, now 25u , adquisition time = 10u
// this version should be fine now for both chips...
uint16_t ConversionAdc(bool JustificacionOrdenBits, uint8_t canal){
	
  uint8_t hlooper = 0;
  uint8_t samples_setter = 1;
  uint16_t adc_sum = 0; 
  uint16_t ret_value = 0;
  uint16_t biggest = 0;
  uint16_t smallest = 0xFFFF;
  uint16_t temp_res = 0;
  uint8_t of_FLG = false;
#if 0
  ADCON1bits.ADCS = 0b001;  // Fosc/8 --> because Errata in this Chip!    0b11; // Reloj RC
	 // Se selecciona la referencia de voltaje
  ADCON1bits.ADPREF = 0; // Se selecciona AVDD = VDD
  ADCON1bits.ADNREF = 0; // Se selecciona AVSS = VSS
#else
  ADCON1 = 0x10;
#endif

  ADCON1bits.ADFM = JustificacionOrdenBits;
    // Se selecciona el canal a convertir
  ADCON0bits.CHS = canal;
  // ADC On
  ADCON0bits.ADON = 1;
	
	__delay_us( 25 );


	if((canal == BATERIA_ADC_CHANNEL)||(canal == VREF_ADC_CHANNEL))
	{
		__delay_us( 25 );
    samples_setter = ADC_SAMPLES;
    of_FLG = true;
	}


  for(hlooper = 0; hlooper < samples_setter; hlooper++)
  {
    
    __delay_us( 10 );
    ADCON0bits.GO_nDONE = 1;

    NOP();
    while(ADCON0bits.GO_nDONE == true)
    {
      // loop until flag is set
    }
#if DEBUG_ADC_RESULTS    
    adc_res[hlooper] = 256 * ADRESH + ADRESL;
#endif    
    
    temp_res = (256 * ADRESH + ADRESL);
    
    if(of_FLG == true)
    {
      if(temp_res > biggest)
      {
        biggest = temp_res;
      }
      
      if(temp_res < smallest)
      {
        smallest = temp_res;
      }
    }
    
    adc_sum = adc_sum + temp_res;
   
  }
  
  ADCON0bits.ADON = 0;
  
  if(of_FLG == true)
  {
    ret_value = (adc_sum - smallest - biggest)/(samples_setter - 2);
  }
  else
  {
    ret_value = adc_sum / samples_setter;
  }
  

  return ret_value;

}

#elif 1

// this version should be fine now for both chips...
uint16_t ConversionAdc(bool JustificacionOrdenBits, uint8_t canal){
	
  uint8_t hlooper = 0;
  uint8_t samples_setter = 1;
  uint16_t adc_sum = 0; 
  uint16_t ret_value = 0;
  
  
  ADCON1bits.ADCS = 0b001;  // Fosc/8 --> because Errata in this Chip!    0b11; // Reloj RC
	 // Se selecciona la referencia de voltaje
  ADCON1bits.ADPREF = 0; // Se selecciona AVDD = VDD
  ADCON1bits.ADNREF = 0; // Se selecciona AVSS = VSS
	
  // Se selecciona el canal a convertir
  ADCON0bits.CHS = canal;

  ADCON1bits.ADFM = JustificacionOrdenBits;
  
  // ADC On
  ADCON0bits.ADON = 1;
	
	__delay_us( 250 );


	if((canal == BATERIA_ADC_CHANNEL)||(canal == VREF_ADC_CHANNEL))
	{
		__delay_us( 750 );
    samples_setter = ADC_SAMPLES;
	}


  for(hlooper = 0; hlooper < samples_setter; hlooper++)
  {
    
    ADCON0bits.GO_nDONE = 1;

    NOP();
    while(ADCON0bits.GO_nDONE == true)
    {
      // loop until flag is set
    }
#if DEBUG_ADC_RESULTS    
    adc_res[hlooper] = 256 * ADRESH + ADRESL;
#endif    
    adc_sum = adc_sum + (256 * ADRESH + ADRESL);
   
  }
  
  ADCON0bits.ADON = 0;
  
  ret_value = adc_sum / samples_setter;

  return ret_value;

}

#else
  
// this is just a temporary testing not for releases!
uint16_t ConversionAdc(bool JustificacionOrdenBits, uint8_t canal)
{
	
  uint8_t hlooper = 0;
  uint8_t samples_setter = 1;
  uint16_t adc_sum = 0; 
  uint16_t ret_value = 0;
  uint16_t s_looper = 500u;
  uint16_t min = 65535;
  uint16_t max = 0;
  uint8_t show = false;
  
  ADCON1bits.ADCS = 0b001;  // Fosc/8 --> because Errata in this Chip!    0b11; // Reloj RC
	 // Se selecciona la referencia de voltaje
  ADCON1bits.ADPREF = 0; // Se selecciona AVDD = VDD
  ADCON1bits.ADNREF = 0; // Se selecciona AVSS = VSS
	
  // Se selecciona el canal a convertir
  ADCON0bits.CHS = canal;

  ADCON1bits.ADFM = JustificacionOrdenBits;
  
  // ADC On
  ADCON0bits.ADON = 1;
	
	__delay_us( 250 );


	if((canal == BATERIA_ADC_CHANNEL)||(canal == VREF_ADC_CHANNEL))
	{
		// __delay_us( 750 );
    samples_setter = ADC_SAMPLES;
	}


while(s_looper > 0)
{
  
  show = false;
  adc_sum = 0;
  for(hlooper = 0; hlooper < samples_setter; hlooper++)
  {
    
    ADCON0bits.GO_nDONE = 1;

    NOP();
    while(ADCON0bits.GO_nDONE == true)
    {
      // loop until flag is set
    }
#if DEBUG_ADC_RESULTS    
    adc_res[hlooper] = 256 * ADRESH + ADRESL;
#endif    
    adc_sum = adc_sum + (256 * ADRESH + ADRESL);
   
  }
  
  ret_value = adc_sum / samples_setter;
  
  
  if(ret_value > max)
  {
    max = ret_value;
    show = true;
  }
  if(ret_value < min)
  {
    min = ret_value;
    show = true;
  }
  if(show == true)
  {
    if(canal==VREF_ADC_CHANNEL)
    {
        DB_PRINT("\r\nVref: ");
    }
    else if(canal == BATERIA_ADC_CHANNEL)
    {
      DB_PRINT("\r\nBAT: ");
    }
    
    UART_int(ret_value);
  }
  
  s_looper--;
  
  
} 
  ADCON0bits.ADON = 0;
  
  ret_value = adc_sum / samples_setter;

  return ret_value;

}



#endif
uint16_t calculate_mV_from_ADC(uint16_t ADC_value){
const uint16_t const_Vref_value = 8204; // 2048;
uint32_t temp_ADC_value = ADC_value;
uint16_t ret_value = 0;

	if(ADC_Vref != 0)
	{
		ret_value = (temp_ADC_value * const_Vref_value) /  ADC_Vref;
	}

	return ret_value;
	
}

// Setting the value means of course to take an adc read of the Vref 
// i will use the 2048mV Vref and calculate against that than...
void adc_set_vref_adc_value(void){
  
  // switch on the Vref capabilitys and configure it,wat for staiblization anddd
  // ADC_Vref
  FVRCON = 0x82;  // 10000010
  while(FVRCONbits.FVRRDY == false)
  {
    // wait to stabilize...
  }
  
  ADC_Vref = ConversionAdc(RIGHT_JUSTIFIED, VREF_ADC_CHANNEL);
  
  FVRCONbits.FVREN = false;
  
  
}

// void adc_getvref_adc_value(void){
  
  // return ADC_Vref;
  
// }

#else
  
// this version should be fine now for both chips...
void ConversionAdc(bool JustificacionOrdenBits, uint8_t canal)
{
	

  ADCON1bits.ADCS = 0b001;  // Fosc/8 --> because Errata in this Chip!    0b11; // Reloj RC
	 // Se selecciona la referencia de voltaje
  ADCON1bits.ADPREF = 0; // Se selecciona AVDD = VDD
  ADCON1bits.ADNREF = 0; // Se selecciona AVSS = VSS
	
  // Se selecciona el canal a convertir
  ADCON0bits.CHS = canal;

  ADCON1bits.ADFM = JustificacionOrdenBits;
  
  // ADC On
  ADCON0bits.ADON = 1;
	
	__delay_us( 250 );

	#if 1
	if(canal == BATERIA_ADC_CHANNEL)
	{
		__delay_us( 750 );
	}
	#endif

  ADCON0bits.GO_nDONE = 1;
  //ADCON0bits.GO_DONE = 1;
  // TODO: check on GIE = false
  // Apaga todas las interrupciones y espera a salir por la conversión ADC



	NOP();
	while(ADCON0bits.GO_nDONE == true)
  {
    // loop until flag is set
  }



  ADCON0bits.ADON = 0;

	

}

#endif



#else // USE_OLD_ADC_IMPLEMENTATION

static uint16_t ADC_Vref = 0;


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

	return temp_val;
	
	
}


// Setting the value means of course to take an adc read of the Vref 
// i will use the 2048mV Vref and calculate against that than...
void adc_set_vref_adc_value(void){
  
  // switch on the Vref capabilitys and configure it,wat for staiblization anddd
  // ADC_Vref
  FVRCON = 0x82;  // 10000010
  while(FVRCONbits.FVRRDY == false)
  {
    // wait to stabilize...
  }
  
  ADC_Vref = ConversionAdc(RIGHT_JUSTIFIED, VREF_ADC_CHANNEL,);
  
  FVRCONbits.FVREN = false;
  
  
}

void adc_getvref_adc_value(void){
  
  return ADC_Vref;
  
}



uint16_t calculate_mV_from_ADC(uint16_t ADC_value){
const uint16_t const_Vref_value = 8204; // 2048;
uint32_t temp_ADC_value = ADC_value;
uint16_t ret_value = 0;

	if(ADC_Vref != 0)
	{
		ret_value = (temp_ADC_value * const_Vref_value) /  ADC_Vref;
	}

	return ret_value;
	
}



void set_ADC_Vref(uint16_t adc_result){
	
	ADC_Vref = adc_result;
	
}











#endif

