// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //

#include "e_rtc.h"

#include "Global.h"

#include "extension_strings.h"

#include "UART.h"

#include "io_port_sfr_names.h"

//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

typedef struct udt_my_time {
  
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  
  
}my_time_t;

my_time_t ertc;


volatile uint8_t tmr4_of_cnt = (uint8_t)10u;
volatile uint32_t eRTC_second_cnt = 0u;

//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 

//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 



//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //


//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //



//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

void eRTC_clock_incrementer(void){
  
  eRTC_second_cnt = eRTC_second_cnt + 2u;
  
  if(eRTC_second_cnt >= SECONDS_PER_DAY)
  {
    
    eRTC_second_cnt = 0;
    
  }

}



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

uint32_t eRTC_get_second_cnt(void){
  
  bool temp_GIE = GLOBAL_IE;

	GIE = false;

  uint32_t ret_val = eRTC_second_cnt;
  
  GIE = temp_GIE;
  
  return ret_val;
  
}


//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //



#if 1

void ertc_convert_to_real_time(uint32_t in_time){
  
  uint32_t temp_timer = in_time;  // eRTC_second_cnt / 10;
  
  ertc.hours = (uint8_t)(temp_timer / SECONDS_PER_HOUR);
  
  ertc.minutes = (uint8_t)( (temp_timer - ((uint32_t)ertc.hours * SECONDS_PER_HOUR)) / MINUTES_PER_HOUR);

  ertc.seconds = (uint8_t)(temp_timer - ((uint32_t)ertc.hours * SECONDS_PER_HOUR) - (uint16_t)ertc.minutes * SECONDS_PER_MINUTE);

}


#else

void ertc_convert_to_real_time(uint32_t in_time){
  
  uint32_t temp_timer = in_time;  // eRTC_second_cnt / 10;
  
  UART_int(temp_timer);
  DB_PRINT("  ");
  
  ertc.hours = (uint8_t)(temp_timer / SECONDS_PER_HOUR);
  
  UART_int(ertc.hours);
  DB_PRINT("  ");
  
  temp_timer = temp_timer - (uint32_t)ertc.hours * SECONDS_PER_HOUR;
  UART_int(temp_timer);
  DB_PRINT("  ");
  
  
  ertc.minutes = (uint8_t)(temp_timer / MINUTES_PER_HOUR);
  
  
  UART_int(ertc.minutes);
  DB_PRINT("  ");
  
  temp_timer = temp_timer - (uint16_t)ertc.minutes * SECONDS_PER_MINUTE;
  UART_int(temp_timer);
  DB_PRINT("  ");
  
  
  
  ertc.seconds = temp_timer;  // (uint8_t)(temp_timer - (ertc.hours * SECONDS_PER_HOUR) - ertc.minutes * SECONDS_PER_MINUTE);
  
  UART_int(ertc.seconds);
  DB_PRINT("  ");
  
  
}





#endif


void ertc_convert_to_str(void){
  
  
  my_time_t *const the_time = &ertc;
  uint8_t time_str[11];
#if 1

  DecimalUint8ToA(time_str, the_time->hours, 2, false );
  
  time_str[2] = ':';
  time_str[5] = ':';
  
  DecimalUint8ToA(time_str + 3, the_time->minutes, 2, false );
  DecimalUint8ToA(time_str + 6, the_time->seconds, 2, true );

#else
  
  DecimalUint8ToA(time_str, the_time->hours, 2, false );
  DecimalUint8ToA(time_str + 2, the_time->minutes, 2, false );
  DecimalUint8ToA(time_str + 4, the_time->seconds, 2, true );
 
#endif 
  
  DB_PRINT(time_str);
  DB_PRINT("\r\n");
  
  
}



// EOF