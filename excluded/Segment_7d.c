//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //

#include "Segment_7d.h"
#include "Global.h"

#include "timers.h"
#include "io_port_sfr_names.h"
// #include "SM.h"
#include "UART.h"

#include "my_assert.h"

#include "device_driver_config.h"


#include "stdint.h"

//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //


union s_port{
	uint16_t value;
	struct{
		unsigned shadow_pin_b0	:1;
		unsigned shadow_pin_b1	:1;
		unsigned shadow_pin_b2	:1;
		unsigned shadow_pin_b3	:1;
		unsigned free_0	:4;
    unsigned free_1	:8;
	};
	
};

struct udt_seg_7d{
	
	union{
		uint8_t reg;
		struct{
			unsigned is_blinking	: 1;	// showing --> value, dark, value
			unsigned is_showing		: 1;
			unsigned is_alternating	: 1;	// showing --> value1, value2, value1
			unsigned main_value_is_showing : 1;	// blinking and alternating needs that one --> value1, dark, value2, dark, value1...
			unsigned blinking_is_showing : 1;
			unsigned is_an_error_message	: 1;
			unsigned free: 2;
		};
	}FLGS;
	
	uint8_t main_value;
	uint8_t secondary_value;
	uint8_t value_to_display;	// because that really depedns on what is goign to be displayed...
	
	uint8_t tens_7s;
	uint8_t ones_7s;
	union s_port shadow_port;
	
	
}Seg_7d;




#define DB_SEG_7d 0

// inverted output is for singking the current and not driving 
#define INVERTED_OUTPUT 0
#define BITS_TO_BANG_OUT 16
//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 
#if GdL_V20_1 // USE_MAX6969
static const uint8_t const_dark_seg = 10;
#else
static const uint8_t const_dark_seg = 15;
#endif
 
static const uint8_t const_max_number_to_display = 99;
static const uint8_t const_seg_7d_dark_value = 0xFF;





//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //


static void blinking_timer_handler(struct udt_seg_7d *seg_7d_pnt);
static void show_alernating_value_timer_handler(struct udt_seg_7d *seg_7d_pnt);
static void what_to_show_decider(struct udt_seg_7d *seg_7d_pnt);

static void move_shadow_port_to_hardware_pin(union s_port *port_pnt);
static void fill_bcd_bytes(struct udt_seg_7d *seg_7d_pnt);
static void update_IO_ports_with_bcd_values(struct udt_seg_7d *seg_7d_pnt);
static void short_delay(void);
static void all_seg_7d_dark(struct udt_seg_7d *seg_7d_pnt);
static void load_calibration_message(struct udt_seg_7d *seg_7d_pnt);
static void load_error_message(struct udt_seg_7d *seg_7d_pnt);

static void seg7_d_debugger(void);

//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

void seg_7d_init(void){
	
	Seg_7d.FLGS.reg = 0;
	
	Seg_7d.main_value = 1;
	Seg_7d.secondary_value = 0;
	Seg_7d.value_to_display = 1;	
	Seg_7d.tens_7s = 0;
	Seg_7d.ones_7s = 1;
#if DEBUGGING_IS_ON&&GdL_V20_1	
  seg7_d_debugger();
#endif  
  
}




void seg_7d_debugger(void){
	
	uint8_t hlooper = 0;
	
	static struct udt_seg_7d *seg_7d_pnt = &Seg_7d;
	
	for(hlooper = 0; hlooper < 20; hlooper++)
	{
		
		Seg_7d.value_to_display = hlooper;
		
		fill_bcd_bytes(seg_7d_pnt);
	
		update_IO_ports_with_bcd_values(seg_7d_pnt);
		
		UWT("Seg_set: ");
		UART_int(hlooper);
		UWT("\r\n");
		
		my_delay_ms(150);
		
		Seg_7d.value_to_display = const_seg_7d_dark_value;
		
		fill_bcd_bytes(seg_7d_pnt);
	
		update_IO_ports_with_bcd_values(seg_7d_pnt);
		
		my_delay_ms(250);
		
	}
	

	
	seg_7d_init();
	
	
	
	
	
}






// 200 ms of --> move it out---


