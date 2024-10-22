#ifndef IO_PORT_SFR_NAMES_H
#define IO_PORT_SFR_NAMES_H

#include "xc.h"

#include "Global.h"


#define RELEASE_THIS_VERSION	0	// Not Used anymore


#define DEBUG_16F1936	0	// 25052021	// for different code parts which are just for debugging purposses adde, p.e. DB_LED, etc...

#define DEBUG_16F1936_UART	0	// this is set in fron of all debugging related UART sentences towards the PC

#define DB_LUZ_UART 0

#define UART_char transmit_char


#define UART_TAB UART_char(9);


#define UNUSED_FOR_UART_LUZ 0


#define INT_IE INTCONbits.INTE
#define INT_IF INTCONbits.INTF


#define ADC_ON	ADCON0bits.ADON




#if 0
// --------   T M R  0  ---------------
#define TMR0_IE INTCONbits.TMR0IE
#define TMR0_IF INTCONbits.TMR0IF

#define TMR0_002_PRESCALER	0
#define TMR0_004_PRESCALER	1
#define TMR0_008_PRESCALER	2
#define TMR0_016_PRESCALER	3
#define TMR0_032_PRESCALER	4
#define TMR0_064_PRESCALER	5
#define TMR0_128_PRESCALER	6
#define TMR0_256_PRESCALER	7


// --------   T M R  1  ---------------
#define TMR1_ON T1CONbits.TMR1ON
#define T1_PRESCALER	T1CONbits.T1CKPS
#define TMR1_IE PIE1bits.TMR1IE
#define TMR1_IF PIR1bits.TMR1IF

#define TMR1_1_PRESCALER	0
#define TMR1_2_PRESCALER	1
#define TMR1_4_PRESCALER	2
#define TMR1_8_PRESCALER	3



// --------   T M R  2  ---------------
#define TMR2_ON T2CONbits.TMR2ON
#define T2_PRESCALER	T2CONbits.T2CKPS
#define T2_POSTSCALER	T2CONbits.T2OUTPS

#define TMR2_IE PIE1bits.TMR2IE
#define TMR2_IF PIR1bits.TMR2IF

#define TMR2_01_PRESCALER	0
#define TMR2_04_PRESCALER	1
#define TMR2_16_PRESCALER	2
#define TMR2_64_PRESCALER	3

#define TMR2_01_POSTSCALER	0
#define TMR2_02_POSTSCALER	1
#define TMR2_03_POSTSCALER	2
#define TMR2_04_POSTSCALER	3
#define TMR2_05_POSTSCALER	4
#define TMR2_06_POSTSCALER	5
#define TMR2_07_POSTSCALER	6
#define TMR2_08_POSTSCALER	7
#define TMR2_09_POSTSCALER	8
#define TMR2_10_POSTSCALER	9
#define TMR2_11_POSTSCALER	10
#define TMR2_12_POSTSCALER	11
#define TMR2_13_POSTSCALER	12
#define TMR2_14_POSTSCALER	13
#define TMR2_15_POSTSCALER	14
#define TMR2_16_POSTSCALER	15

// --------   T M R  4  ---------------
#define TMR4_ON T4CONbits.TMR4ON
#define T4_PRESCALER	T4CONbits.T4CKPS
#define T4_POSTSCALER	T4CONbits.T4OUTPS

#define TMR4_IE PIE3bits.TMR4IE
#define TMR4_IF PIR3bits.TMR4IF

#define TMR4_01_PRESCALER	0
#define TMR4_04_PRESCALER	1
#define TMR4_16_PRESCALER	2
#define TMR4_64_PRESCALER	3

#define TMR4_01_POSTSCALER	0
#define TMR4_02_POSTSCALER	1
#define TMR4_03_POSTSCALER	2
#define TMR4_04_POSTSCALER	3
#define TMR4_05_POSTSCALER	4
#define TMR4_06_POSTSCALER	5
#define TMR4_07_POSTSCALER	6
#define TMR4_08_POSTSCALER	7
#define TMR4_09_POSTSCALER	8
#define TMR4_10_POSTSCALER	9
#define TMR4_11_POSTSCALER	10
#define TMR4_12_POSTSCALER	11
#define TMR4_13_POSTSCALER	12
#define TMR4_14_POSTSCALER	13
#define TMR4_15_POSTSCALER	14
#define TMR4_16_POSTSCALER	15

