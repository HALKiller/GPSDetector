// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com



//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //
#include <stdint.h>

#include "timers.h"

#include "Global.h"

#include "generic_union_flgs.h"

#include "handlers.h"

#include "my_assert.h"

#if DEBUGGING_IS_ON
#include "UART.h"
#endif

//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //



//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 
#define TMR1_2_SECOND_OF_CNT 30u  // (uint8_t)MIPS*4u	
#define TMR1_4_SECOND_OF_CNT 60u  // (uint8_t)MIPS*8u	


//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 

static const uint8_t timeout_setter[2] = {
  
  TMR1_2_SECOND_OF_CNT,
  TMR1_4_SECOND_OF_CNT,
  
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

#if DEBUGGING_IS_ON&&0
  DB_PRINT("TMR1: ");
  UART_int(t_id);
  UART_CRLF;
#endif  

  if(TMR1_ON == TRUE)
  {
    DB_PRINT("ERR_tmr1");
  }
  
  tmr1_id = t_id;
  timeout_cnt_setter = timeout_setter[tmr1_id];
  
#if DEBUGGING_IS_ON&&0
  DB_PRINT("cnt: ");
  UART_int(timeout_cnt_setter);
  UART_CRLF;
#endif    
}



void timers_tmr1_decreaser(void){
  
  
  timeout_cnt--;
  
  if(timeout_cnt == 0u)
  {
    
    tmr1_timeout_handler();

  }
  

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
    
  // bool t_tmr1_ie = TMR1_IE;
  // bool t_tmr1on = TMR1_ON;
    
  // bool t_tmr1_ie = (bool)TMR1_IE;
  // bool t_tmr1on = (bool)TMR1_ON;
  // bool t_tmr1on = (TMR1_ON != 0u);
  // bool t_tmr1_ie = (TMR1_IE != 0u);

  
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
	WDTCONbits.WDTPS = WDT_TIMEOUT_004s_timeout;  // 0x0Bu;  // 0x0c = 4seconds
  
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


// EOF