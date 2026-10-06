#ifndef IO_PORT_SFR_NAMES_H
#define IO_PORT_SFR_NAMES_H

#include "xc.h"

#include "Global.h"

// PCB_VERSIONS:
// 66 --> The Version with 16F1936, analog inclination sensor
// 67 --> SPI_TILT, Status LEDs,  
// 68 --> These were never for sales and are intermediate between 67 and 69
// 69 --> the PCB with SPI_TILT_SENSOR which got rejected by its numbering system...


// these i use when there is a pin to be checked and i use the three DB_LED for that...
#if PCB_VERSION==69

  #include "bsp_pcb_vxx69.h"

#elif PCB_VERSION==68

  #include "bsp_pcb_vxx68.h"

#elif PCB_VERSION==67

  #include "bsp_pcb_vxx67.h"
  
#elif PCB_VERSION==66

  #include "bsp_pcb_vxx66.h"

#else

wat

#endif  // IO_PORTS_FROM_VERSION





#define DB_LUZ_UART 0

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



// ---------------  P O R T   P I N   C A L L S   A S   M A C R O S   ------------------------------
// ------------------  A L L   V E R S I O N S    ---------------------------------------

#define VALIM_DDS_ON() VDD_AD9954 = true;
#define VALIM_DDS_OFF() VDD_AD9954 = false;

// Because in the end we did not implement the On/Off
#define VALIM_TILT_ON()
#define VALIM_TILT_OFF()  
  

// ---------------  P O R T   P I N   C A L L S   A S   M A C R O S   ------------------------------
// ------------------  V E R S I O N   S P E C I F I C   ---------------------------------------




// --------   DEBUGGING  ---------------
#if DEBUGGING_BB_IS_ON

#define USE_BB_LED_OUTPUT 1

#else
  
#define USE_BB_LED_OUTPUT 0
  
#endif

#if USE_BB_LED_OUTPUT

#define DB_LED_1 ICSPDAT   // 
#define DB_LED_2 ICSPCLCK // BB_TX_UART

#else
  
#define DB_LED_1
#define DB_LED_2
#define DB_LED_3

#endif

#endif




#endif	// IO_PORT_SFR_NAMES_H