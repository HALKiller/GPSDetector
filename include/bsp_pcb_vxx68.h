#ifndef BSP_PCB_VXX68_H
#define BSP_PCB_VXX68_H

#define PCB_V_STRING "68"

// --------   PORT A  ---------------
#define SYNC_AD9954 	LATAbits.LATA0		
#define SDIO_AD9954 	LATAbits.LATA1		
#define SCLK_AD9954 	LATAbits.LATA2	 
#define BATERIA 	    PORTAbits.PORTA3   
#define VDD_AD9954 	  LATAbits.LATA4 
#define LDR 				  PORTAbits.PORTA5   
#define FREE_RA6      LATAbits.LATA6  // becaseu that is always together with the DDS
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



// --------   PORT C  ---------------
#define BB_SPI_CS 		LATCbits.LATC0
#define BB_SPI_CLCK		LATCbits.LATC1
#define RESET_AD9954  LATCbits.LATC2  // that is basically unused so far...
#define BB_SPI_SDO		LATCbits.LATC3
#define BB_SPI_SDI		PORTCbits.RC4
#define GPS_VALIM			LATCbits.LATC5
#define UART_TX_PC 		LATCbits.LATC6
#define UART_RX_PC 		PORTCbits.RC7


// --------   PORT E  ---------------
#define MCLR 			PORTEbits.RE3
  
  
#define AD9954_SDI PORTAbits.RA1
#define SDIO_TRIS TRISAbits.TRISA1


  #define INVERTED_LDR_SENSOR 1
  #define USE_SPI_TILT 0
  
  #define L_POWER_LATA 0x00
  #if DEBUGGING_BB_IS_ON  
    #define L_POWER_LATB 0x40
  #else
    #define L_POWER_LATB 0x00
  #endif  
  #define L_POWER_LATC 0x00
  
  
#define INIT_ANSELA 0x00
#define INIT_ANSELB 0x09

#define INIT_TRISA 0x00
#define INIT_TRISB 0x09
#define INIT_TRISC 0x82
  
  
  
  
// --------   AN_Channels names  ---------------
  #define LDR_ADC_CHANNEL 3
  #define BATERIA_ADC_CHANNEL 4
  #define VREF_ADC_CHANNEL 0x1F


  #define STATUS_LED_RED_ON()     DB_PRINT("\r\nRED_ON\r\n");
  #define STATUS_LED_GREEN_ON()   DB_PRINT("\r\nGREEN_ON\r\n");
  #define STATUS_LED_RED_OFF()    DB_PRINT("\r\nRED_OFF\r\n");
  #define STATUS_LED_GREEN_OFF()  DB_PRINT("\r\nGREEN_OFF\r\n");
  #define STATUS_LED_RED_SWAP()   DB_PRINT("RED_BLINKS\r\n");
  #define STATUS_LED_GREEN_SWAP() DB_PRINT("GREEN_BLINKS\r\n");
  
  #define KS_50_ON
  #define KS_50_OFF

  #define VCC_TLV_ON()
  #define VCC_TLV_OFF()
  
  #define BIG_Z 0x5A
  #define SENSOR_LDR_IDENTIFYER BIG_Z // 0x85

#define SENSOR_IS_TOP_MOUNTED ((uint8_t)1u) // because the signal is inverted depending on the sid of mounting


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




























#endif // BSP_PCB_VXX68_H