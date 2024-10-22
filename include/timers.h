#ifndef TIMERS_H
#define TIMERS_H



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



// --------   T M R  2  ---------------
#define TMR2_ON T2CONbits.TMR2ON
#define T2_PRESCALER	T2CONbits.T2CKPS
#define T2_POSTSCALER	T2CONbits.T2OUTPS

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

#endif// if 0



void configure_tmr4(void);


void configure_tmr2(void);


void init_tmr0(void);


void init_tmr1(void);



extern uint8_t tmr4_200ms_of;






























#endif //TIMERS_H