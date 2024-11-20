// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //

#include "e_rtc.h"

#include "Global.h"

#include "extension_strings.h"

#include "UART.h"

#include "pwm_luz.h"

#include "io_port_sfr_names.h"

#include "handlers.h"

//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

typedef struct udt_my_time {
  
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  
  
}my_time_t;

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

  eRTC_second_cnt = 59100u;
  
  tmr4_of_cnt = (uint8_t)10u;

}



void eRTC_clock_sync_to_gps(uint32_t gps_time){

  bool temp_GIE = GLOBAL_IE;

	GIE = false;

  eRTC_second_cnt = gps_time;    
  
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
  uint8_t who_is_transmitting_now;
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

}

#endif


#if 0

void CalculaTiempoSiguienteTransmision(void){
  
  uint16_t t_entre_transmisiones;
  uint32_t hora_captura = 0u;
  uint16_t resto_division;
  int8_t quien_transmite;
  uint8_t transmission_time = get_transmit_time();
  uint8_t max_detectores = get_max_detectores();
  uint8_t det_number = get_detector_number();
  //*****Dieter//
  // if(gHayHoraUTC)
	// {

//  t_duracion_transmision = gTiempoSincronismo + gTiempoDatos + gTiempoSeguridad;
#if 0
      t_entre_transmisiones = max_detectores * transmission_time;
      
      
      
      hora_captura  = gTramaRmc.UtcOfPosition.Horas * 3600ul;
      hora_captura += gTramaRmc.UtcOfPosition.Minutos * 60;
      hora_captura += gTramaRmc.UtcOfPosition.Segundos + ( 15 - transmission_time );  //15 Dieter
#endif
      
      hora_captura = eRTC_get_second_cnt();
      hora_captura = hora_captura + ( 15 - gd.transmit_time );
    //  hora_captura += gTramaRmc.UtcOfPosition.Segundos + 5;
      resto_division = hora_captura % gd.time_between_tx;
      quien_transmite = ( resto_division / gd.transmit_time );
      quien_transmite = ( quien_transmite % max_detectores ) + 1;
      int8_t cuantas_balizas = det_number - ( quien_transmite % max_detectores + 1 );
      uint8_t cualquier_baliza_transmite = gd.transmit_time - ( resto_division % gd.transmit_time );
     

			gd.seconds_until_next_tx = cuantas_balizas * gd.transmit_time + cualquier_baliza_transmite;
			
			if ( cuantas_balizas < 0 )
      {
        gd.seconds_until_next_tx = gd.seconds_until_next_tx + t_entre_transmisiones;
      }
  // }
  
}




#if 0 //7 for refernece pruposes..
void CalculaTiempoSiguienteTransmision(void)
{
  uint16_t t_entre_transmisiones;
//  uint8_t t_duracion_transmision;
  uint32_t hora_captura = 0;
  uint16_t resto_division;
  int8_t quien_transmite;
  //*****Dieter//
  if(gHayHoraUTC)
	{

//  t_duracion_transmision = gTiempoSincronismo + gTiempoDatos + gTiempoSeguridad;
      t_entre_transmisiones = gTotalBalizas * gTiempoDuracionTransmision;
   //****Dieter***
      
      hora_captura  = gTramaRmc.UtcOfPosition.Horas * 3600ul;
      hora_captura += gTramaRmc.UtcOfPosition.Minutos * 60;
      hora_captura += gTramaRmc.UtcOfPosition.Segundos + ( 15 - gTiempoDuracionTransmision );  //15 Dieter
    //  hora_captura += gTramaRmc.UtcOfPosition.Segundos + 5;
      resto_division = hora_captura % t_entre_transmisiones;
      quien_transmite = ( resto_division / gTiempoDuracionTransmision );
      quien_transmite = ( quien_transmite % gTotalBalizas ) + 1;
      int8_t cuantas_balizas = gNumeroDeBaliza - ( quien_transmite % gTotalBalizas + 1 );
      uint8_t cualquier_baliza_transmite = gTiempoDuracionTransmision - ( resto_division % gTiempoDuracionTransmision );
     
// 11 words saved		 
#if 1

			gSegundosHastaLaSiguienteTransmision = cuantas_balizas * gTiempoDuracionTransmision + cualquier_baliza_transmite;
			
			if ( cuantas_balizas < 0 )
      {
        gSegundosHastaLaSiguienteTransmision = gSegundosHastaLaSiguienteTransmision + t_entre_transmisiones;
      }
			
#else
	
			
			
			if ( cuantas_balizas < 0 )
      {
        gSegundosHastaLaSiguienteTransmision = t_entre_transmisiones + cuantas_balizas * gTiempoDuracionTransmision + cualquier_baliza_transmite;
      }
      else
      {
        gSegundosHastaLaSiguienteTransmision = cuantas_balizas * gTiempoDuracionTransmision + cualquier_baliza_transmite;
      }

#endif			
			
  }
}

#endif

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