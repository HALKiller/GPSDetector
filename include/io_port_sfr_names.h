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
#define ICSPDAT 			LATBbits.LATB7
// #define ICSPDAT 			PORTBbits.RB7


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
#define LDR_ADC_CHANNEL 3

#define VREF_ADC_CHANNEL 0x1F


// --------   DEBUGGING  ---------------
#if DEBUGGING_IS_ON

#define DB_LED_1 LED 
#define DB_LED_2 GPS_VALIM 
#define DB_LED_3 ICSPDAT
#else
  
#define DB_LED_1
#define DB_LED_2
#define DB_LED_3
#endif





#endif








#endif	// IO_PORT_SFR_NAMES_H