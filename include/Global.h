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

// To free TMR6 completely we can use actually
// TMR2 for the TILT Sensor Timing
#define USE_TMR2_AS_TILT_SENS_TMR 1

// #define USE_SHORT_FILE_NAMES 1


#define NULL_TERMINATOR	(char)'\0'

#define TRUE (1u)
#define FALSE (0u)

#define True (uint8_t)(1u)
#define False (uint8_t)(0u)



//  * * * * * * * * * * * * *  D E V E L O P I N G  r e l a t e d     * * * * * * * * * * * * *  //
//  * * * * * * * * * * * * *  D E V E L O P I N G  r e l a t e d     * * * * * * * * * * * * *  //
#define OV_PWM_LUZ 1

// This define activates the overworking of the GPSModule for 
// 115200 Baud setting// furthermore do we need to increase the MCU SPeed
// so that we can get the 115kBaud into the system
#define USE_115K_BAUD 1

// to find the root of the problem of the GLONASS --> adding it for zthe time being but as off
#define GLONASS_BUG 1

//  * * * * * * * * * * * * *  D E B U G G I N G   A N D   V E R S I O N  r e l a t e d     * * * * * * * * * * * * *  //
//  * * * * * * * * * * * * *  D E B U G G I N G   A N D   V E R S I O N  r e l a t e d     * * * * * * * * * * * * *  //

// when set we have a release version
#define COMPILE_FOR_RELEASE 0

// if COMPILE_FOR_RELEASE == false we can compile for test
#define COMPILE_FOR_TEST 1

// or compile for internal test --> that is only for me
#define COMPILE_FOR_INTERNAL_TEST 0

#define COMPILE_FOR_DEBUG 0

#if COMPILE_FOR_RELEASE+COMPILE_FOR_TEST+COMPILE_FOR_INTERNAL_TEST+COMPILE_FOR_DEBUG != 1
err
#endif

// depending on that define we compile different pcb versions
#define PCB_VERSION 69


// The actual Version as string...
#define FW_VERSION_STR "31"


// to get better adc readings...
#define USE_ADC_OVERSAMPLING 1

// testing setups to reduce the consumption when
// sleep before  TX/Search mode
// #define DB_L_POWER 1

// calculating the Vbat value based on
 // measuring first a reference voltage
#define VREF_METHOD 1

// when the TILT Sensor cant get read corretly we indicate that error with the LED
#define INDICATE_TILT_SENSOR_ERROR 1

// because than it is possible to reflect that into the message
// if the DDS got reconfigured because the readback went bad...
// that has to be like that otherwise we would not be able to send the DDS reconfigure information
// in the message
#define CREATE_TX_MESSAGE_AFTER_DDS_CFG 1

// this one seems allready unused...
#define TRY_HEX_IN_VERSION_DIGITS 1

// this one seems allready unused...
#define USE_ERR_MSG_IN_GOOD_POS 0


// with this define we execute only a singel function over and over again 
// --> to be able to implement and test a certain part easily
#define COMPILE_SINGLE_FUNCTION_FOR_TESTING 0

#define REDUCE_ALL_ROM_USAGE 0


// because there seem to be problems sometimes when the first lock is into a GLONASS,
// there is a time offset 
#define DISABLE_GLONASS 1
#define DISABLE_NOT_GLONASS 0

// this define creates a version were we do not swoff the GPS --> 
// we keep it on until the tx moment has come -->
#define RUN_GPS_TILL_TX 1


// ---------------------------------------------------------  COMPILE FOR RELEASE  -------------------------  //
// ---------------------------------------------------------  COMPILE FOR RELEASE  -------------------------  //
#if COMPILE_FOR_RELEASE


#define DEBUGGING_IS_ON 0
#define DEBUGGING_BB_IS_ON 0  // Bit Banged UART
#define G_ENABLE_ASSERT 0     // to reduce ROM --> set to 0 no asserts
#define COMPILE_WITH_RX_LUZ 1 // 656 bytes --> 8%
#define COMPILE_WITH_PWM_LUZ 1
#define USE_BB_UART_AS_GPS_INPUT 0
#define DO_TRANSMIT_RF 1  // when reset(0) we do not transmit over radio

#define LETTER_REPLACER "R"


// to avoid to have to wait for a GPS_position we emulate some fake position and time
#define EMULATE_GPS_TIME_POSITION 0
//************************
// when set the LED really iluminates, otherwise we skpip one instruction
#define USE_PWM_LED 1

