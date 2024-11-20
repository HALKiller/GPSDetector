#ifndef MESSAGES_H
#define MESSAGES_H

#include "Global.h"



typedef enum message_type{
  
  e_Activation,             // msg_activation
  e_send_position,          // msg_position               // PreparaMensajePosicion
  e_No_gps,                 // msg_no_gps                 // PreparaMensajeNoHayGps
  e_No_position_no_time,    // msg_no_position_no_time    //PreparaMensajeNoHayPosNiHora
  e_No_gps_reception,       // msg_no_gps_reception       // PreparaMensajeFueraDeCobertura
  e_Gps_searches_position,  // msg_gps_searches_position  // PreparaMensajeBuscandoGps
  e_Low_baterie,            // msg_low_baterie            // PreparaMensajeBateriaBaja
  
  
}msg_t;


void messages_before_transmission(msg_t msg_id);


















































#endif //MESSAGES_H