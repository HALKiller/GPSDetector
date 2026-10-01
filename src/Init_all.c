// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// the init routines for all the necessary periferals 
// because we want a good random function we initialize early the TMR2_ON// and let it 
// run freely so that when the Rx line goes high for the first time we use the accumulated TMR2 value as seed
// 

#include "Init_all.h"

#include "Global.h"

#include "Clock.h"

#include "UART.h"

#include "detector.h"

#include "timers.h"

#include "gd_states.h"

#include "handlers.h"

#include "io_port_sfr_names.h"

#include "generic_union_flgs.h"

#include "tilt_sensor.h"

#include "messages.h"

#include "string.h"



#include "xc.h"


// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //
#ifndef FILE_INIT_ALL_DB_ENABLED
#define FILE_INIT_ALL_DB_ENABLED 0
#endif
#if FILE_INIT_ALL_DB_ENABLED
#define DB_PRINT(str) G_DB_PRINT(str)
#define UART_int(var) G_UART_INT(var)
#else
#define DB_PRINT(str)
#define UART_int(var)
#endif
// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  




#define DEBUG_FOTOTRANSISTOR 0

#define DEBUG_DEVICE_DRIVER_CONFIG 0

#define MHZ32_TEST 0





// static void init_wdt(void);
static void init_IO_PORTS(void);



// this is the keeper of the reset output...
static uint8_t t_status __at(0x16F); // (0xA0);



void init_all(void){


#if DEBUGGING_IS_ON||DEBUGGING_BB_IS_ON
  uint8_t t_var = (uint8_t)MIPS;
#endif

  CLRWDT();

  init_clock();


// datasheet --> switching to the PLL can take +- 2ms --> 
	// __delay_ms(5u);


#if !COMPILE_FOR_RELEASE
// we are using this timer also with SPI_TILT_SENSOR --> NOT ANYMORE! 06112025
  init_TMR_bitbang_uart(NORMAL_CLOCK);
#endif

	init_tmr1();
	

#if (DEBUGGING_IS_ON||DEBUGGING_BB_IS_ON)||1

  DB_PRINT("\r\n--- RESET ");
	DB_PRINT("BUILD: ");
	DB_PRINT(__DATE__);
	DB_PRINT("  ");
	DB_PRINT(__TIME__);
	// DB_PRINT("\r\n");
	// DB_PRINT("\r\nMIPS: ");
  // UART_CRLF;
  
#endif


  gd_states_initialize();
  
  init_handler_flg();
  
  init_detector_config();
	
	init_IO_PORTS();
 
  configure_tmr4();
  
  tilt_sensor_init();
  
	init_wdt();
#if !GLONASS_BUG
  gps_first_run();
#endif
  
  startup();

#if GLONASS_BUG
  gps_first_run();
#endif
  
#if 0
// for testing consumptioon in LPM tilt sensor setup
  SWITCH_CLOCK = true;
#endif
	
}




// we set here all the timers and handlers and stuff
void startup(void){
  
  dFLAGS.reg = 0u;
  
  tilt_sensor_init();
  
  gps_init();
  
  
#if EMULATE_GPS_TIME_POSITION  

  RTC_TIME_IS_GOOD = false;
  RTC_ALARM_ON = false;
  eRTC_clock_reset();
 
#else  
  // on startup we reset the time...
  RTC_TIME_IS_GOOD = false;
  RTC_ALARM_ON = false;
  eRTC_clock_reset();
  
#endif
  // TODO:  we should reset the time also ...
  gd.no_position_cnt = 0u;
  
  // setting up the initial state 
  detector_init_ilumination_handling();
  
  
	TMR0_IF = FALSE;
	TMR0_IE = FALSE;

	PERIPHERIC_IE = TRUE;
	GLOBAL_IE = TRUE;
  TMR4_IF = FALSE;
  TMR4_IE = TRUE;
  TMR4_ON = TRUE;
  

  measure_bat_for_batcnt(E_STARTUP_STATE);


  
}