#if 1

void refresh_7Sd(void){
	
	static struct udt_seg_7d *seg_7d_pnt = &Seg_7d;
	

	blinking_timer_handler(seg_7d_pnt);
	
	show_alernating_value_timer_handler(seg_7d_pnt);
	
	what_to_show_decider(seg_7d_pnt);
	
	fill_bcd_bytes(seg_7d_pnt);
	
	update_IO_ports_with_bcd_values(seg_7d_pnt);

#if 0	
	UWT("seg_7d_SET\r\n");
#endif	
	
}

#elif 1


void refresh_7Sd(void){
	
	static struct udt_seg_7d *seg_7d_pnt = &Seg_7d;

	blinking_timer_handler(seg_7d_pnt);
	
	show_alernating_value_timer_handler(seg_7d_pnt);
	
	what_to_show_decider(seg_7d_pnt);
	
	fill_bcd_bytes(seg_7d_pnt);
	
	update_IO_ports_with_bcd_values(seg_7d_pnt);
	
}


#else
	
void refresh_7Sd(void){
static struct udt_seg_7d *seg_7d_pnt = &Seg_7d;


	if(seg_7d_pnt->FLGS.is_an_error_message == true)
	{
		load_error_message(seg_7d_pnt);
	}
	else
	{
		
		blinking_timer_handler(seg_7d_pnt);
		
		show_alernating_value_timer_handler(seg_7d_pnt);
		
		what_to_show_decider(seg_7d_pnt);
		fill_bcd_bytes(seg_7d_pnt);
		
	}

	
	
	update_IO_ports_with_bcd_values(seg_7d_pnt);
	
	
}

#endif

// moves a vlue to the main value to be displayed...
void segment_7d_write_main_value(uint16_t the_value){
	
	assert(the_value <= const_max_number_to_display);

	if(the_value <= const_max_number_to_display)
	{
		Seg_7d.main_value = the_value;
	}
	else
	{
		UWT("OF_seg7_main_value\r\n");
	}
	
#if DB_SEG_7d
		UWT("\r\n SEG_7d new main value!\r\n");
		UART_int(Seg_7d.main_value);
#endif		
	
}

#if 1
void segment_7d_swoff_seg_7d(void){
	
#if DB_SEG_7d
		UWT("\r\n SEG_7d received swoff order!\r\n");
#endif	
	
	Seg_7d.main_value = const_seg_7d_dark_value;
}
#endif

// moves a value into the secondary value to be displayed
void segment_7d_write_secondary_value(uint16_t the_value){
	
	assert(the_value <= const_max_number_to_display);
	
	if(the_value <= const_max_number_to_display)
	{
		Seg_7d.secondary_value = the_value;
	}
	else
	{
		UWT("OF_seg7_secondary_value\r\n");
	}
	
	
}

// sets the alternating On or Off
void segment_7d_set_alternating_flg(uint8_t set_reset_flg){
	
	Seg_7d.FLGS.is_alternating = set_reset_flg;
	
}


void segment_7d_reset_alternating_flg(void){
	
	Seg_7d.FLGS.is_alternating = false;
	
}


void segment_7d_set_blinking_flg(uint8_t set_reset_flg){
	
	Seg_7d.FLGS.is_blinking = set_reset_flg;
	
}


void segment_7d_reset_blinking_flg(void){
	
	Seg_7d.FLGS.is_blinking = false;
	
}


uint16_t seg_7d_get_seg_7d_flgs_value(void){
	
	return Seg_7d.FLGS.reg;
	
}


void seg_7d_set_is_error_message(uint8_t set_reset){
	
	Seg_7d.FLGS.is_an_error_message = set_reset;
	
}


//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //


// with 200 ms refresh rate that would be 600 ms show/swap/show
static void blinking_timer_handler(struct udt_seg_7d *seg_7d_pnt){
	static uint8_t entrance_cnt_for_blinking = 0;
	const uint8_t const_blink_frequency_setter = 3;

	entrance_cnt_for_blinking++;
	if(entrance_cnt_for_blinking >= const_blink_frequency_setter)
	{
		entrance_cnt_for_blinking = 0;
		seg_7d_pnt->FLGS.blinking_is_showing = !seg_7d_pnt->FLGS.blinking_is_showing;
	}	

}


