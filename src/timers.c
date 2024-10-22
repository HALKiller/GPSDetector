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



// f.e.
// 8 MIPS and 200ms 
    // T4_PRESCALER = TMR4_64_PRESCALER;
    // T4_POSTSCALER = TMR4_10_POSTSCALER;
    // PR4 = 250;	
    // cnt = 10;
    
// 4 MIPS and 200ms
    // T4_PRESCALER = TMR4_64_PRESCALER;
    // T4_POSTSCALER = TMR4_10_POSTSCALER;
    // PR4 = 250;	
    // cnt = 5;

// 2 MIPS and 200ms
    // T4_PRESCALER = TMR4_64_PRESCALER;
    // T4_POSTSCALER = TMR4_10_POSTSCALER;
    // PR4 = 125;	
    // cnt = 5;    
    
// 1 MIPS and 200ms
    // T4_PRESCALER = TMR4_16_PRESCALER;
    // T4_POSTSCALER = TMR4_10_POSTSCALER;
    // PR4 = 125;	
    // cnt = 10;        
    
// 31.25kHz Clock and 200ms
    // T4_PRESCALER = TMR4_01_PRESCALER;
    // T4_POSTSCALER = TMR4_11_POSTSCALER;
    // PR4 = 142;	
    // cnt = 1;    



void configure_tmr4(void){
  
	
	
  TMR4_ON = FALSE;
	
  
  if(FAST_CLOCK == TRUE)
  {
    
    T4_PRESCALER = TMR4_64_PRESCALER;
    T4_POSTSCALER = TMR4_10_POSTSCALER;
    PR4 = (uint8_t)250u;	
    tmr4_200ms_of = (uint8_t)10u;
    // with a cnt to 50 for 1000ms
  }
  else
  {
    T4_PRESCALER = TMR4_01_PRESCALER;
    T4_POSTSCALER = TMR4_11_POSTSCALER;
    PR4 = (uint8_t)142;	
    tmr4_200ms_of = (uint8_t)1u;
    // directly to 200ms
  }


}



void init_tmr0(void){



	TMR0_IE = FALSE;
	TMR0_IF = FALSE;


	OPTION_REG = (uint8_t)0x80u;
	OPTION_REGbits.PS = TMR0_004_PRESCALER;
  

	

}



void init_tmr1(void){
	
	// T1CONbits.T1CKPS = 0x03;	// 8:1 Prescaler
	T1_PRESCALER = TMR1_8_PRESCALER;
	
	
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