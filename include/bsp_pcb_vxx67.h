#ifndef BSP_PCB_VXX69_H
#define BSP_PCB_VXX69_H


#define PCB_V_STRING "67"


// --------   PORT A  ---------------
#define VDD_AD9954  	LATAbits.LATA0		
#define UNUSED_RA1    LATAbits.LATA1		
#define SCLK_AD9954 	LATAbits.LATA2	 
#define UPDATE_AD9954 LATAbits.LATA3   
#define PS1_AD9954 	  LATAbits.LATA4 
#define PS0_AD9954 	  LATAbits.LATA5   
#define SYNC_AD9954   LATAbits.LATA6
#define SDIO_AD9954   LATAbits.LATA7


// --------   PORT B  ---------------
#define BATERIA 	        PORTBbits.RB0
#define RESET_AD9954 	    LATBbits.LATB1 
#define EN_TLV 	          LATBbits.LATB2
#define LDR	              PORTBbits.RB3
#define STATUS_LED_RED    LATBbits.LATB4
#define STATUS_LED_GREEN  LATBbits.LATB5 
#define ICSPCLCK 			    LATBbits.LATB6
#define ICSPDAT 			    LATBbits.LATB7



// --------   PORT C  ---------------
#define BB_SPI_SDO		LATCbits.LATC0
#define BB_SPI_SDI		PORTCbits.RC1
#define BB_SPI_CS     LATCbits.LATC2  
#define BB_SPI_CLCK		LATCbits.LATC3
#define GPS_VALIM	    LATCbits.LATC4
#define LED			      LATCbits.LATC5
#define UART_TX_PC 		LATCbits.LATC6
#define UART_RX_PC 		PORTCbits.RC7


// --------   PORT E  ---------------
#define MCLR 			PORTEbits.RE3


#define AD9954_SDI PORTAbits.RA7
#define SDIO_TRIS TRISAbits.TRISA7  


#define INVERTED_LDR_SENSOR 1
#define USE_SPI_TILT 1


// the init values for the port pins...
#define L_POWER_LATA 0x00 // Becasue of GPS_Reset pin

#if DEBUGGING_BB_IS_ON  
  #define L_POWER_LATB 0x40
#else
  #define L_POWER_LATB 0x00
#endif  

#define L_POWER_LATC 0x04

#define INIT_ANSELA 0x00  // 40u
#define INIT_ANSELB 0x09

#define INIT_TRISA 0x00
#define INIT_TRISB 0x09
#define INIT_TRISC 0x82





// --------   AN_Channels names  ---------------
#define LDR_ADC_CHANNEL 9
#define BATERIA_ADC_CHANNEL 12
#define VREF_ADC_CHANNEL 0x1F



#define STATUS_LED_RED_ON() STATUS_LED_RED=true;
#define STATUS_LED_GREEN_ON() STATUS_LED_GREEN=true;
#define STATUS_LED_RED_OFF() STATUS_LED_RED=false;
#define STATUS_LED_GREEN_OFF() STATUS_LED_GREEN=false;
#define STATUS_LED_RED_SWAP() STATUS_LED_RED=!STATUS_LED_RED;
#define STATUS_LED_GREEN_SWAP() STATUS_LED_GREEN=!STATUS_LED_GREEN;

#define KS_50_ON
#define KS_50_OFF

#define VCC_TLV_ON()  EN_TLV=1;
#define VCC_TLV_OFF() EN_TLV=0;

#define BIG_Z 0x5A
#define SENSOR_LDR_IDENTIFYER BIG_Z // 0x85

#define SENSOR_IS_TOP_MOUNTED ((uint8_t)0u) 


// rx_luz related

enum{
  
  TMR_15ms_OF_TIME,
  TMR_1ms_OF_TIME,
  TMR_500us_OF_TIME,
  TMR_3072us_OF_TIME,
  
};




#define M_ADC_THRESHOLD 154u

#define M_US_TO_SEND "030720"

#define DEFAULT_TMR2 TMR_3072us_OF_TIME


































#endif // BSP_PCB_VXX69_H