//********************
// so that the tilt sensor is not 
// activating/deactivating the detector but is always on
#define TILT_IS_ALWAYS_ON 0

// when set we read the cfg back from the DDS to assure the cfg got 
// transferred correctly. if not we retry 
#define READBACK_DDS 1

#define DB_LUZ 0

#define SEND_ONLY_ADC_VALUE 0
#define TEST_STATUS_LEDS 0
#define DB_V69_PCB 0
#define SEND_LOCK_TIME_DECIMAS_LATITUDE 0

// all DB_FILE ANBLED are swoffed
#define SWOFF_ALL_DB_FILE_ENABLED 1

// ---------------------------------------------------------  COMPILE FOR TEST  -------------------------  //
// ---------------------------------------------------------  COMPILE FOR TEST  -------------------------  //
#elif COMPILE_FOR_TEST


#define DEBUGGING_IS_ON 0
#define DEBUGGING_BB_IS_ON 1  // Bit Banged UART
#define G_ENABLE_ASSERT 0     // to reduce ROM --> set to 0 no asserts
#define COMPILE_WITH_RX_LUZ 1
#define COMPILE_WITH_PWM_LUZ 1
#define USE_BB_UART_AS_GPS_INPUT 0
#define DO_TRANSMIT_RF 1  // when reset(0) we do not transmit over radio

#define LETTER_REPLACER "T"

// to avoid to have to wait for a GPS_position we emulate some fake position and time
#define EMULATE_GPS_TIME_POSITION 0

// when set the LED really iluminates, otherwise we skpip one instruction
#define USE_PWM_LED 1


// so that the tilt sensor is not 
// activating/deactivating the detector but is always on
#define TILT_IS_ALWAYS_ON 0

// we are using the readback functionality of the DDS to check its correct confuration
#define READBACK_DDS 1
#define DB_LUZ 0



#define SEND_ONLY_ADC_VALUE 0
#define TEST_STATUS_LEDS 0
#define DB_V69_PCB 0
#define SEND_LOCK_TIME_DECIMAS_LATITUDE 0

// all DB_FILE ANBLED are swoffed
#define SWOFF_ALL_DB_FILE_ENABLED 0


// ---------------------------------------------------------  COMPILE FOR INTERNAL TEST  -------------------------  //
#elif COMPILE_FOR_INTERNAL_TEST


#define DEBUGGING_IS_ON 0
#define DEBUGGING_BB_IS_ON 0  // Bit Banged UART
#define G_ENABLE_ASSERT 0     // to reduce ROM --> set to 0 no asserts
#define COMPILE_WITH_RX_LUZ 1
#define COMPILE_WITH_PWM_LUZ 1
#define USE_BB_UART_AS_GPS_INPUT 1
#define DO_TRANSMIT_RF 1  // when reset(0) we do not transmit over radio

#define LETTER_REPLACER "I"

// to avoid to have to wait for a GPS_position we emulate some fake position and time
#define EMULATE_GPS_TIME_POSITION 0


//************************
// when set the LED really iluminates, otherwise we skpip one instruction
#define USE_PWM_LED 1

//********************
// so that the tilt sensor is not 
// activating/deactivating the detector but is always on
#define TILT_IS_ALWAYS_ON 1

#define READBACK_DDS 1
#define DB_LUZ 0



#define SEND_ONLY_ADC_VALUE 0
#define TEST_STATUS_LEDS 0
#define DB_V69_PCB 0
#define SEND_LOCK_TIME_DECIMAS_LATITUDE 0

// all DB_FILE ANBLED are swoffed
#define SWOFF_ALL_DB_FILE_ENABLED 0


// ---------------------------------------------------------  COMPILE FOR DEBUG  -------------------------  //
// ---------------------------------------------------------  COMPILE FOR DEBUG  -------------------------  //
#elif COMPILE_FOR_DEBUG // COMPILE_FOR_DEBUG --> the DB PART here


#define DEBUGGING_IS_ON 0
#define DEBUGGING_BB_IS_ON 1  // Bit Banged UART
#define G_ENABLE_ASSERT 0     // to reduce ROM --> set to 0 no asserts
#define COMPILE_WITH_RX_LUZ 0
#define COMPILE_WITH_PWM_LUZ 0
#define USE_BB_UART_AS_GPS_INPUT 0
#define DO_TRANSMIT_RF 1  // when reset(0) we do not transmit over radio

#define LETTER_REPLACER "D"

// to avoid to have to wait for a GPS_position we emulate some fake position and time
#define EMULATE_GPS_TIME_POSITION 0


