// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// the init routines for all the necessary periferals 
// because we want a good random function we initialize early the TMR2_ON// and let it 
// run freely so that when the Rx line goes high for the first time we use the accumulated TMR2 value as seed
// 

#include "Init_all.h"

#include "Global.h"

#include "UART.h"

#include "Clock.h"

#include "handlers.h"

#include "timers.h"

#include "io_port_sfr_names.h"

#include "device_driver_config.h"

#include "ADC.h"

#include "generic_union_flgs.h"

#include "tilt_sensor.h"

#include "xc.h"


#define DEBUG_FOTOTRANSISTOR 0

#define DEBUG_DEVICE_DRIVER_CONFIG 0

#define MHZ32_TEST 0

static uint8_t temp_tilt_sensor_tester(void);
static void next_clock(void);




static void init_wdt(void);
static void init_IO_PORTS(void);
static void init_tmr0(void);
static void init_tmr1(void);
static void configure_tmr2(void);


uint8_t t_status __at(0x16F); // (0xA0);

uint8_t fs_clock = 1;


#if 1

void init_all(void){

#if DEBUGGING_IS_ON	
  uint8_t db_cnt = 0;
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
  
	configure_tmr4();
		
	init_uart_flags();
	
	init_UART();
	
	init_IO_PORTS();
  
  tilt_sensor_init();
  
  
  
  
#if DEBUGGING_IS_ON&&0

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
  

  
#endif  


	init_handler_flg();

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
 

	RX_IF = false;
	RX_IE = true;
  
	TMR0_IF = false;
	TMR0_IE = true;


	PERIPHERIC_IE = true;
	GLOBAL_IE = true;


  TMR4_IF = false;
  TMR4_IE = true;
  TMR4ON = true;
  



  



	
}




#elif 1



void init_all(void){
	
  uint8_t db_cnt = 0;
  uint8_t last_state = 0;
  
  
  CLRWDT();

	// set_clock_speed(FAST_CLOCK_OSC);
  init_clock();

// datasheet --> switching to the PLL can take +- 2ms --> 
	__delay_ms(5);

	init_uart_flags();
	
	init_UART();
	
	init_IO_PORTS();
  


	init_handler_flg();


	UWT("BUILD: ");
	UWT(__DATE__);
	UWT("  ");
	UWT(__TIME__);
	UWT("\r\n");
	UWT("MIPS: ");
	UART_int(MIPS);
	UART_CRLF;
 
  while(1)
  {
    
    
    if(last_state != temp_tilt_sensor_tester())
    {
      next_clock();
      last_state = !last_state;
    }
    
    
    CLRWDT();
    DB_LED_2 = !DB_LED_2;
    
    
    
  }

	
}


static void next_clock(void){
  
  static uint8_t speed = 1;
  
  if(fs_clock)
  {
    UWT("\r\nNext clock: ");
    UART_int(speed);
    __delay_ms(500);
    
    speed = clock_slowdown();
  }
  else
  {
    init_clock();
    // init_clock_2();
    // _delay_ms(50);
    __delay_ms(500);
  }
  
  fs_clock = !fs_clock;
  
  
}




static uint8_t temp_tilt_sensor_tester(void){
  
  static uint8_t state_cnt = 125;
  uint8_t ret_val = 0;
  
  uint8_t state = TILT_SENSOR;
  
  if(state == 1u)
  {
    state_cnt++;
    if(state_cnt > 250)
    {
      ret_val = 1u;
      state_cnt = 250;
    }
  }
  else
  {
    state_cnt--;
    if(state_cnt < 1)
    {
      ret_val = 0u;
      state_cnt = 1;
    }
  }

  
  return ret_val;
  
}


#else
  

