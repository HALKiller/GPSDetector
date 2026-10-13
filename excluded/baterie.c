// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //
#include <stdint.h>

#include "ADC.h"
#include "Global.h"
#include "UART.h"


#if 1

//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

union udt_generic{
	
	uint8_t reg;
	
	struct
  {
		unsigned b0: 1;
		unsigned b1: 1;
		unsigned b2: 1;
		unsigned b3: 1;
		unsigned b4: 1;
		unsigned b5: 1;
		unsigned b6: 1;
		unsigned b7: 1;

	};
	struct
	{
		unsigned rgb:		3;
		unsigned rest:	5;
	};
};

union udt_generic Bat_led_FLGS;



#define RED_IS_ON			Bat_led_FLGS.b0
#define GREEN_IS_ON		Bat_led_FLGS.b1
#define BLUE_IS_ON		Bat_led_FLGS.b2
#define BASE_IS_GREEN Bat_led_FLGS.b3 

#define BAT_COLOR			Bat_led_FLGS.rgb

//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 
const uint16_t const_for_some_value = 123;


//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 
#define c_ADC_OVERSAMPLING 8
#define cADC_BATERIA_MIMIMUM_THRESHHOLD 127	// these are ADC
#define BATERIE_MINIMUM_mv_LEVEL	6000
#define BATERIE_MAXIMUM_mv_LEVEL	9000
#define BATERIE_MEDIUM_mv_LEVEL = ((BATERIE_MAXIMUM_mv_LEVEL - BATERIE_MAXIMUM_mv_LEVEL)/2) + BATERIE_MINIMUM_mv_LEVEL

#define LEDBAT_PWM_BASE 64


//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //


uint16_t batled_setting = 64;	// we stat inicially full on...

//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

static void calculate_batled(uint16_t bat_value);


//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //


void init_batled(void){
	
	Bat_led_FLGS.reg = 0;
	
}










// implementing the measurement of the internal vref for the calculation of the vbat value//
static void bat_handler(void){
	
	uint8_t sample_looper = 0;
	uint16_t sum = 0;
	uint8_t temp_val = 0;
	
	uint8_t vref_adc = 0;
	const uint32_t V_ref_uV= 4096000;
	const uint16_t const_diode_drop_mV = 700;
	uint16_t uV_per_adc = 0;
	uint32_t Vbat_mV = 0;
	uint32_t adc_vbat = 0;
	
	vref_adc = adc_samples_channel(VREF_ADC_CHANNEL);
	
	uV_per_adc = V_ref_uV / vref_adc;
	
UWT("ADC_Vref: ");
UART_int(vref_adc);
UART_CRLF;	



	for(sample_looper = 0; sample_looper < c_ADC_OVERSAMPLING; sample_looper++)
	{
		temp_val = adc_samples_channel(BATERIA_ADC_CHANNEL);
		sum = sum + temp_val;
		
	}
	
	adc_vbat = sum / c_ADC_OVERSAMPLING;
	
	Vbat_mV = adc_vbat * uV_per_adc / 500;	// divide by 500 becaue R_Divider = 2:1 and therefore only by 400
	Vbat_mV = Vbat_mV + const_diode_drop_mV;
	
	
#if 1
UWT("ADC ");
UART_int(sum / c_ADC_OVERSAMPLING);
UART_CRLF;

UWT("Bat: ");
UART_int(Vbat_mV);
#endif



	// TODO --> i still need here the R_divider_values and the actuall thresshold voltages etc...
	if(Vbat_mV < BATERIE_MINIMUM_mv_LEVEL)
		// if((sum / c_ADC_OVERSAMPLING) < cADC_BATERIA_MIMIMUM_THRESHHOLD)
	{
		
		// handler flag for low batery
		UWT("\r\nBAT is BAD!\r\n");
		leds_update_shadow_led(e_LED_ON_OFF, e_LC_RED);
	}
	else
	{
		UWT("\r\nBAT is GOOD!\r\n");
		leds_update_shadow_led(e_LED_ON_OFF, e_LC_GREEN);
	}
	

}




//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //



uint8_t baterie_set_batled_setting(uint8_t of_cnt){
	
	
	if(of_cnt >= batled_setting)
	{
		// swoff
		if(BASE_IS_GREEN == true)
		{
			RED_IS_ON = false;
		}
		else
		{
			GREEN_IS_ON = false;
		}
		
	}
	else
	{
		RED_IS_ON = true;
		GREEN_IS_ON = true;
	}
	
	leds_update_shadow_led(e_LED_ON_OFF, BAT_COLOR);
	
	
}

static void calculate_batled(uint16_t bat_value){

	TMR4_IE = false;
	// < 6000mV
	if(BATERIE_MAXIMUM_mv_LEVEL > bat_value)
	{
		batled_setting = 0;
		BASE_IS_GREEN = true;
	}	// < 7500 
	else if(BATERIE_MEDIUM_mv_LEVEL > bat_value)
	{
		batled_setting = LEDBAT_PWM_BASE - ((BATERIE_MEDIUM_mv_LEVEL - bat_value) * LEDBAT_PWM_BASE / BATERIE_MEDIUM_mv_LEVEL);
		
		BASE_IS_GREEN = false;
	}	// < 9000 
	else if(BATERIE_MAXIMUM_mv_LEVEL > bat_value)
	{
		batled_setting = LEDBAT_PWM_BASE - (bat_value - BATERIE_MEDIUM_mv_LEVEL) * LEDBAT_PWM_BASE / BATERIE_MEDIUM_mv_LEVEL;
		BASE_IS_GREEN = true;
	}	// > 9000
	else
	{
		batled_setting = 0;
		BASE_IS_GREEN = true;
	}

	TMR4_IE = true;
	
// UWT("Batled: ");
// UART_int(batled_setting);

}



#endif




// EOF