//************************
// when set the LED really iluminates, otherwise we skpip one instruction
#define USE_PWM_LED 1

//********************
// so that the tilt sensor is not 
// activating/deactivating the detector but is always on
#define TILT_IS_ALWAYS_ON 1

#define READBACK_DDS 1
#define DB_LUZ 0



// when set we send the amount of recharged times baterie on activation...

#define SEND_ONLY_ADC_VALUE 0
#define TEST_STATUS_LEDS 1
#define DB_V69_PCB 0
#define SEND_LOCK_TIME_DECIMAS_LATITUDE 0 // sending the lock time in the decima slot...

// all DB_FILE_ENABLED are swoffed with a single cmd
#define SWOFF_ALL_DB_FILE_ENABLED 0

#else
  
// none --> thorw err!

#endif  // COMPILE_FOR_RELEASE

// ----------------------------------------- END OF RTID COMPILE  -------------------------  //
// ----------------------------------------- END OF RTID COMPILE  -------------------------  //



// when set we replace the seconds in the messages with an err code 
// --> therefore we can add several states into that

#define SEND_ERROR_CODES_IN_SECONDS_SLOT 0


//****************************
// when set we use a calculation to get the values for the registers
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



// this define activates the position cnt validation-->
// that could get avoided with tsetting it in case to one and accepting the first one a svalid
#define USE_POSITION_CNT_VALIDATION 1
// this define sets the amount of valid necessary positions before we accept 
// the last position as the position to transmit
#define POSITION_CNT_BEFORE_VALID 10u



//  * * * * * * * * * * * * *  B S P related    * * * * * * * * * * * * *  //
//  * * * * * * * * * * * * *  B S P related    * * * * * * * * * * * * *  //

// this define elinates all the independent LATAA,LATB;LATC setting with a macro
#define USE_SINGLE_LPM_SETTER 1

#define HW_GPS_DETECTOR 1


#if HW_VERSION_V10+HW_VERSION_V20+GdL_V30+GdL_V20_1+HW_GPS_DETECTOR != 1
wat
#endif



// when there is the actual pcb to be used...
#define USE_REAL_PCB 1

// When cleared we have a valid cfg allready from the bootloader...
#define NOT_COMPILED_FOR_BOOTLOADING 1



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

  #define G_UART_INT(var) UART_INT_C(var)
  
  #if DEBUGGING_BB_IS_ON

    #define G_DB_PRINT(str) send_bb_string((const unsigned char *)(str))

  #else
      
    #define G_DB_PRINT(str) send_string((const unsigned char *)(str))

  #endif

  #define RUN_ERTC_TEST 1

  #define RUN_TMR0_TEST_SLOW_CLCK 1



#else
  

  #if DEBUGGING_BB_IS_ON

    #define DB_LED_1_ON (DB_LED_1 = true)
    #define DB_LED_1_OFF (DB_LED_1 = false)

    #define DB_LED1_SWAP DB_LED_1=!DB_LED_1
    #define DB_LED2_SWAP DB_LED_2=!DB_LED_2
    #define G_DB_PRINT(str) send_bb_string((const unsigned char *)(str))
    #define G_UART_INT(var) UART_INT_C(var)
    
  #else
    
    #define G_DB_PRINT(str)
    #define DB_LED1_SWAP
    #define G_UART_INT(var)


    #define DB_SWAP
    #define DB_LED_1_ON
    #define DB_LED_1_OFF
    #define DB_LED_2_ON
    #define DB_LED_2_OFF
    #define DB_LED2_SWAP
    #define DB_LED3_SWAP
  
 #endif

#endif


#if SWOFF_ALL_DB_FILE_ENABLED

#define FILE_MAIN_DB_ENABLED 0
#define FILE_INIT_ALL_DB_ENABLED 0
#define FILE_UART_DB_ENABLED 0  // the UART_int is not working otherwise of course...
#define FILE_CLOCK_DB_ENABLED 0
#define FILE_CONFIGURATION_BITS_DB_ENABLED 0
#define FILE_INTERRUPT_ISR_FILE_DB_ENABLED 0
#define FILE_ADC_DB_ENABLED 0
#define FILE_HANDLERS_DB_ENABLED 0
#define FILE_RING_BUFFER_DB_ENABLED 0
#define FILE_RX_LUZ_DB_ENABLED 0
#define FILE_MY_ASSERT_DB_ENABLED 0
#define FILE_TILT_SENSOR_DB_ENABLED 0
#define FILE_TIMERS_DB_ENABLED 0
#define FILE_GD_STATES_DB_ENABLED 0
#define FILE_GPS_DB_ENABLED 0
#define FILE_EXTENSION_STRINGS_DB_ENABLED 0
#define FILE_E_RTC_DB_ENABLED 0
#define FILE_EEPROM_DB_ENABLED 0
#define FILE_DDS_DB_ENABLED 0
#define FILE_MESSAGES_DB_ENABLED 0
#define FILE_BIT_BANGED_UART_DB_ENABLED 0
#define FILE_DETECTOR_DB_ENABLED 0