void init_all(void){
	
  uint8_t db_cnt = 0;

  CLRWDT();

	set_clock_speed(FAST_CLOCK_OSC);
  // init_clock();

// datasheet --> switching to the PLL can take +- 2ms --> 
	__delay_ms(5);

  DB_LED_1 = false; // just to make sure...
	
	init_tmr0();
	
	init_tmr1();
	
	configure_tmr2();
	
	configure_tmr4();
		
	init_uart_flags();
	
	init_UART();
	
	init_IO_PORTS();
  


	init_handler_flg();


	UWT("BUILD: ");
	UWT(__DATE__);
	UWT("  ");
	UWT(__TIME__);
	UWT("\r\n");
	UWT("MIPS: ");
	UART_int(MIPS);
	UART_CRLF;
 
	
	RX_IF = false;
	RX_IE = true;
	TMR0_IF = false;
	TMR0_IE = true;
  

	PERIPHERIC_IE = true;
	GLOBAL_IE = true;
	



#if 1
  
#if MHZ32_TEST     
  UWT("Fast clock\r\n");
#else  
  UWT("Slow clock\r\n");
  // checking the different speeds...
  set_clock_speed(SLOW_CLOCK_OSC);
#endif  




  FVRCONbits.FVREN = 0;
  FVRCONbits.ADFVR = 0;
  
  // RCSTAbits.CREN = false;
	// TXSTAbits.TXEN = false;
  
  // RCSTAbits.SPEN = false; // serial port disabled

  GLOBAL_IE = false;
  
  TMR0_IE = true;
  
  TMR4_ON = true;
  
  init_wdt();

  CLRWDT();

#if 1

  DB_LED_1 = !DB_LED_1;

  while(db_cnt < 2)
  {
    SLEEP();
    DB_LED_2 = !DB_LED_2;
    db_cnt++;
  }
  
  DB_LED_1 = !DB_LED_1;
  CLRWDT();
#endif  
  // init_wdt();
  
  GLOBAL_IE = false;
  
	while(db_cnt < 200)
	{
    
    
#if MHZ32_TEST   

    UWT("RND\r\n");
    while(TMR4_IF == false);
    DB_LED_1 = !DB_LED_1;
    SLEEP();
    db_cnt++;
    if(db_cnt == 50)
    {
      
      db_cnt = 0;
      CLRWDT();
      SLEEP();
      DB_LED_2 = !DB_LED_2;
    }
    TMR4_IF = false;
    CLRWDT();    
    
#else // 31.25kHz testing
  
    while(TMR4_IF == false)
    {
      CLRWDT();
    }
    
    TMR4_IF = false;
    
    DB_LED_1 = !DB_LED_1;
    
    CLRWDT();
    SLEEP();
    DB_LED_2 = !DB_LED_2;
    
#endif  
    db_cnt++;
  }
  
  
#elif 1  
	while(1)
	{
    
    
    
    
    
    DB_LED_1 = true;   
    __delay_ms(200);
    DB_LED_1 = false;
   
    
    
    
    IO_Set_channel(IO_DB_LED_2);
    __delay_ms(200);
    IO_Clear_channel(IO_DB_LED_2);
    __delay_ms(200);



    DB_LED_1 = true;   
    __delay_ms(200);
    DB_LED_1 = false;

	
    // CLRWDT();
    UWT("Status transfered var: ");
    t_status = t_status & 0x18;
    UART_int(t_status);
  
  
  }
  
#endif	
  
#if 1  

	
  init_wdt();
   
  CLRWDT();
  
  UWT("WDT_test\r\n");
  
#endif

#if 0  

  FVRCONbits.FVREN = 0;
  FVRCONbits.ADFVR = 0;
  
  RCSTAbits.CREN = false;
	TXSTAbits.TXEN = false;
  GIE = false;
  

  

  while(1)
  {
    SLEEP();

  }
 #endif 
  



	
}



#endif

