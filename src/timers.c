// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com



//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //
#include <stdint.h>

#include "timers.h"

#include "Global.h"



//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //





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
  
	
	
  TMR4ON = false;
	
  
  if(FAST_CLOCK == true)
  {
    
    T4_PRESCALER = TMR4_64_PRESCALER;
    T4_POSTSCALER = TMR4_10_POSTSCALER;
    PR4 = 250;	
    tmr4_200ms_of = 10;
    // with a cnt to 50 for 1000ms
  }
  else
  {
    T4_PRESCALER = TMR2_01_PRESCALER;
    T4_POSTSCALER = TMR2_11_POSTSCALER;
    PR4 = 142;	
    tmr4_200ms_of = 1;
    // directly to 200ms
  }


}



//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //





// EOF