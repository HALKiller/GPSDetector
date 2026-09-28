//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //


#include "btn_number_setter.h"
#include "digital_inputs.h"
#include "UART.h"
#include "checksumming.h"
#include "handlers.h"
#include "Segment_7d.h"
#include "Global.h"
#include "leds.h"
#include "io_port_sfr_names.h"
#include "Clock.h"


#include <stdint.h>



//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //


union udt_btn{
	
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
}BTN_FLGS;




#define WAIT_FOR_ALL_BTN_RELEASE  BTN_FLGS.b0
#define SWOFF_IS_ACTIVE        		BTN_FLGS.b1
#define SOME_BTN_GOT_PRESSED			BTN_FLGS.b2




 enum {
  e_BTN_INCREMENT,
  e_BTN_DECREMENT,
	e_BTN_GRABAR,
	e_NUM_BTN,		
 };  



//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 



//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 
#define LOCAL_DEBUGGING_IS_ON	0
 
#define BTN_INCREMENT	0
#define BTN_DECREMENT	1
#define BTN_GRABAR 2

#define MAX_DETECTOR_NUMBER 60

//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //

uint8_t maximum_detectors = MAX_DETECTOR_NUMBER;
uint8_t detector_number = 1;

// uint8_t WAIT_FOR_ALL_BTN_RELEASE = false;


//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

static void prepare_for_single_number_grabacion(void);



//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //


void init_detector_number(void){
	
	detector_number = 1;
	
}


void btn_nr_setter_set_swoff_flg(void){
	
	SWOFF_IS_ACTIVE = true;
	
}
	
 
void btn_nr_setter_reset_swoff_flg(void){
	
	SWOFF_IS_ACTIVE = false;
	
} 
  
  
uint8_t btn_nr_setter_get_swoff_state(void){
  
  return SWOFF_IS_ACTIVE;
  
}  





#if 1


	
uint8_t btn_number_setter_btn_states(void){
	
	uint8_t t_val = 0;
	uint8_t old_detector_number = detector_number;
	uint8_t ret_value = true;
	uint8_t btn_state[3];	//  = 0;
	
  static uint8_t rnd_cnt = 0;	
	
	btn_state[e_BTN_INCREMENT] = get_debounced_btn_state(e_BTN_INCREMENT);
	btn_state[e_BTN_DECREMENT] = get_debounced_btn_state(e_BTN_DECREMENT);
	btn_state[e_BTN_GRABAR] = get_debounced_btn_state(e_BTN_GRABAR);
	
	
	SOME_BTN_GOT_PRESSED = true;
	
	if((btn_state[e_BTN_INCREMENT] == false)
	&& (btn_state[e_BTN_DECREMENT] == false)
	&& (btn_state[e_BTN_GRABAR] == false))
	{
		SOME_BTN_GOT_PRESSED = false;
	}
	
	if(SWOFF_IS_ACTIVE == true)
	{
		if(SOME_BTN_GOT_PRESSED == true)
		{
      
#if REDUCE_CONSUMPTION	
      init_clock();
#endif
        
			SWOFF_IS_ACTIVE = false;
      BCD_ACTIVE = true;
			segment_7d_write_main_value(detector_number);
			WAIT_FOR_ALL_BTN_RELEASE = true;
		}
    else
    {
      ret_value = false;
    }
	}
	else
	{	
		if(WAIT_FOR_ALL_BTN_RELEASE == true)
		{
			if(SOME_BTN_GOT_PRESSED == false)
			{
				WAIT_FOR_ALL_BTN_RELEASE = false;
			}

		}
		else
		{

			if((btn_state[e_BTN_INCREMENT] == 3) || (btn_state[e_BTN_INCREMENT] == 1))

			{
				// incrementer
				detector_number++;
				if(detector_number > maximum_detectors)
				{
					detector_number = 1;
				}
			}
			else if((btn_state[e_BTN_DECREMENT] == 3) || (btn_state[e_BTN_DECREMENT] == 1))
			{
				// decrement
				detector_number--;
				if(detector_number == 0)
				{
					detector_number = maximum_detectors;
				}
			}	
			else if(btn_state[e_BTN_GRABAR] != false)
			{
				// todo --> still might need to wait for the low  transition before starting it actually...
				prepare_for_single_number_grabacion();
				// to make sure we do not send twice allrady the grabacion etc...
				WAIT_FOR_ALL_BTN_RELEASE = true;
				
			}
			else
			{
				ret_value = false;
			}
			
		}
	}

	
#if 0

	segment_7d_write_main_value(detector_number);
  
#else
  
	if(old_detector_number != detector_number)
	{
		segment_7d_write_main_value(detector_number);
#if DEBUGGING_IS_ON    
		UWT("Det_Nr: ");
		UART_int(detector_number);
		UWT("\r\n");
#endif 
   
	}
#endif
		
	
	
#if 0

rnd_cnt++;
if(rnd_cnt == 10)
{

	UWT("Det_Nr: ");
	UART_int(detector_number);
	UWT("\r\n");
	rnd_cnt = 0;
}

#endif			
	
	return ret_value;
	


	
	
}





