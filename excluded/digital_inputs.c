// handles the digital inputs (buttons) and uses a basic 
// debounce algorithm to assure that the change of state has 
// stable ocurred for at least the set cnt in the related function(10)


#include "digital_inputs.h"
#include "io_port_sfr_names.h"
#include "handlers.h"
#include "Global.h"
#include "device_driver_config.h"
#include "my_assert.h"

#include "UART.h"


#include "xc.h"
#include "stdint.h"


static const uint8_t const_CANTIDAD_DIGITAL_BUTTONS = 3;

// therefore we need 1000ms of permanent pressed state to set the long press flag...
const uint16_t const_time_pressed_ms = 500;	
// const uint16_t const_swoff_time_pressed_ms = 2000;

struct udt_digital_io{
	union puls_FLG{
		uint8_t reg;
		
		struct {
			unsigned button_is_pressed	: 1;
			unsigned button_is_long_pressed	: 1;
			unsigned was_pressed : 1;
			unsigned button_number			:	3;
			unsigned free	: 2;
		};
		
	}FLGS;
	
	uint8_t is_pressed_cnt;
	uint16_t long_press_cnt;
	// button_is_pressed
	uint8_t is_released_cnt;
};

struct udt_digital_io button[const_CANTIDAD_DIGITAL_BUTTONS];


static void update_button_state_handler(struct udt_digital_io *btn_pnt);
static uint8_t get_button_state_from_port(uint8_t btn_number);





void load_digital_IO_struct(void){
	
	uint8_t hlooper = 0;
	
	for(hlooper = 0; hlooper < const_CANTIDAD_DIGITAL_BUTTONS; hlooper++)
	{		
		button[hlooper].is_pressed_cnt = 0;
		button[hlooper].long_press_cnt = 0;
		button[hlooper].is_released_cnt = 0;
		button[hlooper].FLGS.reg = 0;
		button[hlooper].FLGS.button_number = hlooper;
	}	
	
}


void update_button_handler(void){
	
	uint8_t hlooper = 0;

	for(hlooper = 0; hlooper < const_CANTIDAD_DIGITAL_BUTTONS; hlooper++)
	{		

		update_button_state_handler(&button[hlooper]);
		
	}
	
  
	
}

#if 1

uint8_t get_debounced_btn_state(uint8_t btn_nr){
	
	struct udt_digital_io *btn_pnt = &button[btn_nr];
	uint8_t ret_value = 0;

uint8_t hlooper = 0;


	if(btn_pnt->FLGS.button_is_long_pressed == true)
	{
		ret_value = 3;
	}
	else if((btn_pnt->FLGS.button_is_pressed == true) && (btn_pnt->FLGS.was_pressed == true))
	{
		ret_value = 2;
	}
	else if(btn_pnt->FLGS.button_is_pressed == true)
	{
		ret_value = 1;
		btn_pnt->FLGS.was_pressed = true;
		// UWT("First set btn\r\n");
	}
	else
	{
		ret_value = 0;
	}
	
#if 0
	if(ret_value == 1)
	{
		for(hlooper = 0; hlooper < 255; hlooper++)
		{
			UWT("RETVAL\r\n");
		}
	}
#endif	
	return ret_value;
	
	
}

#else
	

uint8_t get_debounced_btn_state(uint8_t btn_nr){
	
	return button[btn_nr].FLGS.button_is_pressed;
	
}


#endif


#if 0


void update_pulsador_state_handler(void){
	
	static uint8_t is_released_cnt = 0;
	static uint8_t is_pressed_cnt = 0;
	static uint16_t long_press_cnt = 0;
	const uint8_t const_rnd_cnt_debounced = 10;
	const uint16_t const_time_pressed_ms = TIEMPO_PULSADOR_PARA_ENTRAR_CALIBRACION * 200;


	if(PULSADOR_CALIB == true)
	{
		
		is_pressed_cnt++;
		long_press_cnt++;
		is_released_cnt = 0;
	}
	else
	{
		
		is_released_cnt++;
		is_pressed_cnt  = 0;
		long_press_cnt = 0;
	}
	
	if(is_pressed_cnt > const_rnd_cnt_debounced)
	{
		PULSADOR_FLGS.button_is_pressed = true;
		is_pressed_cnt = const_rnd_cnt_debounced;
		
	}
	
	if(is_released_cnt > const_rnd_cnt_debounced)
	{
		PULSADOR_FLGS.button_is_pressed = false;
		PULSADOR_FLGS.button_is_long_pressed = false;
		PULSADOR_FLGS.is_set_for_error_reset = true;
		is_released_cnt = const_rnd_cnt_debounced;
		
	}
	
	if(long_press_cnt > const_time_pressed_ms)
	{
		PULSADOR_FLGS.button_is_long_pressed = true;
		long_press_cnt = const_time_pressed_ms;
		
	}
	
	
	// reset_pulsador_handler_FLG();
	
}


