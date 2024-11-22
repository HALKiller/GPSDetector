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

#include "pwm_luz.h"

#include "timers.h"

#include "gd_states.h"

#include "handlers.h"

#include "io_port_sfr_names.h"

// #include "device_driver_config.h"

// #include "ADC.h"

#include "generic_union_flgs.h"

#include "tilt_sensor.h"

#include "messages.h"




#include "xc.h"


#define DEBUG_FOTOTRANSISTOR 0

#define DEBUG_DEVICE_DRIVER_CONFIG 0

#define MHZ32_TEST 0

// static uint8_t temp_tilt_sensor_tester(void);
// static void next_clock(void);




// static void init_wdt(void);
static void init_IO_PORTS(void);



// this is the keeper of the reset output...
static uint8_t t_status __at(0x16F); // (0xA0);

#if 1

void init_all(void){


#if DEBUGGING_IS_ON
  uint8_t t_var = (uint8_t)MIPS;
#endif

  CLRWDT();

	
  init_clock();


// datasheet --> switching to the PLL can take +- 2ms --> 
	__delay_ms(5u);

  
	
	// init_tmr0();
	
	init_tmr1();
	
	// configure_tmr2();
	
  // TODO:
  // gFLGS reset ons startup
#if DEBUGGING_IS_ON

  uart_init_cfg(B57600);

	UWT("BUILD: ");
	UWT(__DATE__);
	UWT("  ");
	UWT(__TIME__);
	UWT("\r\n");
	UWT("MIPS: ");
	UART_int(t_var);

  UWT("Week: ");
  t_var = (uint8_t)WEEK_OF_YEAR;
  UART_int(t_var);
  	UART_CRLF;
  
  
#endif

  gd_states_initialize();
  
  init_handler_flg();
  
  init_detector_config();
	
	init_IO_PORTS();
 
   // hunting reset states...
  DB_LED1_SWAP;
 
  configure_tmr4();
  
  tilt_sensor_init();
  
	init_wdt();

#if 1

  startup();
  
#else  
  
	RX_IF = FALSE;
	RX_IE = TRUE;
  
	TMR0_IF = FALSE;
	TMR0_IE = FALSE;


	PERIPHERIC_IE = TRUE;
	GLOBAL_IE = TRUE;

  TMR4_IF = FALSE;
  TMR4_IE = TRUE;
  
  TMR4_ON = TRUE;
  
#endif
  
  
// for reference...
  // TMR4ON = true;   // bad
  // TMR4_ON = TRUE;  // good
  // TMR4ON = TRUE;   // bad
  // TMR4_ON = true;  // bad
  


	
}


#else



void init_all(void){


#if DEBUGGING_IS_ON
  uint8_t t_var = MIPS;
#endif

  CLRWDT();

	
  init_clock();

// datasheet --> switching to the PLL can take +- 2ms --> 
	__delay_ms(5);

  
	
	// init_tmr0();
	
	// init_tmr1();
	
	// configure_tmr2();
	
  // TODO:
  // gFLGS reset ons startup
  

		
	// init_uart_flags();
	
	init_UART();
	
	init_IO_PORTS();
  
#if DEBUGGING_IS_ON
	UWT("BUILD: ");
	UWT(__DATE__);
	UWT("  ");
	UWT(__TIME__);
	UWT("\r\n");
	UWT("MIPS: ");
	UART_int(t_var);
	UART_CRLF;
#endif
  
  
  configure_tmr4();
  
  
  
  gd_states_initialize();
  
  
  
  // tilt_sensor_init();

  init_detector_config();


	init_handler_flg();



	RX_IF = FALSE;
	RX_IE = TRUE;
  
	TMR0_IF = FALSE;
	TMR0_IE = FALSE;


	PERIPHERIC_IE = TRUE;
	GLOBAL_IE = TRUE;


  TMR4_IF = FALSE;
  
  TMR4_IE = TRUE;
  
  TMR4_ON = TRUE;
  
  TIMEOUT_FLG = false;

// for reference...
  // TMR4ON = true;   // bad
  // TMR4_ON = TRUE;  // good
  // TMR4ON = TRUE;   // bad
  // TMR4_ON = true;  // bad
  



	
}

#endif

// we set here all the timers and handlers and stuff
void startup(void){
  
  tilt_sensor_init();
  
  // on startup we reset the time...
  RTC_TIME_IS_GOOD = false;
  RTC_ALARM_ON = false;
  // TODO:  we should reset the time also ...
  
  
  // RX_IF = FALSE;
	// RX_IE = TRUE;
  
	TMR0_IF = FALSE;
	TMR0_IE = FALSE;

	PERIPHERIC_IE = TRUE;
	GLOBAL_IE = TRUE;
  TMR4_IF = FALSE;
  TMR4_IE = TRUE;
  TMR4_ON = TRUE;
  
  
}







