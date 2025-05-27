// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //

#include "e_rtc.h"

#include "Global.h"

#include "extension_strings.h"

#include "UART.h"

#include "detector.h"

#include "io_port_sfr_names.h"

#include "handlers.h"

#include "generic_union_flgs.h"


// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  
#ifndef FILE_E_RTC_DB_ENABLED
#define FILE_E_RTC_DB_ENABLED 0
#endif
#if FILE_E_RTC_DB_ENABLED
#define DB_PRINT(str) G_DB_PRINT(str)
#define UART_int(var) G_UART_INT(var)
#else
#define DB_PRINT(str)
#define UART_int(var)
#endif
// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  










//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //


my_time_t ertc;


volatile uint8_t tmr4_of_cnt = (uint8_t)10u;
volatile static uint32_t eRTC_second_cnt = 0u;
volatile static uint8_t rtc_decimo_cnt = 0;
//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 

//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 



//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //


//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //



//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

#if USE_FULL_SECONDS_FOR_RTC
// for only full second handling...

// uh--> that is getting called from the ISR!!!
void eRTC_clock_incrementer(void){
  
  rtc_decimo_cnt++;
  
  if(rtc_decimo_cnt >= 5)
  {
    rtc_decimo_cnt = 0u;
    
    handlers_generic_set_handler_FLG(e_1000ms_h);
    
    eRTC_second_cnt++;
    
    if(eRTC_second_cnt >= SECONDS_PER_DAY)
    {
      
      eRTC_second_cnt = 0;
      
    }
  }
  
}

#else
  
void eRTC_clock_incrementer(void){
  
  eRTC_second_cnt = eRTC_second_cnt + 2u;
  
  if(eRTC_second_cnt >= SECONDS_PER_DAY)
  {
    
    eRTC_second_cnt = 0;
    
  }

}

#endif

#if USE_FULL_SECONDS_FOR_RTC

// TODO: reset value here
void eRTC_clock_reset(void){

  eRTC_second_cnt = 0u; // 59100u;
  
  tmr4_of_cnt = (uint8_t)10u;

}



void eRTC_clock_sync_to_gps(uint32_t gps_time){

  bool temp_GIE = GLOBAL_IE;

	GIE = false;

  eRTC_second_cnt = gps_time;    
  
  RTC_TIME_IS_GOOD = true;
  
  rtc_decimo_cnt = 0u;
  
  GIE = temp_GIE;
  
  
}

#else

// TODO: reset value here
void eRTC_clock_reset(void){

  eRTC_second_cnt = 591000u;
  
  tmr4_of_cnt = (uint8_t)10u;

}
  
void eRTC_clock_sync_to_gps(uint32_t gps_time){

  bool temp_GIE = GLOBAL_IE;

	GIE = false;

  eRTC_second_cnt = gps_time * 10u;    
  
  GIE = temp_GIE;
  
  
}

#endif

uint32_t eRTC_get_second_cnt(void){
  
  bool temp_GIE = GLOBAL_IE;

	GIE = false;

  uint32_t ret_val = eRTC_second_cnt;
  
  GIE = temp_GIE;
  
  return ret_val;
  
}

#if 1 //7 for refernece pruposes..

void eRTC_calculate_time_until_tx(void)
{

  uint32_t time_now = 0;
  uint16_t resto_division;
  int8_t who_is_transmitting_now;
  int8_t cuantas_balizas;
  uint8_t cualquier_baliza_transmite;


  time_now = eRTC_get_second_cnt();
  
  time_now = time_now + ( 15 - gd.transmission_duration );  //15 Dieter
  
  resto_division = time_now % gd.time_between_tx;
  who_is_transmitting_now = ( resto_division / gd.transmission_duration );
  who_is_transmitting_now = ( who_is_transmitting_now % gd.max_detectores ) + 1;
  cuantas_balizas = (int8_t)gd.number - (int8_t)( who_is_transmitting_now % gd.max_detectores + 1 );
  cualquier_baliza_transmite = gd.transmission_duration - ( resto_division % gd.transmission_duration );

  gd.seconds_until_next_tx = cuantas_balizas * gd.transmission_duration + cualquier_baliza_transmite;
  

  
  if ( cuantas_balizas < 0 )
  {
    
    gd.seconds_until_next_tx = gd.seconds_until_next_tx + gd.time_between_tx;
    
  }

#if 1  

  gd.next_time_tx = gd.seconds_until_next_tx + time_now;
  
  if(gd.next_time_tx > SECONDS_PER_DAY)
  {
    gd.next_time_tx = gd.next_time_tx - SECONDS_PER_DAY;
  }
 
#if (DEBUGGING_IS_ON||DEBUGGING_BB_IS_ON)&&0 
  ertc_convert_to_real_time(gd.next_time_tx);
  DB_PRINT("N_tx: ");
  ertc_convert_to_str();
#endif  
  
#endif
  


}

#endif





//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //



void ertc_convert_to_real_time(uint32_t in_time){
  
  uint32_t temp_timer = in_time;  // eRTC_second_cnt / 10;
  
  ertc.hours = (uint8_t)(temp_timer / SECONDS_PER_HOUR);
  
  ertc.minutes = (uint8_t)( (temp_timer - ((uint32_t)ertc.hours * SECONDS_PER_HOUR)) / MINUTES_PER_HOUR);

  ertc.seconds = (uint8_t)(temp_timer - ((uint32_t)ertc.hours * SECONDS_PER_HOUR) - (uint16_t)ertc.minutes * SECONDS_PER_MINUTE);

}



void ertc_convert_to_str(void){
  
  
  my_time_t *const the_time = &ertc;
  
  uint8_t time_str[11];


  DecimalUint8ToA(time_str, the_time->hours, 2, false );
  
  time_str[2] = ':';
  time_str[5] = ':';
  
  DecimalUint8ToA(time_str + 3, the_time->minutes, 2, false );
  DecimalUint8ToA(time_str + 6, the_time->seconds, 2, true );


 
  DB_PRINT(time_str);
  DB_PRINT("\r\n");
  
  
}



// EOF