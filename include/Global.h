#ifndef GLOBAL_H
#define GLOBAL_H


#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>


#include "bit_banged_uart.h"

//  * * * * * * * * * * * * *  g l o b a l  a b r e v i a t i o n s   r e l a t e d     * * * * * * * * * * * * *  //
//  * * * * * * * * * * * * *  g l o b a l  a b r e v i a t i o n s   r e l a t e d     * * * * * * * * * * * * *  //

#define NULL_TERMINATOR	(char)'\0'

#define TRUE (1u)
#define FALSE (0u)

#define True (uint8_t)(1u)
#define False (uint8_t)(0u)


//  * * * * * * * * * * * * *  D E B U G G I N G   r e l a t e d     * * * * * * * * * * * * *  //
//  * * * * * * * * * * * * *  D E B U G G I N G   r e l a t e d     * * * * * * * * * * * * *  //
#define COMPILE_FOR_RELEASE 0

#define PCB_VERSION 68  // 

#define USE_OLD_CFG_SETTER 0

// debugging an incoherence in the clock switching -->
// that should get eliminated n the long run
#define DB_CLOCKSWITCH 1

// testing setups to reduce the consumption when
// sleep before  TX/Search mode
#define DB_L_POWER 1


// Overworking the SPI communication with the AD9954...
#define USE_NEW_SPI 1
#define DB_NEW_SPI 1



#if COMPILE_FOR_RELEASE


#define DEBUGGING_IS_ON 0
#define DEBUGGING_BB_IS_ON 0  // Bit Banged UART
#define G_ENABLE_ASSERT 0     // to reduce ROM --> set to 0 no asserts
#define COMPILE_WITH_RX_LUZ 1
#define COMPILE_WITH_PWM_LUZ 1

#define DO_TRANSMIT_RF 1  // when reset(0) we do not transmit over radio
#define DB_67 0

//************************
// when set the LED really iluminates, otherwise we skpip one instruction
#define USE_PWM_LED 1

//********************
// so that the tilt sensor is not 
// activating/deactivating the detector but is always on
#define TILT_IS_ALWAYS_ON 0

#define READBACK_DDS 0
#define DB_LUZ 0


#else // COMPILE_FOR_RELEASE


#define DEBUGGING_IS_ON 0
#define DEBUGGING_BB_IS_ON 1  // Bit Banged UART
#define G_ENABLE_ASSERT 0     // to reduce ROM --> set to 0 no asserts
#define COMPILE_WITH_RX_LUZ 0
#define COMPILE_WITH_PWM_LUZ 1

#define DO_TRANSMIT_RF 1  // when reset(0) we do not transmit over radio
#define DB_67 0

//************************
// when set the LED really iluminates, otherwise we skpip one instruction
#define USE_PWM_LED 1

//********************
// so that the tilt sensor is not 
// activating/deactivating the detector but is always on
#define TILT_IS_ALWAYS_ON 1

#define READBACK_DDS 1
#define DB_LUZ 0


// in DEBUGGING we are getting low on TOM and therefore I just write a 
// Version number and do not calculate it
#define REDUCE_ROM_ON_VERSION_CREATION 1



#endif  // COMPILE_FOR_RELEASE


// when set we replace the seconds in the messages with an err code --> therefore 

#define SEND_ERROR_CODES_IN_SECONDS_SLOT 1






//****************************
// when set we use a calculation to get thevalues for the registers
// otherwise we just plug in precalculated values --> ROM friendly
#define CALCULATE_BAUDRATE 0



#define TEST_ERTC_SLOW_CLOCK 1

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
#define USE_DEVICE_DRIVER 0


#define NOT_UNUSED_FUNCTIONS 0


#define SEND_ALL_MESSAGES_FOR_TESTING 0






//  * * * * * * * * * * * * *  B S P related    * * * * * * * * * * * * *  //
//  * * * * * * * * * * * * *  B S P related    * * * * * * * * * * * * *  //



#if PCB_VERSION==68
#define INVERTED_LDR_SENSOR 1
#define USE_SPI_TILT 1
#elif PCB_VERSION==67
#define INVERTED_LDR_SENSOR 0
#define USE_SPI_TILT 0
#else
wat?  
#endif


#define HW_GPS_DETECTOR 1


#if HW_VERSION_V10+HW_VERSION_V20+GdL_V30+GdL_V20_1+HW_GPS_DETECTOR != 1
wat
#endif



// when there is the actual pcb to be used...
#define USE_REAL_PCB 1

#define NOT_COMPILED_FOR_BOOTLOADING 0



#if 0
// thiswould give a colour reflecting the state of the batterie and not just red or green
#define IMPLEMENT_BATLED 0

#define FAST_PROGRAMMER 0

// this define is there so that i can
// read out or inject the parts which 
// i am developing at the moment
#define STILL_DEVELOPING 1

#define REDUCE_CONSUMPTION  0

#endif









extern const uint16_t shifts[16];












