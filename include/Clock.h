#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>




#define FAST_CLOCK_OSC 1
#define SLOW_CLOCK_OSC 0


void set_clock_speed(uint8_t high_low_speed);

void init_clock(void);

void init_clock_2(void);

void set_slow_clock(void);
  
uint8_t clock_slowdown(void);

#define CLOCK_3125KHZ_MF_CLOCK 0x02u
#define CLOCK_3125KHZ_HF_CLOCK 0x03u
#define CLOCK_6250KHZ_MF_CLOCK 0x04u
#define CLOCK_500KHZ_MF_CLOCK 0x07u


#define CLOCK_4_MHZ_HF_CLOCK 0x0Du
#define CLOCK_8_32_MHZ_HF_CLOCK 0x0Eu
#define CLOCK_16_MHZ_HF_CLOCK 0x0Fu
























#endif	// CLOCK_H