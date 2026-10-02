// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// setting the internal clock to 8 MHz FOSC --> which makes 2 Fcy (2MIPS)
#include "Clock.h"

#include "Global.h"

#include "timers.h"

#include "io_port_sfr_names.h"

#include "generic_union_flgs.h"


// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  
#ifndef FILE_CLOCK_DB_ENABLED
#define FILE_CLOCK_DB_ENABLED 0
#endif
#if FILE_CLOCK_DB_ENABLED
#define DB_PRINT(str) G_DB_PRINT(str)
#define UART_int(var) G_UART_INT(var)
#else
#define DB_PRINT(str)
#define UART_int(var)
#endif
// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //

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
	
  // datasheet --> switching to the PLL can take +- 2ms --> 
	my_delay_ms(5u);
  
  
  
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
	
  // datasheet --> switching to the PLL can take +- 2ms --> 
	my_delay_ms(5u);
  
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
  

  FAST_CLOCK = FALSE;
  
}



#endif


