#elif 1
	
uint8_t btn_number_setter_btn_states(void){
	
	uint8_t t_val = 0;
	uint8_t old_detector_number = detector_number;
	uint8_t ret_value = true;
	uint8_t btn_state = 0;
	
static uint8_t rnd_cnt = 0;	
	
#if LOCAL_DEBUGGING_IS_ON || 0
	
	detector_number++;	//  = 35;
	
	if(detector_number > MAX_DETECTOR_NUMBER)
	{
		detector_number = 1;
		ret_value = true;
	}
	prepare_for_single_number_grabacion();
	
	return ret_value;
	
#else	
	
	if(WAIT_FOR_ALL_BTN_RELEASE == true)
	{
		
		if((get_debounced_btn_state(e_BTN_INCREMENT) == false)
		&& (get_debounced_btn_state(e_BTN_DECREMENT) == false)
		&& (get_debounced_btn_state(e_BTN_GRABAR) == false))
		{
			WAIT_FOR_ALL_BTN_RELEASE = false;
		}
	}
	else
	{
#if 1		
		btn_state = get_debounced_btn_state(e_BTN_INCREMENT);
		if((btn_state == 3) || (btn_state == 1))
#else			
		if(get_debounced_btn_state(e_BTN_INCREMENT) == (3 || 1))
#endif			
		{
			// incrementer
			detector_number++;
			if(detector_number > maximum_detectors)
			{
				detector_number = 1;
			}
		}
		else // if(get_debounced_btn_state(e_BTN_DECREMENT) == (3 || 1))
		{
			btn_state = get_debounced_btn_state(e_BTN_DECREMENT);
			if((btn_state == 3) || (btn_state == 1))
			{
				// decrement
				detector_number--;
				if(detector_number == 0)
				{
					detector_number = maximum_detectors;
				}
			}
			else if(get_debounced_btn_state(e_BTN_GRABAR) != false)
			{
				// todo --> still might need to wait for the low  transition before starting it actually...
				prepare_for_single_number_grabacion();
				// to make sure we do not send twice allrady the grabacion etc...
				WAIT_FOR_ALL_BTN_RELEASE = true;
				
			}
			else
			{
				ret_value = false;
			}
		}
		
		
#if 0

		segment_7d_write_main_value(detector_number);
#else		
		if(old_detector_number != detector_number)
		{
			segment_7d_write_main_value(detector_number);
			UWT("Det_Nr: ");
			UART_int(detector_number);
			UWT("\r\n");
		}
#endif		
	}
	
#if 1

rnd_cnt++;
if(rnd_cnt == 10)
{

	UWT("Det_Nr: ");
	UART_int(detector_number);
	UWT("\r\n");
	rnd_cnt = 0;
}

#endif			
	
	return ret_value;
	
#endif	

	
	
}