static void show_alernating_value_timer_handler(struct udt_seg_7d *seg_7d_pnt){

static uint8_t entrance_cnt_for_alternating = 0;
const uint8_t const_alternater_frequency_setter = 6;


	entrance_cnt_for_alternating++;
	if(entrance_cnt_for_alternating >= const_alternater_frequency_setter)
	{
		entrance_cnt_for_alternating = 0;
		seg_7d_pnt->FLGS.main_value_is_showing = !seg_7d_pnt->FLGS.main_value_is_showing;
	}	

}


// * because we can have combinations of values to show there are a lot of different combinations:
// * blinking
// * alternativ value
// * alternating between main and alternativ value
// alternating and blinking combining
static void what_to_show_decider(struct udt_seg_7d *seg_7d_pnt){
uint8_t temp_flg = 0;

	
	if(seg_7d_pnt->FLGS.is_blinking == true)
	{
		temp_flg = temp_flg + 1;
	}
	if(seg_7d_pnt->FLGS.is_alternating == true)
	{
		temp_flg = temp_flg + 2;
	}
	if(seg_7d_pnt->FLGS.main_value_is_showing == true)
	{
		temp_flg = temp_flg + 4;
	}
	if(seg_7d_pnt->FLGS.blinking_is_showing == true)
	{
		temp_flg = temp_flg + 8;
	}
#if DB_SEG_7d
	UWT("s_decider: ");
	UART_int(temp_flg);
#endif	
	switch (temp_flg)
	{
		case 0:
			seg_7d_pnt->value_to_display = seg_7d_pnt->main_value;
		break;
		case 1:
			seg_7d_pnt->value_to_display = const_seg_7d_dark_value;
		break;
		case 2:
			seg_7d_pnt->value_to_display = seg_7d_pnt->secondary_value;
		break;
		case 3:
			seg_7d_pnt->value_to_display = const_seg_7d_dark_value;
		break;
		case 4:
			seg_7d_pnt->value_to_display = seg_7d_pnt->main_value;
		break;
		case 5:
			seg_7d_pnt->value_to_display = const_seg_7d_dark_value;
		break;
		case 6:
			seg_7d_pnt->value_to_display = seg_7d_pnt->main_value;
		break;
		case 7:
			seg_7d_pnt->value_to_display = const_seg_7d_dark_value;
		break;
		case 8:
			seg_7d_pnt->value_to_display = seg_7d_pnt->main_value;
		break;
		case 9:
			seg_7d_pnt->value_to_display = seg_7d_pnt->main_value;
		break;
		case 10:
			seg_7d_pnt->value_to_display = seg_7d_pnt->secondary_value;
		break;
		case 11:
			seg_7d_pnt->value_to_display = seg_7d_pnt->secondary_value;
		break;
		case 12:
			seg_7d_pnt->value_to_display = seg_7d_pnt->main_value;
		break;
		case 13:
			seg_7d_pnt->value_to_display = seg_7d_pnt->main_value;
		break;
		case 14:
			seg_7d_pnt->value_to_display = seg_7d_pnt->main_value;
		break;
		case 15:
			seg_7d_pnt->value_to_display = seg_7d_pnt->main_value;
		break;	
		default:
		break;
		
	
		
	}
	
#if 0
	UWT("Seg7d: ");
	UART_int(seg_7d_pnt->value_to_display);
	UWT("\r\n");
	

#endif
	
}

#if 1

static void fill_bcd_bytes(struct udt_seg_7d *seg_7d_pnt){


#if DB_SEG_7d
	UWT("Fbcd_value: ");
	UART_int(seg_7d_pnt->value_to_display);
#endif

	if(seg_7d_pnt->value_to_display == const_seg_7d_dark_value)
	{
		all_seg_7d_dark(seg_7d_pnt);
	}
	else
	{

		
		seg_7d_pnt->tens_7s = seg_7d_pnt->value_to_display  / 10;
		seg_7d_pnt->ones_7s = (seg_7d_pnt->value_to_display -  (seg_7d_pnt->tens_7s * 10));
		
	}
  
}

