#ifndef BSP_PCB_VXX66_H
#define BSP_PCB_VXX66_H

#define PCB_V_STRING "66"



  // --------   PORT A  ---------------
  #define SYNC_AD9954 	LATAbits.LATA0		
  #define SDIO_AD9954 	LATAbits.LATA1		
  #define SCLK_AD9954 	LATAbits.LATA2	 
  #define LDR 	        PORTAbits.RA3   
  #define VDD_AD9954 	  LATAbits.LATA4 
  #define BATERIA 			PORTAbits.RA5   
  #define EN_KS_50      LATAbits.LATA6
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
  // #define BB_DIRECT 		LATBbits.LATB7
  // #define ICSPDAT 			PORTBbits.RB7


  // --------   PORT C  ---------------
  #define FREE_RC0 			LATCbits.LATC0
  #define FREE_RC1 			LATCbits.LATC1
  #define RESET_AD9954  LATCbits.LATC2  // that is basically unused so far...
  #define FREE_RC3		  LATCbits.LATC3
  #define FREE_RC4		  LATCbits.LATC4
  #define GPS_VALIM			LATCbits.LATC5
  #define UART_TX_PC 		LATCbits.LATC6
  #define UART_RX_PC 		PORTCbits.RC7


  // --------   PORT E  ---------------
  #define MCLR 			PORTEbits.RE3

#define AD9954_SDI PORTAbits.RA1
#define SDIO_TRIS TRISAbits.TRISA1




#define INVERTED_LDR_SENSOR 0
#define USE_SPI_TILT 0

#define L_POWER_LATA 0x40 // KFS_50 inverted Logic needs that
#if DEBUGGING_BB_IS_ON  
  #define L_POWER_LATB 0x40
#else
  #define L_POWER_LATB 0x00
#endif  
#define L_POWER_LATC 0x00


#define INIT_ANSELA 0x28  // 40u
#define INIT_ANSELB 0x00

#define INIT_TRISA 40u
#define INIT_TRISB 1u
#define INIT_TRISC 128u


// --------   AN_Channels names  ---------------
#define LDR_ADC_CHANNEL 3
#define BATERIA_ADC_CHANNEL 4
#define VREF_ADC_CHANNEL 0x1F




#define STATUS_LED_RED_ON() DB_PRINT("\r\nRED_ON\r\n");
#define STATUS_LED_GREEN_ON() DB_PRINT("\r\nGREEN_ON\r\n");
#define STATUS_LED_RED_OFF() DB_PRINT("\r\nRED_OFF\r\n");
#define STATUS_LED_GREEN_OFF() DB_PRINT("\r\nGREEN_OFF\r\n");
#define STATUS_LED_RED_SWAP()   DB_PRINT("RED_BLINKS\r\n");
#define STATUS_LED_GREEN_SWAP() DB_PRINT("GREEN_BLINKS\r\n");


#define KS_50_ON EN_KS_50=0;  // becasue the KS50 works inverted
#define KS_50_OFF EN_KS_50=1;

#define VCC_TLV_ON() EN_KS_50=0;  // becasue the KS50 works inverted
#define VCC_TLV_OFF() EN_KS_50=1;

// #define SENSOR_LDR_IDENTIFYER 49
#define BIG_Z 0x5A
#define SENSOR_LDR_IDENTIFYER BIG_Z // 0x85

#define SENSOR_IS_TOP_MOUNTED ((uint8_t)1u) // because the signal is invertred depending on the sid of mounting


// rx_luz related

enum{
  
  TMR_15ms_OF_TIME,
  TMR_1ms_OF_TIME,
  TMR_500us_OF_TIME,
  TMR_3072us_OF_TIME,
  
};

#define M_ADC_THRESHOLD 10u

#define M_US_TO_SEND "150000"

#define DEFAULT_TMR2 TMR_15ms_OF_TIME


























#endif // BSP_PCB_VXX69_H