static void init_wdt(void){
	
	// TODO --> check WDT overflow time
	WDTCONbits.WDTPS = 0x0B;  // 0x0c = 4seconds
  
#if IS_RELEASE	

	WDTCONbits.SWDTEN = 1;
  
#else
  
	WDTCONbits.SWDTEN = 1;
  
#endif	
	
	
	
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

	TRISA = 40u;  // 0x1F;
	TRISB = 1u; // 0x00;
	TRISC = 128u; // 0x80;

	LATA = 0x00;
	LATB = 0x00;
	LATC = 0x04;
	
	WPUB = 0b00000000;


#endif

}


static void init_tmr0(void){



	TMR0_IE = false;
	TMR0_IF = false;


	OPTION_REG = 0x80;
	OPTION_REGbits.PS = TMR0_004_PRESCALER;
  

	

}


static void init_tmr1(void){
	
	// T1CONbits.T1CKPS = 0x03;	// 8:1 Prescaler
	T1_PRESCALER = TMR1_8_PRESCALER;
	
	
}


static void configure_tmr2(void){
  
	
	
  TMR2ON = false;
	
#if 0	

	T2_PRESCALER = TMR2_64_PRESCALER;
	T2_POSTSCALER = TMR2_15_POSTSCALER;
	PR2 = 250;	

#else	
	
#if MIPS==1
	
	T2_PRESCALER = TMR2_04_PRESCALER;
	T2_POSTSCALER = TMR2_15_POSTSCALER;	// 0x0E;	// 0b0111;	// T2CONbits.T2OUTPS = 0b0111; // PostScaler 7  T2CONbits.TOUTPS = 0b0111; // PostScaler 7	
	PR2 = 250;	//125;  // 250; // thereforefor 150ms we need a counter to 6 --> for halfbit cnt to 3

#elif MIPS==2

MISSING

#elif MIPS==4

MISSING

#elif MIPS==8
	
	T2_PRESCALER = TMR2_64_PRESCALER;
	T2_POSTSCALER = TMR2_15_POSTSCALER;
	PR2 = 125;	

#else	
	
error again --> that MIPS is not standard so far --> write it extra out

#endif

#endif

}



#if 0

void configure_tmr4(void){
  
	
	
  TMR4ON = false;
	
  
  if(FAST_CLOCK == true)
  {
    
    T4_PRESCALER = TMR4_64_PRESCALER;
    T4_POSTSCALER = TMR4_10_POSTSCALER;
    PR4 = 250;	
    tmr4_200ms_of = 10;
    // with a cnt to 50 for 1000ms
  }
  else
  {
    T4_PRESCALER = TMR2_01_PRESCALER;
    T4_POSTSCALER = TMR2_11_POSTSCALER;
    PR4 = 142;	
    tmr4_200ms_of = 1;
    // directly to 200ms
  }


}


// f.e.
// 8 MIPS and 200ms 
    // T4_PRESCALER = TMR4_64_PRESCALER;
    // T4_POSTSCALER = TMR4_10_POSTSCALER;
    // PR4 = 250;	
    // cnt = 10;
    
// 4 MIPS and 200ms
    // T4_PRESCALER = TMR4_64_PRESCALER;
    // T4_POSTSCALER = TMR4_10_POSTSCALER;
    // PR4 = 250;	
    // cnt = 5;

// 2 MIPS and 200ms
    // T4_PRESCALER = TMR4_64_PRESCALER;
    // T4_POSTSCALER = TMR4_10_POSTSCALER;
    // PR4 = 125;	
    // cnt = 5;    
    
// 1 MIPS and 200ms
    // T4_PRESCALER = TMR4_16_PRESCALER;
    // T4_POSTSCALER = TMR4_10_POSTSCALER;
    // PR4 = 125;	
    // cnt = 10;        
    
// 31.25kHz Clock and 200ms
    // T4_PRESCALER = TMR4_01_PRESCALER;
    // T4_POSTSCALER = TMR4_11_POSTSCALER;
    // PR4 = 142;	
    // cnt = 1;      
    


#endif


// EOF