#else
	
static void fill_bcd_bytes(struct udt_seg_7d *seg_7d_pnt){

	if(seg_7d_pnt->value_to_display == const_seg_7d_dark_value)
	{
		all_seg_7d_dark(seg_7d_pnt);

	}
	else
	{

		seg_7d_pnt->hundred_7s = seg_7d_pnt->value_to_display / 100;
		seg_7d_pnt->tens_7s = (seg_7d_pnt->value_to_display - seg_7d_pnt->hundred_7s * 100) / 10;
		seg_7d_pnt->ones_7s = (seg_7d_pnt->value_to_display - (seg_7d_pnt->hundred_7s * 100 + seg_7d_pnt->tens_7s * 10));
		
	}
}

#endif


#if GdL_V20_1 // this is with the max6969 chip...


static void update_IO_ports_with_bcd_values(struct udt_seg_7d *seg_7d_pnt){
	
  uint8_t hlooper = 0;
  
  
  const uint8_t seg7d_lookup_t[] = {
    
    0X3F,
    0X06,
    0X5B,
    0X4F,
    0X66,
    0X6D,
    0X7D,
    0X07,
    0X7F,
    0X6F,
    0x00,
    
  };
  
  
	#define MAX_VALUE_TO_DISPLAY_ON_SINGLE_DIGIT 10
	
	assert((MAX_VALUE_TO_DISPLAY_ON_SINGLE_DIGIT > seg_7d_pnt->ones_7s) || (seg_7d_pnt->ones_7s == const_dark_seg));

	assert((MAX_VALUE_TO_DISPLAY_ON_SINGLE_DIGIT > seg_7d_pnt->tens_7s) || (seg_7d_pnt->tens_7s == const_dark_seg));

	
  seg_7d_pnt->shadow_port.value = (seg7d_lookup_t[seg_7d_pnt->ones_7s] << 7) + seg7d_lookup_t[seg_7d_pnt->tens_7s];
  
  if(INVERTED_OUTPUT == true)
  {
    seg_7d_pnt->shadow_port.value = ~seg_7d_pnt->shadow_port.value;
  }
#if DEBUGGING_IS_ON&&0

  UWT("Seg_7d tens: ");
  UART_int(seg_7d_pnt->tens_7s);
  
  UWT("Seg_7d ones: ");
  UART_int(seg_7d_pnt->ones_7s);
  
  UWT("Constructed: ");
  UART_int(seg_7d_pnt->shadow_port.value);
  UWT("\r\n");
  
#endif
  
#if 0
	seg_7d_pnt->shadow_port.value = (seg_7d_pnt->ones_7s << 8) + seg_7d_pnt->tens_7s;
#endif
	
  // that should be low anyway but still...assure the correct state beforehand
  IO_Write_channel(IO_BCD_CLK, LOW);
  
  for(hlooper = 0; hlooper < BITS_TO_BANG_OUT; hlooper++)
  {
    // write the data...
    IO_Write_channel(IO_BCD_DATA, (seg_7d_pnt->shadow_port.value >> (BITS_TO_BANG_OUT - hlooper - 1)) & 0x01);
    //
    short_delay();
    
    IO_Write_channel(IO_BCD_CLK, HIGH);
    
    short_delay();
    
    IO_Write_channel(IO_BCD_CLK, LOW);
    
  }
  
  IO_Write_channel(IO_BCD_LE, HIGH);
  
  short_delay();
  
  IO_Write_channel(IO_BCD_LE, LOW);
  
  
}



#else
  

