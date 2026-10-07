#ifndef TIMERS_H
#define TIMERS_H

#include "Global.h"

#if 1
// --------   T M R  0  ---------------
#define TMR0_IE INTCONbits.TMR0IE
#define TMR0_IF INTCONbits.TMR0IF

#define TMR0_002_PRESCALER	0u
#define TMR0_004_PRESCALER	1u
#define TMR0_008_PRESCALER	2u
#define TMR0_016_PRESCALER	3u
#define TMR0_032_PRESCALER	4u
#define TMR0_064_PRESCALER	5u
#define TMR0_128_PRESCALER	6u
#define TMR0_256_PRESCALER	7u


// --------   T M R  1  ---------------
#define TMR1_ON T1CONbits.TMR1ON
#define T1_PRESCALER	T1CONbits.T1CKPS
#define TMR1_IE PIE1bits.TMR1IE
#define TMR1_IF PIR1bits.TMR1IF

#define TMR1_1_PRESCALER	0u
#define TMR1_2_PRESCALER	1u
#define TMR1_4_PRESCALER	2u
#define TMR1_8_PRESCALER	3u


#define USE_TMR1_FLG 1

#if MIPS == 8

#define TMR1_MIPS_PSA TMR1_8_PRESCALER

#elif MIPS == 4

#define TMR1_MIPS_PSA TMR1_4_PRESCALER

#elif MIPS == 2

#define TMR1_MIPS_PSA TMR1_2_PRESCALER

#elif MIPS == 1

#define TMR1_MIPS_PSA TMR1_1_PRESCALER

#else
  
wat 

#endif

// --------   T M R  2 4 6  ---------------
#define TMR246_01_PRESCALER	0u
#define TMR246_04_PRESCALER	1u
#define TMR246_16_PRESCALER	2u
#define TMR246_64_PRESCALER	3u

#define TMR246_01_POSTSCALER	0u
#define TMR246_02_POSTSCALER	1u
#define TMR246_03_POSTSCALER	2u
#define TMR246_04_POSTSCALER	3u
#define TMR246_05_POSTSCALER	4u
#define TMR246_06_POSTSCALER	5u
#define TMR246_07_POSTSCALER	6u
#define TMR246_08_POSTSCALER	7u
#define TMR246_09_POSTSCALER	8u
#define TMR246_10_POSTSCALER	9u
#define TMR246_11_POSTSCALER	10u
#define TMR246_12_POSTSCALER	11u
#define TMR246_13_POSTSCALER	12u
#define TMR246_14_POSTSCALER	13u
#define TMR246_15_POSTSCALER	14u
#define TMR246_16_POSTSCALER	15u



// --------   T M R  2  ---------------
#define TMR2_ON T2CONbits.TMR2ON
#define TMR2_PRESCALER	T2CONbits.T2CKPS
#define TMR2_POSTSCALER	T2CONbits.T2OUTPS

#define TMR2_IE PIE1bits.TMR2IE
#define TMR2_IF PIR1bits.TMR2IF

#define TMR2_01_PRESCALER	0u
#define TMR2_04_PRESCALER	1u
#define TMR2_16_PRESCALER	2u
#define TMR2_64_PRESCALER	3u

#define TMR2_01_POSTSCALER	0u
#define TMR2_02_POSTSCALER	1u
#define TMR2_03_POSTSCALER	2u
#define TMR2_04_POSTSCALER	3u
#define TMR2_05_POSTSCALER	4u
#define TMR2_06_POSTSCALER	5u
#define TMR2_07_POSTSCALER	6u
#define TMR2_08_POSTSCALER	7u
#define TMR2_09_POSTSCALER	8u
#define TMR2_10_POSTSCALER	9u
#define TMR2_11_POSTSCALER	10u
#define TMR2_12_POSTSCALER	11u
#define TMR2_13_POSTSCALER	12u
#define TMR2_14_POSTSCALER	13u
#define TMR2_15_POSTSCALER	14u
#define TMR2_16_POSTSCALER	15u



#if MIPS==8

#define TMR2_300BAUD_PRE   TMR2_16_PRESCALER;
#define TMR2_300BAUD_POST  TMR2_07_POSTSCALER;
#define TMR2_300BAUD_PR    (uint8_t)238;	


#define TMR2_150BAUD_PRE   TMR2_16_PRESCALER;
#define TMR2_150BAUD_POST  TMR2_14_POSTSCALER;
#define TMR2_150BAUD_PR    (uint8_t)238;	

#define TMR2_DDS_CFG_PR    (uint8_t)160; // (uint8_t)80;	

// the rx_luz tmr2 cfg
#define TMR2_15MS_PRE     TMR2_64_PRESCALER
#define TMR2_15MS_POST    TMR2_15_POSTSCALER
#define TMR2_15MS_PR      125u

