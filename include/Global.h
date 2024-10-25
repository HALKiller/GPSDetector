#ifndef GLOBAL_H
#define GLOBAL_H


#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>



//  * * * * * * * * * * * * *  g l o b a l  a b r e v i a t i o n s   r e l a t e d     * * * * * * * * * * * * *  //
//  * * * * * * * * * * * * *  g l o b a l  a b r e v i a t i o n s   r e l a t e d     * * * * * * * * * * * * *  //

#define NULL_TERMINATOR	(char)'\0'

#define TRUE (1u)
#define FALSE (0u)

#define True (uint8_t)(1u)
#define False (uint8_t)(0u)



// #define DB_LED_PWM 0	// a special db case for testing...
#define DEBUGGING_IS_ON 1


// this define reduces the data_arrays to a lower 
// sizer so that the whoel project keeps on compiling
// and therefroe can get checked on errors of compilation
#define USE_REDUCED_RAM 0

// * * * * * * * * * * *  M I S R A   * * * * * * * * * * * * * * * * 
// when we set this one we use the coding style which removes MISRA Violations
// but might increase code size. Becaseu of that i use this one so that i know
// that the created MISRA warning got worked over and i am aware of the place 
// where it happens and that there is a MISRA compliant solution 
#define ENFORCE_MISRA_RULE 0



// if to use the device driver calls or directly the sfr calls...
#define USE_DEVICE_DRIVER 1




#define NOT_UNUSED_FUNCTIONS 0









//  * * * * * * * * * * * * *  B S P related    * * * * * * * * * * * * *  //
//  * * * * * * * * * * * * *  B S P related    * * * * * * * * * * * * *  //

// this is the version with two pcb
#define HW_VERSION_V10 0

// this is the version with only a single pcb
#define HW_VERSION_V20 0

// charging IMAN in this version --> no max so far...
#define GdL_V30 0

// that is the pcb with the MAX6969 Chip for the seg_7_d
#define GdL_V20_1 0 

#define HW_GPS_DETECTOR 1


#if HW_VERSION_V10+HW_VERSION_V20+GdL_V30+GdL_V20_1+HW_GPS_DETECTOR != 1
wat
#endif






// when there is the actual pcb to be used...
#define USE_REAL_PCB 1

#define NOT_COMPILED_FOR_BOOTLOADING 0


// thiswould give a colour reflecting the state of the batterie and not just red or green
#define IMPLEMENT_BATLED 0

#define FAST_PROGRAMMER 0

// this define is there so that i can
// read out or inject the parts which 
// i am developing at the moment
#define STILL_DEVELOPING 1

#define REDUCE_CONSUMPTION  0

// works with inverted logic that statement --> that is when set we swoff the display 
// that one is only getting used for products on the feria or over my lab table
#define DISPLAY_IS_NOT_ALWAYS_ON 1




#define ADC_BAT_DB_IS_ON 0  // To read out the ADC from the bat measurement

// Debugging defines 
#if DEBUGGING_IS_ON

#define LANGUAGE_SPANISH 0

#define DB_SWAP DB_LED=!DB_LED;

#define DB_LED2_SWAP DB_LED_2=!DB_LED_2
#define DB_LED1_SWAP DB_LED_1=!DB_LED_1
#define DB_LED_1_ON (DB_LED_1 = true)
#define DB_LED_1_OFF (DB_LED_1 = false)

#define DB_PRINT(str) send_string((const unsigned char *)(str))

#else
  
#define LANGUAGE_SPANISH 1
#define DB_SWAP
#define DB_LED1_SWAP

#define DB_LED_1_ON 
#define DB_LED_1_OFF

#define DB_LED2_SWAP
#define DB_PRINT(str)

#endif









//  * * * * * * * * * * * * *  O S C I L A T O R   r e l a t e d    * * * * * * * * * * * * *  //
//  * * * * * * * * * * * * *  O S C I L A T O R   r e l a t e d    * * * * * * * * * * * * *  //

#define MIPS 8

#define LOW_CLOCK_FREQ 31250

#define EXTERNAL_CLOCK	0


#ifndef MIPS 
error--> we always need MIPS
#else
	
#if MIPS == 1

#define _XTAL_FREQ	4000000

#elif MIPS==2

#define _XTAL_FREQ	8000000

#elif MIPS==4

#define _XTAL_FREQ	16000000

#elif MIPS==8

#define _XTAL_FREQ	32000000

#else	
	
error again --> that MIPS is not standard so far --> write it extra out

#endif

#endif


#define US_PER_SECOND 1000000


#if MIPS==8

#define TMR2_OF_125US 0
#define TMR2_OF_200US 1
#define TMR2_OF_250US 0	
	
	
#define TMR2_OF_CNT_POR_SECOND 	5000
#define TICKS_POR_MINUTE 300000	// that is	TMR2_OF_CNT_POR_SECOND x 60 

#else
	// FAIL compilation
#endif




#define CHARS_TO_RECEIVE	35u	// 34 config bytes + 1 chcksum







// * * * * * * * * * * * * *   I n c l i n a t i o n   S e n s o r   r e l a t e d     * * * * * * * * * * * * *  //
// * * * * * * * * * * * * *   I n c l i n a t i o n   S e n s o r   r e l a t e d     * * * * * * * * * * * * *  //

// these are the time s in seconds that the sensor has to have 
// a stable reading to change the state initial state is off!! but still without having set to sleep
#define TIME_THRESHOLD_FOR_DETECTOR_IS_ON ((uint16_t)2u)
#define TIME_THRESHOLD_FOR_DETECTOR_IS_OFF ((uint16_t)2u)
#define SENSOR_IS_TOP_MOUNTED ((uint8_t)1u) // because the signal is invertred depending on the sid of mounting







#endif	// GLOBAL_H