static void update_IO_ports_with_bcd_values(struct udt_seg_7d *seg_7d_pnt){
	
	#define MAX_VALUE_TO_DISPLAY_ON_SINGLE_DIGIT 10
	
	assert((MAX_VALUE_TO_DISPLAY_ON_SINGLE_DIGIT > seg_7d_pnt->ones_7s) || (seg_7d_pnt->ones_7s == const_dark_seg));

#if DB_SEG_7d
	UWT("d7: ");
	UART_int(seg_7d_pnt->tens_7s);
	UART_int(seg_7d_pnt->ones_7s);
#endif

	
	seg_7d_pnt->shadow_port.value = seg_7d_pnt->ones_7s;
	move_shadow_port_to_hardware_pin(&(seg_7d_pnt->shadow_port));
	
	ACT_BCD_02 = true;
	short_delay();
	ACT_BCD_02 = false;
	
	
	assert((MAX_VALUE_TO_DISPLAY_ON_SINGLE_DIGIT > seg_7d_pnt->tens_7s) || (seg_7d_pnt->tens_7s == const_dark_seg));
	
	seg_7d_pnt->shadow_port.value = seg_7d_pnt->tens_7s;
	move_shadow_port_to_hardware_pin(&(seg_7d_pnt->shadow_port));
	
	ACT_BCD_01 = true;
	short_delay();
	ACT_BCD_01 = false;
	
#if 0	
	assert(MAX_VALUE_TO_DISPLAY_ON_SINGLE_DIGIT > seg_7d_pnt->hundred_7s);
	
	seg_7d_pnt->shadow_port.value = seg_7d_pnt->hundred_7s;
	move_shadow_port_to_hardware_pin(&(seg_7d_pnt->shadow_port));

	ACT_BCD_03 = true;
	short_delay();
	ACT_BCD_03 = false;
#endif
	
}


// this function moves the value from the shadow port to the real HW Port
static void move_shadow_port_to_hardware_pin(union s_port *port_pnt){
	
	BCD_01_A = port_pnt->shadow_pin_b0;
	BCD_01_B = port_pnt->shadow_pin_b1;
	BCD_01_C = port_pnt->shadow_pin_b2;
	BCD_01_D = port_pnt->shadow_pin_b3;
	
}


#endif






static void short_delay(void){
	
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");


	
	
}




static void all_seg_7d_dark(struct udt_seg_7d *seg_7d_pnt){

	
	// seg_7d_pnt->hundred_7s = const_dark_seg;
	seg_7d_pnt->tens_7s = const_dark_seg;
	seg_7d_pnt->ones_7s = const_dark_seg;
	
}



#if 0
static void load_calibration_message(struct udt_seg_7d *seg_7d_pnt){
const uint8_t const_calibration_byte_h = 10;	
	
	seg_7d_pnt->hundred_7s = const_calibration_byte_h;
	seg_7d_pnt->tens_7s = const_dark_seg;
	seg_7d_pnt->ones_7s = const_dark_seg;

}



static void load_error_message(struct udt_seg_7d *seg_7d_pnt){
const uint8_t const_error_byte_h = 14;		
uint8_t err_msg = 0;	
	
	err_msg = get_err_from_SM();
	
	seg_7d_pnt->value_to_display = err_msg;
	
	fill_bcd_bytes(seg_7d_pnt);
	
	seg_7d_pnt->hundred_7s = const_error_byte_h;
	
	
}

#endif


#if GdL_V20_1
static void seg7_d_debugger(void){
	
  uint8_t hlooper = 0;
  
  uint16_t outdata = 1;
  
  uint16_t rnd_cnt = 0;
  
  IO_Write_channel(IO_BCD_ACTIVE, HIGH);
  
  // that should be low anyway but still...assure the correct state beforehand
  IO_Write_channel(IO_BCD_CLK, LOW);
  
  for(rnd_cnt = 0; rnd_cnt < 16; rnd_cnt++)
  {
    
  
    for(hlooper = 0; hlooper < BITS_TO_BANG_OUT; hlooper++)
    {
      // write the data...
      IO_Write_channel(IO_BCD_DATA, (outdata >> (BITS_TO_BANG_OUT - hlooper - 1)) & 0x01);

      short_delay();
      
      IO_Write_channel(IO_BCD_CLK, HIGH);
      
      short_delay();
      
      IO_Write_channel(IO_BCD_CLK, LOW);
      
    }
    
    IO_Write_channel(IO_BCD_LE, HIGH);
    
    short_delay();
    
    IO_Write_channel(IO_BCD_LE, LOW);
    
    outdata = outdata * 2;
  
  my_delay_ms(150);
  }
  

}


#endif




// EOF