void set_lpm_ioports(void){
  
  LATA = L_POWER_LATA;
  LATB = L_POWER_LATB;
  LATC = L_POWER_LATC;
  
  
}

#if 1


static void init_IO_PORTS(void){




#if USE_DEVICE_DRIVER

	IO_Init();

#else
 
	ANSELA = INIT_ANSELA;
	ANSELB = INIT_ANSELB;

  set_lpm_ioports();

  TRISA = INIT_TRISA;
  TRISB = INIT_TRISB;
  TRISC = INIT_TRISC;
  
  WPUB = 0b00000000;

  WPUE = 0x08;
  
  
  
#endif



}



#else

static void init_IO_PORTS(void){




#if USE_DEVICE_DRIVER

	IO_Init();




#elif PCB_VERSION==69

	ANSELA = 0x00;
	ANSELB = 0x09;


#if USE_SINGLE_LPM_SETTER  
  
  set_lpm_ioports();
  
#else  
	LATA = 0x40;  // GPS_Reset needs to stay high
#if DEBUGGING_BB_IS_ON
  LATB = 0x40;  // Bit banged Tx pin RB6
#else  
	LATB = 0x00;
#endif
	LATC = 0x00;  // 0x04; 
#endif


	TRISA = 0x00; // 40u;  // 0x1F;
  TRISB = 0x09; // 0x00;  // 	TRISB = 0x01; // 0x00; becasue that is now Valim_Tilt...
	TRISC = 0x82; // 0x80;

	WPUB = 0b00000000;

  WPUE = 0x08;
  


#elif USE_SPI_TILT



	ANSELA = 0x28;  // 40u; // 0x03;	
	ANSELB = 0x00;	

#if PCB_VERSION == 66
	LATA = 0x40;  // Becasu there is an inverted logic implemented...
#else
	LATA = 0x00;
#endif

	LATB = 0x00;
	LATC = 0x04;

	TRISA = 0x28; // 40u;  // 0x1F;
  TRISB = 0x00; // 0x00;  // 	TRISB = 0x01; // 0x00; becasue that is now Valim_Tilt...
	TRISC = 0x90; // 0x80;

	WPUB = 0b00000000;
  WPUE = 0x08;
  DB_PRINT("\r\nSPI_TILT_PREP\r\n");


#elif 0




	ANSELA = 0x28u; // 0x03;	
	ANSELB = 0x00;	

#if PCB_VERSION == 66
	LATA = 0x40;  // Becasu there is an inverted logic implemented...
#else
	LATA = 0x00;
#endif

	LATB = 0x00;
	LATC = 0x04;

	TRISA = 0x28; // 40u;  // 0x1F;
	TRISB = 0x01; // 0x00;
	TRISC = 0x80; // 128u; // 0x80;

	WPUB = 0x00;  // 0b00000000;



#else






	ANSELA = 40u; // 0x03;	
	ANSELB = 0x00;	
	// ANSELC = 0x00;
#if PCB_VERSION == 66
	LATA = 0x40;  // Becasu there is an inverted logic implemented...
#else
	LATA = 0x00;
#endif
	LATB = 0x00;
	LATC = 0x04;

	TRISA = 40u;  // 0x1F; 0x28; // 
	TRISB = 1u; // 0x00;   0x01; //  
	TRISC = 128u; // 0x80; 0x00; // 

	WPUB = 0b00000000;


#endif

}

#endif
// for checking the correct cfg io
#if 0

// from_here_1
  DB_PRINT("LAT: ");
  UART_int(LATA);
  UART_CRLF;
  
  UART_int(LATB);
  UART_CRLF;
  
  UART_int(LATC);
  UART_CRLF;
  
  DB_PRINT("TRIS: ");
  UART_int(TRISA);
  UART_CRLF;
  
  UART_int(TRISB);
  UART_CRLF;
  
  UART_int(TRISC);
  UART_CRLF;
  
  
  DB_PRINT("ANSEL: ");
  UART_int(ANSELA);
  UART_CRLF;
  
  UART_int(ANSELB);
  UART_CRLF;


#endif


// EOF

