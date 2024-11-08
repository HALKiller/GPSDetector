// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// the isr has the interrupts to handle:

// reception of a character in UART --> we are also chacking here for an OERR 
// because it is the easiest place to do so


#include "Global.h"
#include "handlers.h"
#include "io_port_sfr_names.h"
#include "generic_union_flgs.h"
#include "timers.h"
#include "e_rtc.h"
#include "pwm_luz.h"

// #include "UART.h"


#include "ring_buffer.h"


#define TMR0_ADJUSTMENT (uint8_t)0x05u
// #define TMR0_ADJUSTMENT (uint8_t)0x65u

// eRTC related -------------------------
// #define SECONDS_PER_DAY (uint32_t)864000u // becasue of decimo seconds we have a digit more

static uint8_t pwm_of_cnt = 0;


void __interrupt() isr(void){

// void interrupt isr(void){	

  unsigned char rx_data;
  
  // becaseu on the first run there is going to be F_CLOCK
  
  

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

// this finds usage only for the transmission of SPI and messages from the DDS and also
// in the rx_luz part
  if((TMR2_IE == true) && (TMR2_IF == true))
  {
    TMR2_ON = false;
    TMR2_IF = false;
  }
	
	// OF time = 2.04ms
	// this is only going to be for the PWM --> 
  // therfore we need that still to be handled  
  // when to switch on and off this is a percentage value and therefroe i can always cnt till 10 and
  // then reset the luz
	if((TMR0_IE == true) && (TMR0_IF == true))
	{
    
    pwm_of_cnt++;
    
    if(pwm_of_cnt == get_pwm_luz_pwm_value()) // pwm_luz.pwm_value)
    {
      LED = 0u;
      DB_LED_2_OFF;
    }
    
    if(pwm_of_cnt >= 10u)
    {
      LED = 1u;
      DB_LED_2_ON;
      pwm_of_cnt = 0u;
    }
    
		
    if(FAST_CLOCK == false)
    {
      TMR0 = TMR0 + TMR0_ADJUSTMENT;
    }
    
		TMR0_IF = false;
    // DB_LED2_SWAP;

	}
  

  // this is basically the e_rtc clocking here...
	if((TMR4_IE == true) && (TMR4_IF == true))
	{
		
    tmr4_of_cnt--;
    
    if((FAST_CLOCK == false) || (tmr4_of_cnt == (uint8_t)0u))
    {
    
      eRTC_clock_incrementer();
      
      if(SWITCH_CLOCK == true)
      {
        handlers_generic_set_handler_FLG(e_switch_clock_handler);
        TMR4ON = false; // stop the Timer
        SWITCH_CLOCK = false;
      }
      else
      {
        
        // DB_LED2_SWAP;
        
        handlers_generic_set_handler_FLG(e_200ms_h);
        tmr4_of_cnt = tmr4_200ms_of;
        
      }
      
      tmr4_of_cnt = tmr4_200ms_of;
		}
    
		TMR4_IF = false;

	}
	


}













// EOF