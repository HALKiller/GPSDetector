// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// the isr has the interrupts to handle:

// reception of a character in UART --> we are also chacking here for an OERR 
// because it is the easiest place to do so


#include "Global.h"
#include "handlers.h"
#include "io_port_sfr_names.h"
#include "generic_union_flgs.h"


#include "ring_buffer.h"


#define TMR0_ADJUSTMENT 4

// eRTC related -------------------------
#define SECONDS_PER_DAY (uint32_t)864000u // becasue of decimo seconds we have a digit more

static volatile uint8_t tmr4_of_cnt = 0;

static volatile uint32_t eRTC_second_cnt = 0;

static void eRTC_clock_incrementer(void);

extern uint8_t tmr4_200ms_of;


void __interrupt() isr(void){

// void interrupt isr(void){	

  unsigned char rx_data;
  
  static tmr4_of_cnt = tmr4_200ms_of;


	if(RCSTAbits.OERR == true)
	{
		RCSTAbits.CREN = false;
		asm ("nop");
		RCSTAbits.CREN = true;
	}


	while((RX_IE == true) && (RX_IF == true))
	{			
		
		rx_data = RCREG;
		set_data_value_into_buffer(rx_data);
		
	}


	
	// OF time = 2.04ms
	
	if((TMR0_IE == true) && (TMR0_IF == true))
	{
	
		TMR0_IF = false;
		TMR0 = TMR0 + TMR0_ADJUSTMENT;
		handlers_generic_set_handler_FLG(e_2ms_of_handler);

	}
  
// well then --> lets use TMR_4" purely for the coloring of the LEDS of the BAT_Level...


	if((TMR4_IE == true) && (TMR4_IF == true))
	{
		
    tmr4_of_cnt--;
    
    if(tmr4_of_cnt == (uint8_t)0u)
    {
    
      eRTC_clock_incrementer();
      
      if(SWITCH_CLOCK == true)
      {
        handlers_generic_set_handler_FLG(e_switch_clock_handler);
        TMR4ON = false; // stop the Timer
      }
      else
      {
        handlers_generic_set_handler_FLG(e_200ms_h);
      }

		}
		TMR4_IF = false;

	}
	


}


static void eRTC_clock_incrementer(void){
  
  eRTC_second_cnt = eRTC_second_cnt + 2u;
  
  if(eRTC_second_cnt >= SECONDS_PER_DAY)
  {
    
    eRTC_second_cnt = 0;
    
  }
  
  
  
  
}


















// EOF