#define TMR2_1MS_PRE      TMR2_64_PRESCALER
#define TMR2_1MS_POST     TMR2_01_POSTSCALER
#define TMR2_1MS_PR       125u

#define TMR2_500US_PRE    TMR2_16_PRESCALER
#define TMR2_500US_POST   TMR2_01_POSTSCALER
#define TMR2_500US_PR     250u

#define TMR2_3072US_PRE   TMR2_64_PRESCALER
#define TMR2_3072US_POST  TMR2_04_POSTSCALER
#define TMR2_3072US_PR    96u


#elif MIPS==4

#define TMR2_300BAUD_PRE   TMR2_04_PRESCALER;
#define TMR2_300BAUD_POST  TMR2_14_POSTSCALER;
#define TMR2_300BAUD_PR    (uint8_t)238;	


#define TMR2_150BAUD_PRE   TMR2_16_PRESCALER;
#define TMR2_150BAUD_POST  TMR2_07_POSTSCALER;
#define TMR2_150BAUD_PR    (uint8_t)238;	

#define TMR2_DDS_CFG_PR    (uint8_t)80; // (uint8_t)40;	

// the rx_luz tmr2 cfg
#define TMR2_15MS_PRE     TMR2_16_PRESCALER
#define TMR2_15MS_POST    TMR2_15_POSTSCALER
#define TMR2_15MS_PR      250u

#define TMR2_1MS_PRE      TMR2_16_PRESCALER
#define TMR2_1MS_POST     TMR2_01_POSTSCALER
#define TMR2_1MS_PR       250u

#define TMR2_500US_PRE    TMR2_16_PRESCALER
#define TMR2_500US_POST   TMR2_01_POSTSCALER
#define TMR2_500US_PR     125u

#define TMR2_3072US_PRE   TMR2_16_PRESCALER
#define TMR2_3072US_POST  TMR2_08_POSTSCALER
#define TMR2_3072US_PR    96u




#elif MIPS==2

#define TMR2_300BAUD_PRE   TMR2_04_PRESCALER;
#define TMR2_300BAUD_POST  TMR2_07_POSTSCALER;
#define TMR2_300BAUD_PR    (uint8_t)238;	


#define TMR2_150BAUD_PRE   TMR2_04_PRESCALER;
#define TMR2_150BAUD_POST  TMR2_14_POSTSCALER;
#define TMR2_150BAUD_PR    (uint8_t)238;		

#define TMR2_DDS_CFG_PR    (uint8_t)40; // (uint8_t)20;	

// the rx_luz tmr2 cfg
#define TMR2_15MS_PRE     TMR2_16_PRESCALER
#define TMR2_15MS_POST    TMR2_15_POSTSCALER
#define TMR2_15MS_PR      125u

#define TMR2_1MS_PRE      TMR2_16_PRESCALER
#define TMR2_1MS_POST     TMR2_01_POSTSCALER
#define TMR2_1MS_PR       125u

#define TMR2_500US_PRE    TMR2_04_PRESCALER
#define TMR2_500US_POST   TMR2_01_POSTSCALER
#define TMR2_500US_PR     250u

#define TMR2_3072US_PRE   TMR2_16_PRESCALER
#define TMR2_3072US_POST  TMR2_04_POSTSCALER
#define TMR2_3072US_PR    96u



#elif MIPS==1

#define TMR2_300BAUD_PRE   TMR2_01_PRESCALER;
#define TMR2_300BAUD_POST  TMR2_14_POSTSCALER;
#define TMR2_300BAUD_PR    (uint8_t)238;	


#define TMR2_150BAUD_PRE   TMR2_04_PRESCALER;
#define TMR2_150BAUD_POST  TMR2_07_POSTSCALER;
#define TMR2_150BAUD_PR    (uint8_t)238;			

#define TMR2_DDS_CFG_PR    (uint8_t)20; // (uint8_t)10;	


// the rx_luz tmr2 cfg
#define TMR2_15MS_PRE     TMR2_04_PRESCALER
#define TMR2_15MS_POST    TMR2_15_POSTSCALER
#define TMR2_15MS_PR      250u

#define TMR2_1MS_PRE      TMR2_04_PRESCALER
#define TMR2_1MS_POST     TMR2_01_POSTSCALER
#define TMR2_1MS_PR       250u

#define TMR2_500US_PRE    TMR2_04_PRESCALER
#define TMR2_500US_POST   TMR2_01_POSTSCALER
#define TMR2_500US_PR     125u

#define TMR2_3072US_PRE   TMR2_04_PRESCALER
#define TMR2_3072US_POST  TMR2_08_POSTSCALER
#define TMR2_3072US_PR    96u


