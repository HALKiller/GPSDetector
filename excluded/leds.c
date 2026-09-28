//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //

// for the different LED out puts we create this one here...


//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //

#include "leds.h"
#include "io_port_sfr_names.h"
#include "Global.h"
#include "my_assert.h"
#include "UART.h"
#include "device_driver_config.h"

#include <stdint.h>



//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

union udt_led {
	
	uint8_t reg;
	struct
	{
		unsigned shadow_led_color : 3;
		unsigned rest : 5;
	};
	struct
	{
		unsigned shadow_led_r : 1;
		unsigned shadow_led_g : 1;
		unsigned shadow_led_b : 1;
		unsigned free : 5;
	};
};

union udt_led led_rgb[2];

//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 



//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 



//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //


void leds_move_shadow_leds_to_real_leds(void);


//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //



//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

void init_leds(void){
	
	uint8_t hlooper = 0;
	
	for(hlooper = 0; hlooper < e_NUM_LEDS; hlooper++)
	{
		
		led_rgb[hlooper].reg = 0;

	}


#if 1
	for(hlooper = 0; hlooper < e_NUM_LED_COLORS; hlooper++)
	{
		leds_update_shadow_led(e_LED_STATE, hlooper);
		leds_update_shadow_led(e_LED_ON_OFF, hlooper);
		
#if DEBUGGING_IS_ON
    __delay_ms(75);
#else    
		__delay_ms(75);
#endif
		
		
		
	}
	
	
	leds_update_shadow_led(e_LED_ON_OFF, e_LC_GREEN);
	leds_update_shadow_led(e_LED_STATE, e_LED_OFF);
	
#endif


}


void leds_update_shadow_led(uint8_t led_channel, uint8_t color_set){
	
	
	assert(led_channel < e_NUM_LEDS);
  
	assert(color_set < e_NUM_LED_COLORS);
	
	led_rgb[led_channel].shadow_led_color = color_set;
	
	leds_move_shadow_leds_to_real_leds();
	
}



void leds_move_shadow_leds_to_real_leds(void){
	
	// assert(led_channel < e_NUM_LEDS);

#if 0
	
  
  
	STATE_LED_R = led_rgb[e_LED_STATE].shadow_led_r;
	STATE_LED_G = led_rgb[e_LED_STATE].shadow_led_g;
	STATE_LED_B = led_rgb[e_LED_STATE].shadow_led_b;


	ONOFF_LED_R = led_rgb[e_LED_ON_OFF].shadow_led_r;
	ONOFF_LED_G = led_rgb[e_LED_ON_OFF].shadow_led_g;
	ONOFF_LED_B = led_rgb[e_LED_ON_OFF].shadow_led_b;

  

#else

  

	IO_Write_channel(IO_STATE_LED_R, led_rgb[e_LED_STATE].shadow_led_r);
	IO_Write_channel(IO_STATE_LED_G, led_rgb[e_LED_STATE].shadow_led_g);
	IO_Write_channel(IO_STATE_LED_B, led_rgb[e_LED_STATE].shadow_led_b);
	
	IO_Write_channel(IO_ONOFF_LED_R, led_rgb[e_LED_ON_OFF].shadow_led_r);
	IO_Write_channel(IO_ONOFF_LED_G, led_rgb[e_LED_ON_OFF].shadow_led_g);
	IO_Write_channel(IO_ONOFF_LED_B, led_rgb[e_LED_ON_OFF].shadow_led_b);

  

#endif



}




//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //



//   * * * * * *      I S R  - -  H A N D E L E R     * * * * * * * * * * * * * *   //



// EOF