#else
	

static void update_button_state_handler(struct udt_digital_io *btn_pnt){
	
#if 1	
static uint16_t rnd_cnt = 0;	
#endif	
uint8_t btn_nr = 0;

const uint8_t const_rnd_cnt_debounced = 10;

	btn_nr = btn_pnt->FLGS.button_number;
	
	if(get_button_state_from_port(btn_nr) == true)
	{
		
		btn_pnt->is_pressed_cnt++;
		btn_pnt->long_press_cnt++; 		
		btn_pnt->is_released_cnt = 0;
		
		if(btn_pnt->is_pressed_cnt > const_rnd_cnt_debounced)
		{
			btn_pnt->FLGS.button_is_pressed = true;
			btn_pnt->is_pressed_cnt = const_rnd_cnt_debounced;
		}
		
		if(btn_pnt->long_press_cnt > const_time_pressed_ms)
		{
			btn_pnt->FLGS.button_is_long_pressed = true;
			btn_pnt->long_press_cnt = const_time_pressed_ms;
			
		}
		
		
		
	}
	else
	{
		
		btn_pnt->is_released_cnt++;
		btn_pnt->is_pressed_cnt = 0;
		btn_pnt->long_press_cnt = 0;
		
		if(btn_pnt->is_released_cnt > const_rnd_cnt_debounced)
		{
			btn_pnt->FLGS.button_is_pressed = false;
			btn_pnt->FLGS.button_is_long_pressed = false;
			btn_pnt->FLGS.was_pressed = false;
			btn_pnt->is_released_cnt = const_rnd_cnt_debounced;
			
		}	
	}
	
#if 0

if(btn_nr == 0)
{
	rnd_cnt++;
}
if(rnd_cnt == 1000)
{
	

	
	UWT("Btn: ");
	UART_int(btn_nr);
	UWT("FLG: ");
	UART_int(btn_pnt->FLGS.reg);
	UWT("lpc: ");
	UART_int(btn_pnt->long_press_cnt);
	if(btn_nr == 2)
	{
		UWT("\r\n");
		
		rnd_cnt = 0;
	}
}
#endif	
	
}


#endif



#if 1

static uint8_t get_button_state_from_port(uint8_t btn_number){
	
	uint8_t  ret_value = 0;
	
	
	assert(btn_number < 3);
	
	switch (btn_number)
	{
		
		case 0:
  
			ret_value = BTN_INCREMENT;	
   
		break;
		case 1:
			ret_value = BTN_DECREMENT;	
		break;
		case 2:
			ret_value = BTN_GRABAR;	
		break;			
		default:
#if 0
			errhandler(e_NON_EXISTING_BUTTON_INPUT);
#endif
		break;

	}
	
	return ret_value;
	
}

#else

static uint8_t get_button_state_from_port(uint8_t btn_number){
	
	uint8_t  ret_value = 0;
	
	
	assert(btn_number < 3);
	
	switch (btn_number)
	{
		
		case 0:
    DB_LED = true;
			ret_value = IO_Read_channel(IO_BTN_INCREMENT);
    DB_LED = false;      
		break;
		case 1:
			ret_value = IO_Read_channel(IO_BTN_DECREMENT);	
		break;
		case 2:
			ret_value = IO_Read_channel(IO_BTN_GRABAR);	
		break;			
		default:
#if 0
			errhandler(e_NON_EXISTING_BUTTON_INPUT);
#endif
		break;

	}
	
	return ret_value;
	
}

#endif