#else
  
wat

#endif




// --------   T M R  4  ---------------
#define TMR4_ON T4CONbits.TMR4ON
#define T4_PRESCALER	T4CONbits.T4CKPS
#define T4_POSTSCALER	T4CONbits.T4OUTPS

#define TMR4_IE PIE3bits.TMR4IE
#define TMR4_IF PIR3bits.TMR4IF

#define TMR4_01_PRESCALER	0u
#define TMR4_04_PRESCALER	1u
#define TMR4_16_PRESCALER	2u
#define TMR4_64_PRESCALER	3u

#define TMR4_01_POSTSCALER	0u
#define TMR4_02_POSTSCALER	1u
#define TMR4_03_POSTSCALER	2u
#define TMR4_04_POSTSCALER	3u
#define TMR4_05_POSTSCALER	4u
#define TMR4_06_POSTSCALER	5u
#define TMR4_07_POSTSCALER	6u
#define TMR4_08_POSTSCALER	7u
#define TMR4_09_POSTSCALER	8u
#define TMR4_10_POSTSCALER	9u
#define TMR4_11_POSTSCALER	10u
#define TMR4_12_POSTSCALER	11u
#define TMR4_13_POSTSCALER	12u
#define TMR4_14_POSTSCALER	13u
#define TMR4_15_POSTSCALER	14u
#define TMR4_16_POSTSCALER	15u

// --------   T M R  6  ---------------
#define TMR6_ON       T6CONbits.TMR6ON
#define T6_PRESCALER	T6CONbits.T6CKPS
#define T6_POSTSCALER	T6CONbits.T6OUTPS

#define TMR6_IE PIE3bits.TMR6IE
#define TMR6_IF PIR3bits.TMR6IF

#define TMR6_01_PRESCALER	0u
#define TMR6_04_PRESCALER	1u
#define TMR6_16_PRESCALER	2u
#define TMR6_64_PRESCALER	3u

#define TMR6_01_POSTSCALER	0u
#define TMR6_02_POSTSCALER	1u
#define TMR6_03_POSTSCALER	2u
#define TMR6_04_POSTSCALER	3u
#define TMR6_05_POSTSCALER	4u
#define TMR6_06_POSTSCALER	5u
#define TMR6_07_POSTSCALER	6u
#define TMR6_08_POSTSCALER	7u
#define TMR6_09_POSTSCALER	8u
#define TMR6_10_POSTSCALER	9u
#define TMR6_11_POSTSCALER	10u
#define TMR6_12_POSTSCALER	11u
#define TMR6_13_POSTSCALER	12u
#define TMR6_14_POSTSCALER	13u
#define TMR6_15_POSTSCALER	14u
#define TMR6_16_POSTSCALER	15u



// Watch Dog TIMER timeout
#define WDT_TIMEOUT_001ms_timeout 0x00u
#define WDT_TIMEOUT_002ms_timeout 0x01u
#define WDT_TIMEOUT_004ms_timeout 0x02u
#define WDT_TIMEOUT_008ms_timeout 0x03u
#define WDT_TIMEOUT_016ms_timeout 0x04u
#define WDT_TIMEOUT_032ms_timeout 0x05u
#define WDT_TIMEOUT_064ms_timeout 0x06u
#define WDT_TIMEOUT_128ms_timeout 0x07u
#define WDT_TIMEOUT_256ms_timeout 0x08u
#define WDT_TIMEOUT_512ms_timeout 0x09u
#define WDT_TIMEOUT_001s_timeout  0x0Au
#define WDT_TIMEOUT_002s_timeout  0x0Bu
#define WDT_TIMEOUT_004s_timeout  0x0Cu
#define WDT_TIMEOUT_008s_timeout  0x0Du
#define WDT_TIMEOUT_016s_timeout  0x0Eu
#define WDT_TIMEOUT_032s_timeout  0x0Fu
#define WDT_TIMEOUT_064s_timeout  0x10u
#define WDT_TIMEOUT_128s_timeout  0x11u
#define WDT_TIMEOUT_256s_timeout  0x12u
#define MAX_WDT_TIMEOUT  0x12u



#endif// if 0




#if MIPS == 8

#define eRTC_TMR_PSA   TMR4_64_PRESCALER;
#define eRTC_TMR_POST  TMR4_10_POSTSCALER;
#define eRTC_TMR_PR    (uint8_t)250;	
#define eRTC_TMR_CNT   (uint8_t)10;

#elif MIPS == 4

#define  eRTC_TMR_PSA  TMR4_64_PRESCALER;
#define  eRTC_TMR_POST TMR4_10_POSTSCALER;
#define  eRTC_TMR_PR   (uint8_t)250;	
#define  eRTC_TMR_CNT  (uint8_t)5;

