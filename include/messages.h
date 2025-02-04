#ifndef MESSAGES_H
#define MESSAGES_H

#include "Global.h"

#define USE_NEW_VERSION_ID 1
#define OV_VERSION_ID 1




// when set we send the amount of recharged times baterie on activation...
#define TRY_BATCHARGE_ACTIVATION 0

#define SEND_NEW_VERSION_NUMBER 1








void calculate_version_number(void);

int get_month_index(void);






typedef enum message_type{
  
  e_Activation,             // msg_activation --> activation and receiving something usable
  e_send_position,          // msg_position   --> position got locked            // PreparaMensajePosicion
  e_resend_position,        // msg_position   --> position not found resend last one            // PreparaMensajePosicion  
  e_No_gps,                 // msg_no_gps     --> not receiving nothing            // PreparaMensajeNoHayGps
  e_NUM_MSG,

}msg_t;

void set_message_for_tx(msg_t next_msg);

void messages_before_transmission(void);


















































#endif //MESSAGES_H