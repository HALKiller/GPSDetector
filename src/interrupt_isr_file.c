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
#include "detector.h"

#include "ring_buffer.h"

#if DEBUGGING_IS_ON
#include "UART.h"

#endif

#define TMR0_ADJUSTMENT (uint8_t)0x05u

static volatile uint8_t pwm_of_cnt = 0;


// because the xc 1.38 needs a different decalration for the ISR becaue it is only C89 conform!!!
#if XC8_V_138
void interrupt isr(void){	
#elif XC8_V_246
void __interrupt() isr(void){
#endif

  unsigned char rx_data;
  
  // becaseu on the first run there is going to be F_CLOCK

#if 1 
	if(RCSTAbits.OERR == TRUE)
	{
		RCSTAbits.CREN = FALSE;
		asm ("nop");
		RCSTAbits.CREN = TRUE;
	}
#else
	if(RCSTAbits.OERR == true)
	{
		RCSTAbits.CREN = false;
		asm ("nop");
		RCSTAbits.CREN = true;
	}
#endif


	while((RX_IE == TRUE) && (RX_IF == TRUE))
	{			
#if 0
    RX_IF = FALSE;
#else    
		rx_data = RCREG;
		set_data_value_into_buffer(rx_data);
#endif		
	}



// this finds usage only for the transmission of SPI and messages from the DDS and also
// in the rx_luz part
  if((TMR2_IE == TRUE) && (TMR2_IF == TRUE))
  {
    TMR2_ON = FALSE;
    TMR2_IF = FALSE;
  }


	
	// OF time = 2.04ms
	// this is only going to be for the PWM --> 
  // therefore we need that still to be handled  
  // when to switch on and off this is a percentage value and therefroe i can always cnt till 10 and
  // then reset the luz
	if((TMR0_IE == TRUE) && (TMR0_IF == TRUE))
	{
#if COMPILE_WITH_PWM_LUZ   

    pwm_of_cnt++;
    
    if(pwm_of_cnt == get_pwm_luz_pwm_value()) // pwm_luz.pwm_value)
    {
      LED = 0u;
#if PWM_LUZ_DEBUG      
      LED_SIMUL_OFF;
#endif      
    }
    
    if(pwm_of_cnt >= 10u)
    {
#if USE_PWM_LED      
      LED = 1u;
#endif      
      
#if PWM_LUZ_DEBUG      
      LED_SIMUL_ON;
#endif      
      pwm_of_cnt = 0u;
    }
    
		
    if(FAST_CLOCK == FALSE)
    {
      TMR0 = TMR0 + TMR0_ADJUSTMENT;
    }
#endif    
		TMR0_IF = FALSE;

	}


#if 1

  // this is basically the e_rtc clocking here...
	if((TMR4_IE == TRUE) && (TMR4_IF == TRUE))
	{

    tmr4_of_cnt--;
    
    if((FAST_CLOCK == FALSE) || (tmr4_of_cnt == (uint8_t)0u))
    {
    
      eRTC_clock_incrementer();
      
      handlers_generic_set_handler_FLG(e_200ms_h);
      
      if(SWITCH_CLOCK == TRUE)
      {
        handlers_generic_set_handler_FLG(e_switch_clock_handler);
        TMR4ON = FALSE; // stop the Timer
        SWITCH_CLOCK = FALSE;
      }

     
      tmr4_of_cnt = eRTC_TMR_CNT; // tmr4_200ms_of;
      
		}

		TMR4_IF = FALSE;

	}

#else
  
  // this is basically the e_rtc clocking here...
	if((TMR4_IE == TRUE) && (TMR4_IF == TRUE))
	{
#if 1		
    tmr4_of_cnt--;
    
    if((FAST_CLOCK == FALSE) || (tmr4_of_cnt == (uint8_t)0u))
    {
    
      eRTC_clock_incrementer();

      if(SWITCH_CLOCK == TRUE)
      {
        handlers_generic_set_handler_FLG(e_switch_clock_handler);
        TMR4ON = FALSE; // stop the Timer
        SWITCH_CLOCK = FALSE;
      }
    
      else
      {
        
        handlers_generic_set_handler_FLG(e_200ms_h);
        
        tmr4_of_cnt = tmr4_200ms_of;
        
      }
     
      tmr4_of_cnt = tmr4_200ms_of;
      
		}
#endif    
		TMR4_IF = FALSE;

	}
  
#endif
	
// TMR1 is a timeout tiemr and for full second timeout timing prepared...
  if((TMR1_IE == TRUE) && (TMR1_IF == TRUE))
  {
    timers_tmr1_decreaser();
    TMR1_IF = FALSE;
  }

}













// EOF