// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

/******************************************************************************/
#line 7 "main.c"

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/


// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = SWDTEN    // Watchdog Timer Enable (WDT controlled by the SWDTEN bit in the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON      // ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable (All VCAP pin functionality is disabled)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF         // Low-Voltage Programming Enable (Low-voltage programming enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.



#include <xc.h>           

#include "Init_all.h"

#include "UART.h"

#include "generic_union_flgs.h"

#include "Global.h"

#include "handlers.h"

#include "gd_states.h"

#include "io_port_sfr_names.h"

#include "my_assert.h"



// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //
#ifndef FILE_MAIN_DB_ENABLED
#define FILE_MAIN_DB_ENABLED 0
#endif
#if FILE_MAIN_DB_ENABLED
#define DB_PRINT(str) G_DB_PRINT(str)
#define UART_int(var) G_UART_INT(var)
#else
#define DB_PRINT(str)
#define UART_int(var)
#endif
// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //   



/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

union8_t gFLAGS;
union8_t dFLAGS;


// TODO before rc --> Watch Dog timer !


/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
#if 0
void main(void)
{
  
  // TODO: the init_routine needs to get overworked becasue the rx_luz is going 
  // to be happening before anything else
  // and therefore there should not be any 
// Isr be active allready except perhaps the UART during debugging!!!
  
#if DEBUGGING_IS_ON

  const unsigned char SW_version[] = "GPSDetector_v.1.0_db\r\n";
  
#else
  
  const unsigned char SW_version[] = "GPSDetector_v.1.0_rc\r\n";
  
#endif


	init_all();

  while(1)
  {
    LED = true;
    // DB_PRINT(&SW_version[0]);
    // my_delay_ms(500);
    // LED = !LED;
    // my_delay_ms(500);
  }


  // the reset is over --> lets swap to the next gd_state...
  
  gd_states_switch_to_next_state(E_LUZ_COM_STATE);
  
	get_the_next_handler();

  // becasue we should never ever get back here 
  // --> we reset the stackptr
  assert(false);

}

#else
  /******************************************************************************/
void main(void)
{
  
  // TODO: the init_routine needs to get overworked becasue the rx_luz is going 
  // to be happening before anything else
  // and therefore there should not be any 
// Isr be active allready except perhaps the UART during debugging!!!
  
#if DEBUGGING_IS_ON||DEBUGGING_BB_IS_ON

  const unsigned char SW_version[] = "GPSD_v.1.0_db\r\n";
  
#else
  
  const unsigned char SW_version[] = "G\r\n";
  
#endif


	init_all();

  DB_PRINT(&SW_version[0]);

  // the reset is over --> lets swap to the next gd_state...
  
  gd_states_switch_to_next_state(E_LUZ_COM_STATE);
  
	get_the_next_handler();

  // becasue we should never ever get back here 
  // --> we reset the stackptr
  assert(false);

}

#endif











// EOF-