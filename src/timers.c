// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#line 6 "timers.c"

//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //
#include <stdint.h>

#include "timers.h"

#include "Global.h"

#include "generic_union_flgs.h"

#include "handlers.h"

#include "io_port_sfr_names.h"

#include "my_assert.h"

#if DEBUGGING_IS_ON||DEBUGGING_BB_IS_ON
#include "UART.h"
#endif

// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  
#ifndef FILE_TIMERS_DB_ENABLED
#define FILE_TIMERS_DB_ENABLED 0
#endif
#if FILE_TIMERS_DB_ENABLED
#define DB_PRINT(str) G_DB_PRINT(str)
#define UART_int(var) G_UART_INT(var)
#else
#define DB_PRINT(str)
#define UART_int(var)
#endif
// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - // 


//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //



//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
#if USE_RX_CLOCK  
#define TMR1_2_SECOND_OF_CNT 4u  // (uint8_t)MIPS*4u	
#else
#define TMR1_2_SECOND_OF_CNT 30u  // (uint8_t)MIPS*4u	
#endif

#define TMR1_1_SCOND_OF_CNT   15u
#define TMR1_4_SECOND_OF_CNT 60u  // (uint8_t)MIPS*8u	
#define TMR1_10_SECOND_OF_CNT 150u  // (uint8_t)MIPS*8u	
#define TMR1_3_SECOND_OF_CNT 45
#define TMR1_5_SECOND_OF_CNT 75

//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 

static const uint8_t timeout_setter[4] = {
  
  TMR1_2_SECOND_OF_CNT,
  TMR1_4_SECOND_OF_CNT,
  TMR1_10_SECOND_OF_CNT,
  TMR1_5_SECOND_OF_CNT,
  
};






//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //

volatile uint8_t tmr4_200ms_of = 10;

volatile static uint8_t timeout_cnt = TMR1_2_SECOND_OF_CNT;

static tmr1_id_t tmr1_id = NUM_TMR1_ID;

static uint8_t timeout_cnt_setter = TMR1_2_SECOND_OF_CNT;  // TMR1_4_SECOND_OF_CNT; // 


//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

static void tmr1_timeout_handler(void);



//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //


#if 1

void timers_set_tmr1_id(tmr1_id_t t_id){

#if 0 // DEBUGGING_IS_ON||DEBUGGING_BB_IS_ON
  DB_PRINT("\r\nTMR1: ");
  UART_int(t_id);
  UART_CRLF;
#endif  


  if(TMR1_ON == TRUE)
  {
  
    DB_PRINT("\r\nERR_tmr1. activ t_id: ");
    UART_int(t_id);
    DB_PRINT(" ");
    UART_int(timeout_cnt);

    TMR1_ON = false;

  }


  tmr1_id = t_id;
  timeout_cnt_setter = timeout_setter[tmr1_id];

}



void timers_tmr1_decreaser(void){
  
  timeout_cnt--;
  
  if(timeout_cnt == 0u)
  {
    
    tmr1_timeout_handler();

  }
  

}


// reset all flgs and tmr to 0
void start_timeout_tmr(void){
  
  reset_timeout_timer();
  
  TMR1_IE = true;
	TMR1_ON = true;
  
}

// stop tmr 1 and its IE
void stop_timeout_tmr(void){
  
  TMR1_ON = false;
  TMR1_IE = false;
	
}



#if !USE_TMR1_FLG

uint8_t timeout_checker(void){
	
	uint8_t ret_value = 0;
	
  assert(tmr1_id != NUM_TMR1_ID);
  
	if(TMR1IF == true)
	{		
		TMR1IF = false;
    
		timeout_cnt--;
    
		if(timeout_cnt == 0)
		{	
			ret_value = true;
		}
	}

	return ret_value;
}

#endif



void reset_timeout_timer(void){
	  
  uint8_t t_tmr1on = (TMR1_ON != 0u); // for misra
  uint8_t t_tmr1_ie = (TMR1_IE != 0u);    
    
  // uint8_t t_tmr1_ie = TMR1_IE;
  // uint8_t t_tmr1on = TMR1_ON;
    
  TMR1_ON = FALSE;
  
  TIMEOUT_FLG = FALSE;
  
	timeout_cnt = timeout_cnt_setter; 
  
	TMR1IF = FALSE;
  
	TMR1H = 0u;
  
	TMR1L = 0u;
  
	TMR1_ON = t_tmr1on;
  
  TMR1_IE = t_tmr1_ie;
  
}

