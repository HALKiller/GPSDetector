// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// setting the internal clock to 8 MHz FOSC --> which makes 2 Fcy (2MIPS)
#include "Clock.h"

#include "Global.h"

#include "io_port_sfr_names.h"

#include "generic_union_flgs.h"




#if MIPS == 8

#define CLOCK_SETTING CLOCK_8_32_MHZ_HF_CLOCK
#define PLL_SETTING 0x01u

#elif MIPS == 4

#define CLOCK_SETTING CLOCK_16_MHZ_HF_CLOCK
#define PLL_SETTING 0u

#elif MIPS == 2

#define CLOCK_SETTING CLOCK_8_32_MHZ_HF_CLOCK
#define PLL_SETTING 0u

#elif MIPS == 1

#define CLOCK_SETTING CLOCK_4_MHZ_HF_CLOCK
#define PLL_SETTING 0u

#else
  
wat? because that shall not be possible! 

#endif



#if 1

// uint8_t fast_clock = false;

#if MIPS == 8

void init_clock(void){
	
 
	// The PLL needs this setup
	OSCCONbits.SCS = 0u;  // false;
	// internal with PLL to get 32MHz
	OSCCONbits.IRCF = CLOCK_SETTING;
	
	// PLL Enabled
	OSCCONbits.SPLLEN = PLL_SETTING;
	
  while(OSCSTATbits.HFIOFR == 0u)
  {
    // just wait till it is locked
  }
  
	while(OSCSTATbits.HFIOFL == 0u)
  {
    // just wait till it is stable
  }
  
  FAST_CLOCK = TRUE;
	
}


#else

void init_clock(void){
	
 
	// The PLL needs this setup
	OSCCONbits.SCS = 0u;  // false;
	// internal with PLL to get 32MHz
	OSCCONbits.IRCF = CLOCK_SETTING;
	
	// PLL Enabled
	OSCCONbits.SPLLEN = PLL_SETTING;
	
  while(OSCSTATbits.HFIOFR == 0u)
  {
    // just wait till it is locked
  }
	while(OSCSTATbits.HFIOFL == 0u)
  {
    // just wait till it is stable
  }
  
  FAST_CLOCK = TRUE;
	
}

#endif

void set_slow_clock(void){
  

  OSCCONbits.SCS = 0x00u;  // 0x2u;
  
  OSCCONbits.IRCF = CLOCK_500KHZ_MF_CLOCK;  // CLOCK_3125KHZ_MF_CLOCK;  //0x0C; // 31.25kHz   0x7;
  
  OSCCONbits.SPLLEN = 0u;
  
  // while((bool)!OSCSTATbits.MFIOFR)
  while(OSCSTATbits.MFIOFR == 0u)
  {
    // just wait till it is locked
  }
  
  // fast_clock = false;
  FAST_CLOCK = FALSE;
  
  
}



#endif


