// --------   T M R  6  ---------------
#define TMR6_ON       T6CONbits.TMR6ON
#define T6_PRESCALER	T6CONbits.T6CKPS
#define T6_POSTSCALER	T6CONbits.T6OUTPS

#define TMR6_IE PIE3bits.TMR6IE
#define TMR6_IF PIR3bits.TMR6IF

#define TMR6_01_PRESCALER	0
#define TMR6_04_PRESCALER	1
#define TMR6_16_PRESCALER	2
#define TMR6_64_PRESCALER	3

#define TMR6_01_POSTSCALER	0
#define TMR6_02_POSTSCALER	1
#define TMR6_03_POSTSCALER	2
#define TMR6_04_POSTSCALER	3
#define TMR6_05_POSTSCALER	4
#define TMR6_06_POSTSCALER	5
#define TMR6_07_POSTSCALER	6
#define TMR6_08_POSTSCALER	7
#define TMR6_09_POSTSCALER	8
#define TMR6_10_POSTSCALER	9
#define TMR6_11_POSTSCALER	10
#define TMR6_12_POSTSCALER	11
#define TMR6_13_POSTSCALER	12
#define TMR6_14_POSTSCALER	13
#define TMR6_15_POSTSCALER	14
#define TMR6_16_POSTSCALER	15

#endif// if 0

// --------- EDGE DETECTION BIT -----------
#define IOC_IE	INTCONbits.IOCIE
#define IOC_IF	INTCONbits.IOCIF
#define IOCB_IF IOCBF


// --------   UART INTERRUPT  ---------------
#define RX_IE PIE1bits.RCIE
#define RX_IF PIR1bits.RCIF

#define TX_IE PIE1bits.TXIE
#define TX_IF	PIR1bits.TXIF


#define PERIPHERIC_IE	INTCONbits.PEIE
#define GLOBAL_IE INTCONbits.GIE


#if HW_GPS_DETECTOR


// --------   PORT A  ---------------
#define SYNC_AD9954 	LATAbits.LATA0		
#define SDIO_AD9954 	LATAbits.LATA1		
#define SCLK_AD9954 	LATAbits.LATA2	 
#define BATERIA 	    PORTAbits.PORTA3   
#define VDD_AD9954 	  LATAbits.LATA4 
#define LDR 				  PORTAbits.PORTA5   
#define FREE_RA6      LATAbits.LATA6
#define LED           LATAbits.LATA7



// --------   PORT B  ---------------
#define TILT_SENSOR 	PORTBbits.RB0
#define FREE_RB1 	    LATBbits.LATB1
#define PS0_AD9954 	  LATBbits.LATB2
#define PS1_AD9954 	  LATBbits.LATB3
#define UPDATE_AD9954 LATBbits.LATB4
#define FREE_RB5 	    LATBbits.LATB5
#define ICSPCLCK 			LATBbits.LATB6
#define ICSPDAT 			PORTBbits.RB7


// --------   PORT C  ---------------
#define FREE_RC0 			LATCbits.LATC0
#define FREE_RC1 			LATCbits.LATC1
#define RESET_AD9954  LATCbits.LATC2
#define FREE_RC3		  LATCbits.LATC3
#define FREE_RC4		  LATCbits.LATC4
#define GPS_VALIM			LATCbits.LATC5
#define UART_TX_PC 		LATCbits.LATC6
#define UART_RX_PC 		PORTCbits.RC7


// --------   PORT E  ---------------
#define MCLR 			PORTEbits.RE3



// --------   AN_Channels names  ---------------
#define BATERIA_ADC_CHANNEL 3
#define LDR_ANALOG_CHANNEL 4
#define VREF_ADC_CHANNEL 0x1F


// --------   DEBUGGING  ---------------
#if DEBUGGING_IS_ON

#define DB_LED_1 LED 
#define DB_LED_2 GPS_VALIM 

#else
  
#define DB_LED_1
#define DB_LED_2

#endif





#endif








#endif	// IO_PORT_SFR_NAMES_H