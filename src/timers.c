// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com



//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //
#include <stdint.h>

#include "timers.h"

#include "Global.h"

#include "generic_union_flgs.h"

//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //



uint8_t tmr4_200ms_of = 10;


//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 



//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 



//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //



//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //



//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //


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
	
}


void configure_tmr2(void){
  
	
	
  TMR2_ON = FALSE;
	
#if 0	

	T2_PRESCALER = TMR2_64_PRESCALER;
	T2_POSTSCALER = TMR2_15_POSTSCALER;
	PR2 = 250u;	

#else	
	
#if MIPS==1
	
	T2_PRESCALER = TMR2_04_PRESCALER;
	T2_POSTSCALER = TMR2_15_POSTSCALER;	// 0x0E;	// 0b0111;	// T2CONbits.T2OUTPS = 0b0111; // PostScaler 7  T2CONbits.TOUTPS = 0b0111; // PostScaler 7	
	PR2 = 250;	//125;  // 250; // thereforefor 150ms we need a counter to 6 --> for halfbit cnt to 3

#elif MIPS==2

MISSING

#elif MIPS==4

MISSING

#elif MIPS==8
	
	T2_PRESCALER = TMR2_64_PRESCALER;
	T2_POSTSCALER = TMR2_15_POSTSCALER;
	PR2 = (uint8_t)125;	

#else	
	
error again --> that MIPS is not standard so far --> write it extra out

#endif

#endif

}




//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //





// EOF