//  * * * * * * * * * * * * *  D E B U G G I N G related    * * * * * * * * * * * * *  //
//  * * * * * * * * * * * * *  D E B U G G I N G related    * * * * * * * * * * * * *  //
#if DEBUGGING_IS_ON



#define DB_SWAP DB_LED=!DB_LED;

#define DB_LED3_SWAP DB_LED_3=!DB_LED_3
#define DB_LED2_SWAP DB_LED_2=!DB_LED_2
#define DB_LED1_SWAP DB_LED_1=!DB_LED_1

#define DB_LED_1_ON (DB_LED_1 = true)
#define DB_LED_1_OFF (DB_LED_1 = false)

#define DB_LED_2_ON (DB_LED_2 = true)
#define DB_LED_2_OFF (DB_LED_2 = false)

#if DEBUGGING_BB_IS_ON

#define DB_PRINT(str) send_bb_string((const unsigned char *)(str))
#else
#define DB_PRINT(str) send_string((const unsigned char *)(str))
#endif

#define RUN_ERTC_TEST 1

#define RUN_TMR0_TEST_SLOW_CLCK 1


#define FAKE_GPS_VALIM DB_LED_3




#else
  

#if DEBUGGING_BB_IS_ON

#define DB_PRINT(str) send_bb_string((const unsigned char *)(str))
#define DB_LED1_SWAP DB_LED_1=!DB_LED_1
#define DB_LED2_SWAP DB_LED_2=!DB_LED_2
#else
  
#define DB_PRINT(str)
#define DB_LED1_SWAP

#endif


#define DB_SWAP


#define DB_LED_1_ON
#define DB_LED_1_OFF

#define DB_LED_2_ON
#define DB_LED_2_OFF

#define DB_LED2_SWAP

#define DB_LED3_SWAP


#endif




// #define SWOFF_GIE bool temp_GIE = GLOBAL_IE;GIE = false;

// #define SWON_GIE GIE = temp_GIE;




//  * * * * * * * * * * * * *  O S C I L A T O R   r e l a t e d    * * * * * * * * * * * * *  //
//  * * * * * * * * * * * * *  O S C I L A T O R   r e l a t e d    * * * * * * * * * * * * *  //

#define MIPS 1u

#define SLOW_CLCK LOW_500KHZ

#define LOW_CLOCK_FREQ 31250

#define LOW_31_25KHZ 3125
#define LOW_500KHZ 500

// 3125KHZ


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



// * * * * * * * * * * * * *   D E T E C T O R C O N F I G   r e l a t e d     * * * * * * * * * * * * *  //
// * * * * * * * * * * * * *   D E T E C T O R C O N F I G   r e l a t e d     * * * * * * * * * * * * *  //

#define EE_UNUSED 0xFF

#define BPS_150 0x92  // 0x92 = transmission continuo
#define BPS_300 0x80


#define EEPROM_TX_DURATION 0x0A
#define TIEMPO_SINCRONISMO 0x01
#define PWM_PORCENTAGE 0x01
#define PWM_LUZ_ON_OFF 0x1A // high nibble = on_time, low_nibble = off_time
#define EEPROM_BPS_CONFIG BPS_150
#define EEPROM_MAX_DETECORES 0x06

// how often we use the copy of position 
#define MAXIMUM_RESENT_SAME_POSITION 1


// to avoid a sync time which might be allready after the tx time --> 
// therefroe calcualt ea time shorter than necessary and in case
// give that time before transmission
#define GPS_OFF_BEFORE_TX 1
#define GPS_OFF_TIME_SAFE_SYNC 3u // these are three seconds safe time
#define SLEEP_BEFORE_TX_SWAP_BACK_TIME 3u

// becaseu we want an hysteresis for the Vbat low signal we use a 500mV  
// --> but the whole thing is basd on 1/100 therefore we delta = 5
#define VBAT_DELTA 5u

// that means for example 2 --> locktime = 60 seconds --> 60*(10+2)/10 = 72;
#define GPS_LOCK_TIME_DECIMO_PERCENTAGER 5u  // == 50%, 1 = 10, 2 = 20 ...10 = 100;

// to avoi dthat the gps on is to short at some moment
#define MINIMUM_GPS_ON_BEFORE_TRANSMISSION 50u




// * * * * * * * * * * * * *   I n c l i n a t i o n   S e n s o r   r e l a t e d     * * * * * * * * * * * * *  //
// * * * * * * * * * * * * *   I n c l i n a t i o n   S e n s o r   r e l a t e d     * * * * * * * * * * * * *  //

// these are the time s in seconds that the sensor has to have 
// a stable reading to change the state initial state is off!! but still without having set to sleep
#define TIME_THRESHOLD_FOR_DETECTOR_IS_ON ((uint16_t)2u)
#define TIME_THRESHOLD_FOR_DETECTOR_IS_OFF ((uint16_t)4u)
#define SENSOR_IS_TOP_MOUNTED ((uint8_t)1u) // because the signal is invertred depending on the sid of mounting


#define TIME_BASE_200_CNT 5u  // becaseu for one second we need 5x200ms




#endif	// GLOBAL_H