#elif MIPS == 2

#define  eRTC_TMR_PSA  TMR4_64_PRESCALER;
#define  eRTC_TMR_POST TMR4_10_POSTSCALER;
#define  eRTC_TMR_PR   (uint8_t)125;	
#define  eRTC_TMR_CNT  (uint8_t)5; 

#elif MIPS == 1

#define  eRTC_TMR_PSA  TMR4_16_PRESCALER;
#define  eRTC_TMR_POST TMR4_10_POSTSCALER;
#define  eRTC_TMR_PR   (uint8_t)125;	
#define  eRTC_TMR_CNT  (uint8_t)10;

#else
  
#define  eRTC_TMR_PSA  TMR4_01_PRESCALER;
#define  eRTC_TMR_POST TMR4_11_POSTSCALER;
#define  eRTC_TMR_PR   (uint8_t)142;	
#define  eRTC_TMR_CNT  (uint8_t)1;  

#endif

// f.e.
// 8 MIPS and 200ms 
// #define eRTC_TMR_PSA = TMR4_64_PRESCALER;
// #define eRTC_TMR_POST = TMR4_10_POSTSCALER;
// #define eRTC_TMR_PR = (uint8_t)250;	
// #define eRTC_TMR_CNT = (uint8_t)10;
    
// 4 MIPS and 200ms
// #define  eRTC_TMR_PSA = TMR4_64_PRESCALER;
// #define  eRTC_TMR_POST = TMR4_10_POSTSCALER;
// #define  eRTC_TMR_PR = (uint8_t)250;	
// #define  eRTC_TMR_CNT = (uint8_t)5;

// 2 MIPS and 200ms
// #define  eRTC_TMR_PSA = TMR4_64_PRESCALER;
// #define  eRTC_TMR_POST = TMR4_10_POSTSCALER;
// #define  eRTC_TMR_PR = (uint8_t)125;	
// #define  eRTC_TMR_CNT = (uint8_t)5;    
    
// 1 MIPS and 200ms
// #define  eRTC_TMR_PSA = TMR4_16_PRESCALER;
// #define  eRTC_TMR_POST = TMR4_10_POSTSCALER;
// #define  eRTC_TMR_PR = (uint8_t)125;	
// #define  eRTC_TMR_CNT = (uint8_t)10;        
    
// 31.25kHz Clock and 200ms

#if SLOW_CLCK == LOW_31_25KHZ // 0 // SLOW_CLCK == 3125KHZ

#define  eRTC_TMR_PSA_SLOW_CLCK  TMR4_01_PRESCALER;
#define  eRTC_TMR_POST_SLOW_CLCK TMR4_11_POSTSCALER;
#define  eRTC_TMR_PR_SLOW_CLCK   (uint8_t)142;	
#define  eRTC_TMR_CNT_SLOW_CLCK  (uint8_t)1; 

#define  TMR0_CFG_SLOW_CLCK (uint8_t)0x88u

#elif SLOW_CLCK == LOW_500KHZ

#define  eRTC_TMR_PSA_SLOW_CLCK  TMR4_16_PRESCALER;
#define  eRTC_TMR_POST_SLOW_CLCK TMR4_11_POSTSCALER;
#define  eRTC_TMR_PR_SLOW_CLCK   (uint8_t)142;	
#define  eRTC_TMR_CNT_SLOW_CLCK  (uint8_t)1;   

#define  TMR0_CFG_SLOW_CLCK (uint8_t)0x88u

#else
  
err here --> undefined so far


#endif


#if MIPS == 8

#define TMR0_CFG (uint8_t)0x85u

#elif MIPS == 4

#define TMR0_CFG (uint8_t)0x84u

#elif MIPS == 2

#define TMR0_CFG (uint8_t)0x83u

#elif MIPS == 1

#define TMR0_CFG (uint8_t)0x82u

#else
  

 wat

#endif

typedef enum tmr1_id_e{
  
  RX_LUZ_TIME_OUT,
  GPS_UART_TIMEOUT,
  STATUS_LED_TIMEOUT,
  NUM_TMR1_ID,
  
}tmr1_id_t;


void start_timeout_tmr(void);

void stop_timeout_tmr(void);


void timers_tmr1_decreaser(void);

void timers_set_tmr1_id(tmr1_id_t t_id);

uint8_t timeout_checker(void);

void reset_timeout_timer(void);

void configure_tmr4(void);

void init_tmr0(void);

void init_tmr1(void);

void init_wdt(void);

void my_delay_ms(uint16_t ms_cnt);

extern volatile uint8_t tmr4_200ms_of;


























#endif //TIMERS_H