static void init_IO_PORTS(void){




#if USE_DEVICE_DRIVER

	IO_Init();


// 10072023 --> real PCB

#elif 0

	
	LCDCONbits.LCDEN = 0;	// that should be false anyway after reset but well...


	

	LATA = 0x40;
	LATB = 0x00;
	LATC = 0x04;




  /* TODO Initialize User Ports/Peripherals/Project here */




  /* Iniciar configuración de puertos */
//  TRISAbits.TRISA0 = 0; // A0 - Salida: Conectado a IOSYNC del AD9954
//  TRISAbits.TRISA1 = 0; // A1 - Salida: Conectado a SDIO del AD9954
//  TRISAbits.TRISA2 = 0; // A2 - Salida: Conectado a SCLK del AD9954
//  TRISAbits.TRISA3 = 1; // A3 - Entrada analógica: LDR para luces nocturnas
//  TRISAbits.TRISA4 = 0; // A4 - Salida: Enciende/apaga el AD9954 (DVDD_I/O + reguladores)
//  TRISAbits.TRISA5 = 1; // A5 - Entrada analógica: Nivel de batería
//  TRISAbits.TRISA6 = 0; // A6 - Salida: Conectado a EN_0_REG_EMISORA
//  TRISAbits.TRISA7 = 0; // A7 - Salida: Conectado al MOSFET que controla las luces nocturnas
  TRISA = 0b00101000;

//  TRISBbits.TRISB0 = 1; // B0 - Entrada: Conectado a interruptor de posición
//  TRISBbits.TRISB1 = 1; // B1 - Entrada: Conectado a entrada de puerto Infrarrojos  (no implementado)
//  TRISBbits.TRISB2 = 0; // B2 - Salida: Conectado a PS0 del AD9954
//  TRISBbits.TRISB3 = 0; // B3 - Salida: Conectado a PS1 del AD9954
//  TRISBbits.TRISB4 = 0; // B4 - Salida: Conectado a I/O UPDATE del AD9954
//  TRISBbits.TRISB5 = 1; // B5 - Entrada: No conectado (Valor por defecto)
//  TRISBbits.TRISB6 = 1; // B6 - Entrada: Conectado a ICSP - CLK (Valor por defecto)
//  TRISBbits.TRISB7 = 1; // B7 - Entrada: Conectado a ICSP - DAT (Valor por defecto)
  TRISB = 0b00100011;
  WPUB = 0b00000000; // Resistencias internas Pullup desactivadas

//  TRISCbits.TRISC0 = 1; // C0 - Entrada: No conectado (Valor por defecto)
//  TRISCbits.TRISC1 = 1; // C1 - Entrada: Conectado a termómetro one-wire ds18b20 (no implementado)
//  TRISCbits.TRISC2 = 0; // C2 - Salida: Conectado a reset del AD9954
//  TRISCbits.TRISC3 = 1; // C3 - Salida: DB_LED --> blink blink	No conectado (Valor por defecto)
//  TRISCbits.TRISC4 = 0; // C4 - Salida: LED usado en la placa de pruebas para DEBUG
//  TRISCbits.TRISC5 = 0; // C5 - Salida: Enciende/Apaga el GPS
//  TRISCbits.TRISC6 = 1; // C6 - Entrada: RS-232 pin TX (Configuración por defecto)
//  TRISCbits.TRISC7 = 1; // C7 - Entrada: RS-232 pin RX (Configuración por defecto)
  TRISC = 0b11000011;


  ANSELA = 0b00101000;
  ANSELB = 0x00;

#else



	ANSELA = 40u; // 0x03;	
	ANSELB = 0x00;	
	// ANSELC = 0x00;

	LATA = 0x00;
	LATB = 0x00;
	LATC = 0x04;



	TRISA = 40u;  // 0x1F;
	TRISB = 1u; // 0x00;
	TRISC = 128u; // 0x80;


	
  
  
  
	WPUB = 0b00000000;


#endif

}


// OBSOLETE

#if 0


// from_here_1
  UWT("LAT: ");
  UART_int(LATA);
  UART_CRLF;
  
  UART_int(LATB);
  UART_CRLF;
  
  UART_int(LATC);
  UART_CRLF;
  
   UWT("TRIS: ");
  UART_int(TRISA);
  UART_CRLF;
  
  UART_int(TRISB);
  UART_CRLF;
  
  UART_int(TRISC);
  UART_CRLF;
  
  
  UWT("ANSEL: ");
  UART_int(ANSELA);
  UART_CRLF;
  
  UART_int(ANSELB);
  UART_CRLF;
// to_here_1


// from_here_2
// to_here_2

#endif


// EOF