#else

#define               FILE_MAIN_DB_ENABLED 0
#define           FILE_INIT_ALL_DB_ENABLED 0
#define               FILE_UART_DB_ENABLED 1  // the UART_int is not working otherwise of course...
#define              FILE_CLOCK_DB_ENABLED 0
#define FILE_CONFIGURATION_BITS_DB_ENABLED 0
#define FILE_INTERRUPT_ISR_FILE_DB_ENABLED 0
#define                FILE_ADC_DB_ENABLED 0
#define           FILE_HANDLERS_DB_ENABLED 0
#define        FILE_RING_BUFFER_DB_ENABLED 0
#define             FILE_RX_LUZ_DB_ENABLED 0
#define          FILE_MY_ASSERT_DB_ENABLED 0
#define        FILE_TILT_SENSOR_DB_ENABLED 0
#define             FILE_TIMERS_DB_ENABLED 0
#define          FILE_GD_STATES_DB_ENABLED 0
#define                FILE_GPS_DB_ENABLED 1
#define  FILE_EXTENSION_STRINGS_DB_ENABLED 0
#define              FILE_E_RTC_DB_ENABLED 0
#define             FILE_EEPROM_DB_ENABLED 0
#define                FILE_DDS_DB_ENABLED 0
#define           FILE_MESSAGES_DB_ENABLED 0
#define    FILE_BIT_BANGED_UART_DB_ENABLED 0
#define           FILE_DETECTOR_DB_ENABLED 0

#endif

#if 0
#define DB_REPLACER_ON \
#define DB_PRINT(str) G_DB_PRINT(str)\
#define UART_int(var) UART_INT_C(var)

#define DB_REPLACER_OFF\
#define DB_PRINT(str)\
#define UART_int(var)
#endif


//  * * * * * * * * * * * * *  O S C I L A T O R   r e l a t e d    * * * * * * * * * * * * *  //
//  * * * * * * * * * * * * *  O S C I L A T O R   r e l a t e d    * * * * * * * * * * * * *  //

#if COMPILE_FOR_DEBUG&&0
#define MIPS 8u //1u
#else
#define MIPS 8u //1u
#endif

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
#define BATERIE_LEVEL 0xCF


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
#define MINIMUM_GPS_ON_BEFORE_TRANSMISSION 30u


#define BATCNT_L_ADDRESS 0x00
#define BATCNT_H_ADDRESS 0x01
#define BATCNT_FLG_ADDRESS   0x02

#define BATCNT_HIGH_VOLTAGE (uint8_t)120
#define BATCNT_LOW_VOLTAGE (uint8_t)115u

// * * * * * * * * * * * * *   I n c l i n a t i o n   S e n s o r   r e l a t e d     * * * * * * * * * * * * *  //
// * * * * * * * * * * * * *   I n c l i n a t i o n   S e n s o r   r e l a t e d     * * * * * * * * * * * * *  //

// these are the time s in seconds that the sensor has to have 
// a stable reading to change the state initial state is off!! but still without having set to sleep
#define TIME_BASE 10u  // x100 --> 1200ms
#define SENSOR_TIME_BETWEEN_READING 2u // x100 in ms an multiple of 2
#define RND_CNT_TILT (uint8_t)(SENSOR_TIME_BETWEEN_READING*TIME_BASE_200_CNT/TIME_BASE)
#define TIME_THRESHOLD_FOR_DETECTOR_IS_ON ((uint16_t)2u)
#define TIME_THRESHOLD_FOR_DETECTOR_IS_OFF ((uint16_t)6u)
// #define SENSOR_IS_TOP_MOUNTED ((uint8_t)1u) // because the signal is invertred depending on the sid of mounting


#if OV_PWM_LUZ
#define TIME_BASE_200_CNT 1u  // becaseu for one second we need 5x200ms
#else
#define TIME_BASE_200_CNT 5u  // becaseu for one second we need 5x200ms
#endif




#endif	// GLOBAL_H