#endif


// TMR is for the 200ms base time around which the eRTC runs and the 
// tilt sensor sampling
void configure_tmr4(void){
  
	
  TMR4_ON = FALSE;
	 
  if(FAST_CLOCK == TRUE)
  {
    
    T4_PRESCALER = eRTC_TMR_PSA;
    T4_POSTSCALER = eRTC_TMR_POST;
    PR4 = eRTC_TMR_PR;	
    tmr4_200ms_of = eRTC_TMR_CNT;
    
    
    OPTION_REG = TMR0_CFG;
    

  }
  else
  {
    T4_PRESCALER = eRTC_TMR_PSA_SLOW_CLCK;
    T4_POSTSCALER = eRTC_TMR_POST_SLOW_CLCK;
    PR4 = eRTC_TMR_PR_SLOW_CLCK;	
    tmr4_200ms_of = eRTC_TMR_CNT_SLOW_CLCK;
    // directly to 200ms
    
    OPTION_REG = TMR0_CFG_SLOW_CLCK;

  }

}



void init_tmr0(void){

	OPTION_REG = TMR0_CFG;  // (uint8_t)0x81u;

  
	TMR0_IE = FALSE;
	TMR0_IF = FALSE;
	

}


// a time out timer --> when there is the need to 
// avoid an endless wait for some event to be happening
// base time --> 50ms therefore we need some counter to count up to or down to...
void init_tmr1(void){
	
	
	T1_PRESCALER = TMR1_MIPS_PSA;
  
	TMR1_ON = FALSE;
  TMR1_IE = FALSE;
  TMR1_IF = FALSE;
	
  TMR1H = 0u;
	TMR1L = 0u;
  
}



void init_wdt(void){
	
	// TODO --> check WDT overflow time
	WDTCONbits.WDTPS = WDT_TIMEOUT_016s_timeout;  // 0x0Bu;  // 0x0c = 4seconds
  
	WDTCONbits.SWDTEN = FALSE;
  

}


//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //


// when the timeout timer expires handles here the case for it
static void tmr1_timeout_handler(void){
  
  
  switch (tmr1_id)
  {
    case RX_LUZ_TIME_OUT:
    
      TIMEOUT_FLG = TRUE;
      
    break;
    
    case GPS_UART_TIMEOUT:
    
      handlers_generic_set_handler_FLG(e_gps_test_reception);
      
    break;
    
    case STATUS_LED_TIMEOUT:
    
      STATUS_LED_GREEN_OFF();
      STATUS_LED_RED_OFF();
      
    break;
    
    
    
    default:
#if DEBUGGING_IS_ON&&G_ENABLE_ASSERT
      while(1)
      {
        DB_PRINT("\r\nassert\r\n");
      }
        // assert(false);
#endif      
    break;
    
    
  }
  
  TMR1_ON = FALSE;
  
}

static void compiler_delay_us(uint32_t delay_us){
  /* The delay built-in accepts constants only. */
  while(delay_us >= 1000UL)
  {
    __delay_us(1000u);
    delay_us -= 1000UL;
  }

  while(delay_us >= 100UL)
  {
    __delay_us(100u);
    delay_us -= 100UL;
  }

  while(delay_us >= 10UL)
  {
    __delay_us(10u);
    delay_us -= 10UL;
  }

  while(delay_us != 0UL)
  {
    __delay_us(1u);
    delay_us--;
  }
}

void my_delay_ms(uint16_t ms_cnt){
  
  uint32_t delay_us = (uint32_t)ms_cnt * 1000UL;

  if(FAST_CLOCK == FALSE)
  {
    /* Convert real microseconds to the compiler-calibrated slow-clock value. */
    delay_us = (delay_us + (SLOW_CLCK_DIVIDER / 2u)) / SLOW_CLCK_DIVIDER;
  }

  compiler_delay_us(delay_us);
}













// EOF
