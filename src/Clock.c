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





void init_clock(void){
	
 
	// The PLL needs this setup
	OSCCONbits.SCS = 0u;  // false;
	// internal with PLL to get 32MHz
	OSCCONbits.IRCF = CLOCK_SETTING;
	
	// PLL Enabled
	OSCCONbits.SPLLEN = PLL_SETTING;
	
  while(OSCSTATbits.OSTS == 0u)
  {
    // just wait till it is locked
  }
	
  FAST_CLOCK = TRUE;
  
  // fast_clock = true;
	
}


void set_slow_clock(void){
  

  OSCCONbits.SCS = 0x00u;  // 0x2u;
  
  OSCCONbits.IRCF = CLOCK_3125KHZ_MF_CLOCK;  //0x0C; // 31.25kHz   0x7;
  
  OSCCONbits.SPLLEN = 0u;
  
  // while((bool)!OSCSTATbits.MFIOFR)
  while(OSCSTATbits.MFIOFR == 0u)
  {
    // just wait till it is locked
  }
  
  // fast_clock = false;
  FAST_CLOCK = FALSE;
  
  
}


#if 0

void set_clock_speed(uint8_t high_low_speed){
  
  uint8_t t_GIE = GIE;
  
  GIE = false;
  
  if(high_low_speed == FAST_CLOCK_OSC)
  {
    init_clock();
    fast_clock = true;
  }
  else
  {
    clock_slowdown();
    fast_clock = false;
  }
  GIE = t_GIE;
}

void init_clock_2(void){
	
  
  DB_LED_1 = true;
	// The PLL needs this setup
	OSCCONbits.SCS = false;
	// internal with PLL to get 32MHz
	OSCCONbits.IRCF = 0xB;
	
	// PLL Enabled
	// OSCCONbits.SPLLEN = true;
	
	while(!OSCSTATbits.OSTS)
  {
     // just wait till it is locked
  }
	
  DB_LED_1 = false;
	
}

uint8_t clock_slowdown(void){
  
  static uint8_t speed = 0;
  speed++;
  if(speed > 16)
  {
    speed = 0;
  }
  OSCCONbits.SCS = 0x2u;
  DB_LED_1 = true;
  OSCCONbits.IRCF = speed;  //0x0C; // 31.25kHz   0x7;
  OSCCONbits.SPLLEN = false;
  
  // while(!OSCSTATbits.MFIOFR);
  
  DB_LED_1 = false;
  
  return speed;
  
}

#endif

#else



// use internal clock with ?MHz at the moment...
uint8_t fast_clock = true;



void set_clock_speed(uint8_t high_low_speed){
  
  bool t_GIE = GIE;
  // uint8_t t_GIE = GIE;
  
  GIE = false;
  
  if(high_low_speed == FAST_CLOCK_OSC)
  {
    init_clock();
    fast_clock = true;
  }
  else
  {
    clock_slowdown();
    fast_clock = false;
  }
  GIE = t_GIE;
}

#if MIPS == 1

void init_clock(void){
	
	
	// the source is determined by the config word 1
	OSCCONbits.SCS = false;
	
	// internal with PLL to get 32MHz
	OSCCONbits.IRCF = CLOCK_4_MHZ_HF_CLOCK;  // 0xD;
	
	// PLL Disabled
	OSCCONbits.SPLLEN = false;
	
	while(!OSCSTATbits.OSTS)
  {
    
  }
	
	
}

#elif MIPS == 2

void init_clock(void){
	
	
	// the source is determined by the config word 1
	OSCCONbits.SCS = false;
	
	// internal with PLL to get 32MHz
	OSCCONbits.IRCF = CLOCK_8_32_MHZ_HF_CLOCK;  // 0xE;
	
	// PLL Disabled
	OSCCONbits.SPLLEN = false;
	
	while(!OSCSTATbits.OSTS)
  {
    // just wait till it is locked
  }
	
	
}

#elif MIPS == 4

void init_clock(void){
	
	
	// the source is determined by the config word 1
	OSCCONbits.SCS = false;
	
	// internal with PLL to get 32MHz
	OSCCONbits.IRCF = CLOCK_16_MHZ_HF_CLOCK;  // 0xF;
	
	// PLL Disabled
	OSCCONbits.SPLLEN = false;
	
	while(!OSCSTATbits.OSTS)
  {
    // just wait till it is locked
  }	
	
}

#elif MIPS == 8

void init_clock(void){
	
	// The PLL needs this setup
	OSCCONbits.SCS = false;
  
	// internal with PLL to get 32MHz
	OSCCONbits.IRCF = CLOCK_8_32_MHZ_HF_CLOCK;  // 0xE;
	
	// PLL Enabled
	OSCCONbits.SPLLEN = true;
	
	while(!OSCSTATbits.OSTS)
  {
    // just wait till it is locked
  }	
	
}

#else
  
non standard velocity on clock
	
#endif

uint8_t clock_slowdown(void){
  
  static uint8_t speed = 0;
  speed++;
  if(speed > 16)
  {
    speed = 0;
  }
  OSCCONbits.SCS = 0x2u;
  // DB_LED_1 = true;
  OSCCONbits.IRCF = speed;  //0x0C; // 31.25kHz   0x7;
  OSCCONbits.SPLLEN = false;
  
  // while(!OSCSTATbits.MFIOFR);
  
  // DB_LED_1 = false;
  
  return speed;
  
}


void set_slow_clock(void){
  

  OSCCONbits.SCS = 0x00u;  // 0x2u;
  
  OSCCONbits.IRCF = CLOCK_3125KHZ_MF_CLOCK;  //0x0C; // 31.25kHz   0x7;
  
  OSCCONbits.SPLLEN = false;
  
  while(!OSCSTATbits.MFIOFR)
  {
    // just wait till it is locked
  }
  
  fast_clock = false;
  
  
  
}

#endif


















