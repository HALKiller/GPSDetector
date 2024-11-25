#ifndef MESSAGES_H
#define MESSAGES_H

#include "Global.h"

#define USE_NEW_VERSION_ID 1
#define OV_VERSION_ID 1

#if USE_NEW_VERSION_ID&&0

#define PARSE_MONTH(m) ( \
    (m[0] == 'J' && m[1] == 'a') ? 0 : \
    (m[0] == 'F') ? 31 : \
    (m[0] == 'M' && m[2] == 'r') ? 60 : \
    (m[0] == 'A') ? 91 : \
    (m[0] == 'M' && m[2] == 'y') ? 121 : \
    (m[0] == 'J' && m[2] == 'n') ? 152 : \
    (m[0] == 'J' && m[2] == 'l') ? 182 : \
    (m[0] == 'A') ? 213 : \
    (m[0] == 'S') ? 244 : \
    (m[0] == 'O') ? 274 : \
    (m[0] == 'N') ? 305 : \
    (m[0] == 'D') ? 335 : -1)

#define DAY_OF_YEAR ((__DATE__[4] - '0') * 10 + (__DATE__[5] - '0') + PARSE_MONTH(__DATE__))

#define WEEK_OF_YEAR ((DAY_OF_YEAR - 1) / 7 + 1)

#define YEAR_LAST_TWO_DIGITS ((__DATE__[9] - '0') * 10 + (__DATE__[10] - '0'))

#define COMBINED_CODE (WEEK_OF_YEAR * 100 + YEAR_LAST_TWO_DIGITS)

#endif








void calculate_version_number(void);

int get_month_index(void);






typedef enum message_type{
  
  e_Activation,             // msg_activation --> activation and receiving something usable
  e_send_position,          // msg_position   --> position got locked            // PreparaMensajePosicion
  e_No_gps,                 // msg_no_gps     --> not receiving nothing            // PreparaMensajeNoHayGps
  e_No_position_no_time,    // msg_no_position_no_time --> only on startup! because of that there is always the "activation" after allready working for a while    //PreparaMensajeNoHayPosNiHora
  e_No_gps_reception,       // msg_no_gps_reception       // PreparaMensajeFueraDeCobertura
  e_Gps_searches_position,  // msg_gps_searches_position  // PreparaMensajeBuscandoGps
  e_Low_baterie,            // msg_low_baterie            // PreparaMensajeBateriaBaja
  
  
}msg_t;

void set_message_for_tx(msg_t next_msg);

void messages_before_transmission(void);


















































#endif //MESSAGES_H