#else
	
	
uint8_t btn_number_setter_btn_states(void){
	
	uint8_t t_val = 0;
	uint8_t old_detector_number = detector_number;
	uint8_t ret_value = true;
	uint8_t btn_state = 0;
	
static uint8_t rnd_cnt = 0;	
	
#if LOCAL_DEBUGGING_IS_ON || 0
	
	detector_number++;	//  = 35;
	
	if(detector_number > MAX_DETECTOR_NUMBER)
	{
		detector_number = 1;
		ret_value = true;
	}
	prepare_for_single_number_grabacion();
	
	return ret_value;
	
#else	
	
	if(WAIT_FOR_ALL_BTN_RELEASE == true)
	{
		
		if((get_debounced_btn_state(e_BTN_INCREMENT) == false)
		&& (get_debounced_btn_state(e_BTN_DECREMENT) == false)
		&& (get_debounced_btn_state(e_BTN_GRABAR) == false))
		{
			WAIT_FOR_ALL_BTN_RELEASE = false;
		}
	}
	else
	{
#if 1		
		btn_state = get_debounced_btn_state(e_BTN_INCREMENT);
		if((btn_state == 3) || (btn_state == 1))
#else			
		if(get_debounced_btn_state(e_BTN_INCREMENT) == (3 || 1))
#endif			
		{
			// incrementer
			detector_number++;
			if(detector_number > maximum_detectors)
			{
				detector_number = 1;
			}
		}
		else if(get_debounced_btn_state(e_BTN_DECREMENT) == (3 || 1))
		{
			// decrement
			detector_number--;
			if(detector_number == 0)
			{
				detector_number = maximum_detectors;
			}
		}
		else if(get_debounced_btn_state(e_BTN_GRABAR) != false)
		{
			// todo --> still might need to wait for the low  transition before starting it actually...
			prepare_for_single_number_grabacion();
			// to make sure we do not send twice allrady the grabacion etc...
			WAIT_FOR_ALL_BTN_RELEASE = true;
			
		}
		else
		{
			ret_value = false;
		}
		
#if 0

		segment_7d_write_main_value(detector_number);
#else		
		if(old_detector_number != detector_number)
		{
			segment_7d_write_main_value(detector_number);
			UWT("Det_Nr: ");
			UART_int(detector_number);
			UWT("\r\n");
		}
#endif		
	}
	
#if 1

rnd_cnt++;
if(rnd_cnt == 10)
{

	UWT("Det_Nr: ");
	UART_int(detector_number);
	UWT("\r\n");
	rnd_cnt = 0;
}

#endif			
	
	return ret_value;
	
#endif	

	
	
}


#endif

#if 0

void btn_number_setter_set_new_maximum_detectors(uint8_t the_value){
	
	maximum_detectors = the_value;
	
	
}

#else
  
void btn_number_setter_set_new_maximum_detectors(uint8_t the_value){
	
	maximum_detectors = the_value;
  detector_number = maximum_detectors;
	segment_7d_write_main_value(detector_number);
	
}

  

#endif


uint8_t btn_number_setter_get_detector_number(void){
	
	
	return detector_number;
	
	
}



//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //

static void prepare_for_single_number_grabacion(void){
	
	uint8_t *data_pnt;
	uint8_t chcksum = 0;
	
	// so that the rx_buffer_is not getting written to while we 
	// are using it
	// for the transmission part	
	uart_swoff_reception();
	

	
	// get pointer to the buffer...
	data_pnt = get_pnt_to_uart_rx_buffer();
	
	// startbyte for single parameter is 'n'
	*data_pnt = 'n';
	data_pnt++;
	*data_pnt = 0x30;
	data_pnt++;
	*data_pnt = (detector_number / 10) + 0x30;
	data_pnt++;
	*data_pnt = (detector_number - ((detector_number / 10) * 10)) + 0x30;
	data_pnt++;
	*data_pnt = checksumming_chcksum_creator(get_pnt_to_uart_rx_buffer() + 1, 3);
	
	handlers_generic_set_handler_FLG(e_tx_to_gps);
	
}

//   * * * * * *      I S R  - -  H A N D E L E R     * * * * * * * * * * * * * *   //

#undef LOCAL_DEBUGGING_IS_ON

// EOF