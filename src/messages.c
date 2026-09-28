// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#line 6 "messages.c"

//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //
#include "messages.h"

#include "Global.h"

#include "gps_extensions.h"

#include "eeprom.h"

#include "extension_strings.h"

#include "detector.h"  // for the baterie adc measurement

#include "DDS.h"

#include "my_assert.h"

#include "e_rtc.h"

#include "generic_union_flgs.h" // for the BAT_LOW_FLG

#include "UART.h"

#include "io_port_sfr_names.h"

#include <string.h>

// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  
#ifndef FILE_MESSAGES_DB_ENABLED
#define FILE_MESSAGES_DB_ENABLED 0
#endif
#if FILE_MESSAGES_DB_ENABLED
#define DB_PRINT(str) G_DB_PRINT(str)
#define UART_int(var) G_UART_INT(var)
#else
#define DB_PRINT(str)
#define UART_int(var)
#endif
// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  

//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 

// #define USE_NEW_VERSION_ID 0


//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //





//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 

 
#if SEND_NEW_VERSION_NUMBER
 
const char gMensajeActivandose_new  [] = "ACTIVANDOSE ";

#if 1

const char gMensajeVersion_new[] = "V>" FW_VERSION_STR "<" LETTER_REPLACER ">" PCB_V_STRING "< C ";
// const char gMensajeVersion_new[] = "V>" FW_VERSION_STR "<" LETTER_REPLACER ">69< C "; that works

// const char gMensajeVersion_new[] = "V>" FW_VERSION_STR "<" LETTER_REPLACER ">" PCB_V_STRING "< C ";

#else
  
const char gMensajeVersion_new[] = "V>24<" LETTER_REPLACER ">69< C ";

// const char gMensajeVersion_new[] = "V>24<" LETTER_REPLACER ">69< C ";


#endif

// const char gMensajeVersion_new      [] = "V>25<R>01< C "; 


#endif 
 
 
 
#if 1

const char gMensajeActivandose  [] = "ACTIVANDOSE ";
#if 1
// const char msg_gps_error        [] = "ANFO ERDOR >";
const char msg_gps_error        [] = "NOTICE GPS >";
#else
const char msg_gps_error        [] = "FALLO MODULO GPS >-< ERROR >";
#endif


#if USE_NEW_VERSION_ID



#else
  
#if PIC_16F1936
const char gMensajeVersion      [] = "V>0267< ";
#else
const char gMensajeVersion      [] = "V>xx67< ";
#endif

#endif

const char Hex_char[] = "ABCD";

#else
  
const uint8_t gMensajeActivandose  [] = "ACTIVANDOSE ";
const uint8_t gMensajeRadiogonio   [] = "RADIOGONIO ";
const uint8_t gMensajeNoHay        [] = "NO HAY ";
const uint8_t gMensajeBuscando     [] = "BUSCANDO ";
const uint8_t gMensajeNoCobertura  [] = "FUERA DE COBERTURA ";
const uint8_t gMensajeRevisar      [] = "REVISE ANTENA ";
const uint8_t gMensajeApagado      [] = "APAGADO DURANTE XX MINUTOS";
const uint8_t gMensajeBateriaBaja  [] = "BATERIA BAJA ";
const uint8_t gMensajeLucesActivas [] = "LUCES ACTIVADAS ";
const uint8_t gMensajePos          [] = "POS ";
const uint8_t gMensajeGps          [] = "GPS ";
const uint8_t gMensajeGuion        [] = ">-< ";
const uint8_t gMensajeHora         [] = "HORA ";
#if PIC_16F1936
const uint8_t gMensajeVersion      [] = "V>0267< ";
#else
const uint8_t gMensajeVersion      [] = "V>xx67< ";
#endif

#endif



 
 

//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //

static msg_t msg_id;

static uint16_t version_nr = 0;
#if 0
static char version_char[8];
#endif
static uint16_t bat_cnt = 702; // DDC

//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

static void msg_activation(void);
static void msg_position(uint8_t resend);
static void msg_no_gps(void);


static void insert_msg_header(uint8_t invert);
static void insert_bat_error(uint8_t slot);
static void insert_baterie(void);
static void insert_bat_charge_count(void);
static void insert_time(uint8_t pos);
#if USE_NEW_VERSION_ID
static void insert_version(uint8_t * buf);

static void insert_batcharged_cnt(uint8_t d_pnt);

#endif

//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

void set_message_for_tx(msg_t next_msg){
  
  msg_id = next_msg;
  
}

// it is much easier to enter a single call to here and have an identifier which gives us the message to produce...
void messages_before_transmission(void){

  DDS_flush_buffer();
  
  switch(msg_id)
  {

    case e_Activation:
      msg_activation();
    break;
    case e_send_position:
      msg_position(false);
    break;
    case e_resend_position:
      msg_position(true);
    break;
    case e_No_gps:
      msg_no_gps();
    break;

    default:
    
      assert(false);
      
    break;
  }

  

}


//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //

#if SEND_NEW_VERSION_NUMBER 
// e_Activation
static void msg_activation(void){

  uint8_t d_pnt = 0;

  insert_msg_header(false);
  
  strcpy ( (char*)(sentence_buffer.gps_buffer + 23u), gMensajeActivandose_new );
  
  d_pnt = gps_buffer_get_len();
  
  strcpy ( (char*)(sentence_buffer.gps_buffer + d_pnt), gMensajeVersion_new );

  insert_bat_charge_count();

  d_pnt = gps_buffer_get_len();
  
  sentence_buffer.gps_buffer[d_pnt] = ' ';
  
  d_pnt++;
  
  sentence_buffer.gps_buffer[d_pnt] = ' ';

  
}


#endif



// e_send_position
static void msg_position(uint8_t resend)
{

  insert_msg_header(resend);
  
  sentence_buffer.gps_buffer[34] =  '>';
  sentence_buffer.gps_buffer[50] =  '>';
  sentence_buffer.gps_buffer[60] =  '>';
  sentence_buffer.gps_buffer[22] =  ',';
  sentence_buffer.gps_buffer[33] =  ',';
  sentence_buffer.gps_buffer[37] =  ',';
  sentence_buffer.gps_buffer[49] =  ',';
  sentence_buffer.gps_buffer[53] =  ',';
  sentence_buffer.gps_buffer[36] =  '<';
  sentence_buffer.gps_buffer[52] =  '<';
  sentence_buffer.gps_buffer[28] =  '.';
  sentence_buffer.gps_buffer[44] =  '.';
  sentence_buffer.gps_buffer[25] =  ':';
  sentence_buffer.gps_buffer[41] =  ':';

  sentence_buffer.gps_buffer[61] =  'H';


// TODO: 
// optimize that in the way that the enum value is straight out the char we need
// --> optimizing the usage of RAM and ROM


  sentence_buffer.gps_buffer[35] = 'N';
  
  if ( rmc_sentence.LatiDirection == eSOUTH )
  {
    sentence_buffer.gps_buffer[35] = 'S';
  }

   
  sentence_buffer.gps_buffer[51] = 'E';
  
  if ( rmc_sentence.LongDirection == eWEST )
  {
    sentence_buffer.gps_buffer[51] = 'W';
  }
 
  DecimalUint8ToA ( sentence_buffer.gps_buffer + 23, rmc_sentence.Latitude.Grados        , 2, false );
  DecimalUint8ToA ( sentence_buffer.gps_buffer + 26, rmc_sentence.Latitude.Minutos       , 2, false );
  DecimalUint8ToA ( sentence_buffer.gps_buffer + 38, rmc_sentence.Longitude.Grados       , 3, false );
  DecimalUint8ToA ( sentence_buffer.gps_buffer + 42, rmc_sentence.Longitude.Minutos      , 2, false );

#if SEND_ERROR_CODES_IN_SECONDS_SLOT
  
  ertc_convert_to_real_time(eRTC_get_second_cnt());

  DecimalUint8ToA ( sentence_buffer.gps_buffer + 54, ertc.hours    , 2, false );
  DecimalUint8ToA ( sentence_buffer.gps_buffer + 56, ertc.minutes  , 2, false );
  
  sentence_buffer.gps_buffer[58] = '0';
  
  if(resend == true)
  {
    sentence_buffer.gps_buffer[58] = '1';
  }

#if 1

  insert_bat_error(59u);
  
#else
  
  sentence_buffer.gps_buffer[59] = '0';
  // TODO: Get bat level:
  if(BAT_IS_LOW_FLG == true)
  {
    sentence_buffer.gps_buffer[59] = '1';
  }
  
  if(BAT_IS_TOO_LOW == true)
  {
    sentence_buffer.gps_buffer[59] = '2';
  }
  
  if(DDS_CFG_ERR == true)
  {
    sentence_buffer.gps_buffer[59] = sentence_buffer.gps_buffer[59] + 3u;
    
  }
#endif 

#else
  
  ertc_convert_to_real_time(eRTC_get_second_cnt());

  DecimalUint8ToA ( sentence_buffer.gps_buffer + 54, ertc.hours    , 2, false );
  DecimalUint8ToA ( sentence_buffer.gps_buffer + 56, ertc.minutes  , 2, false );
  DecimalUint8ToA ( sentence_buffer.gps_buffer + 58, ertc.seconds  , 2, false );

#endif

#if SEND_LOCK_TIME_DECIMAS_LATITUDE  

  uint16_t temp_var = gps_get_last_lock_time();

  DecimalUint16ToA( sentence_buffer.gps_buffer + 29, temp_var                            , 4, false );
  DecimalUint16ToA( sentence_buffer.gps_buffer + 45, rmc_sentence.Longitude.Decimas      , 4, false );


#else

  DecimalUint16ToA( sentence_buffer.gps_buffer + 29, rmc_sentence.Latitude.Decimas       , 4, false );
  DecimalUint16ToA( sentence_buffer.gps_buffer + 45, rmc_sentence.Longitude.Decimas      , 4, false );
  
#endif  
  
}



// e_No_gps
static void msg_no_gps(void){


  uint16_t temp_cnt = bat_cnt;
  
  uint8_t d_pnt = 0;

  char t_char = 0;
  
  
  insert_msg_header(false);
  
  strcpy((char*)sentence_buffer.gps_buffer + 23, msg_gps_error);

  // put in the gps error and than the same is with pos...
  d_pnt = gps_buffer_get_len();
  
  t_char = gps_get_gps_state();
  
  sentence_buffer.gps_buffer[d_pnt] = t_char + '0';
  
  // get the error from baterie and stuff...
  insert_bat_error(d_pnt + 1);

  sentence_buffer.gps_buffer[d_pnt + 2] = '<';
  sentence_buffer.gps_buffer[d_pnt + 3] = ' ';
  
  d_pnt = gps_buffer_get_len();
  
  strcpy ( (char*)(sentence_buffer.gps_buffer + d_pnt), gMensajeVersion_new );
  d_pnt = gps_buffer_get_len();
  
  insert_bat_charge_count();
  
}




static void insert_msg_header(uint8_t invert)
{
  
  // DB_PRINT("msg_header\r\n");
  
  insert_baterie();
  

  for( uint8_t i = 0; i < 4; i++ )
  {
    sentence_buffer.gps_buffer[i] =  LeerEeprom ( 0x36 + i );
  }
  
  for( uint8_t i = 0; i < 7; i++ )
  {
    sentence_buffer.gps_buffer[i + 5] =  LeerEeprom ( 0x3A + i );
  }


  sentence_buffer.gps_buffer[4] =  '>';
  sentence_buffer.gps_buffer[18] =  '>';
  sentence_buffer.gps_buffer[12] =  ',';
  sentence_buffer.gps_buffer[13] =  '<';
  sentence_buffer.gps_buffer[21] =  '<';
  sentence_buffer.gps_buffer[16] =  '.';
  sentence_buffer.gps_buffer[19] =  'V';
  sentence_buffer.gps_buffer[20] =  'B';  
  if(invert == true)
  {
    sentence_buffer.gps_buffer[19] =  'B';
    sentence_buffer.gps_buffer[20] =  'V';    
  }

  sentence_buffer.gps_buffer[22] =  ' ';
  
}

static void insert_bat_error(uint8_t slot){
  

  sentence_buffer.gps_buffer[slot] = '0';
  
  if(BAT_IS_LOW_FLG == true)
  {
    sentence_buffer.gps_buffer[slot] = '1';
  }
  
  if(BAT_IS_TOO_LOW == true)
  {
    sentence_buffer.gps_buffer[slot] = '2';
  }
  
  if(DDS_CFG_ERR == true)
  {
    sentence_buffer.gps_buffer[slot] = sentence_buffer.gps_buffer[slot] + 3u;
  }
 
  
  
}

#if USE_ADC_OVERSAMPLING

static void insert_baterie(void)
{
  
  char vbat[3];
 
  uint8_t baterie_mV = gd.bat_decivolt;
  
#if DB_V69_PCB
  baterie_mV = 120;
#endif
 
  
  DecimalUint8ToA( (uint8_t*)&vbat[0], baterie_mV, 3, false );

  sentence_buffer.gps_buffer[14] = vbat[0];
  sentence_buffer.gps_buffer[15] = vbat[1];
  sentence_buffer.gps_buffer[17] = vbat[2];
  
}

#else
  
static void insert_baterie(void)
{
  
  char vbat[3];
  
  uint8_t baterie_mV = LeerValorBateria();
  

  
  DecimalUint8ToA( (uint8_t*)&vbat[0], baterie_mV, 3, false );

  sentence_buffer.gps_buffer[14] = vbat[0];
  sentence_buffer.gps_buffer[15] = vbat[1];
  sentence_buffer.gps_buffer[17] = vbat[2];
  
}

#endif

// converts into a base_23 Alphabet 
// 0 --> AAA
// 1 --> AAB max first digit is W --> AAW overflow to ABA because there cant be an 'X' in the sentence_buffer
// because  that would be the end of the frame.
static void insert_bat_charge_count(void){
  
  uint8_t hlooper;
  uint16_t d_base = 529u; // 23 * 23
  char t_char;
  uint8_t d_pnt;
#if COMPILE_FOR_INTERNAL_TEST  
  uint16_t temp_cnt = get_txcnt();
#else
  uint16_t temp_cnt = get_batcnt();
#endif
  d_pnt = gps_buffer_get_len();
  
  for(hlooper = 0; hlooper < 3; hlooper++)
  {
    t_char = temp_cnt / d_base;  
    
    sentence_buffer.gps_buffer[d_pnt] = t_char + 'A';
    
    d_pnt++;
    
    temp_cnt = temp_cnt%d_base;
    
    d_base = d_base / 23u;
    
  }
  
}



static void insert_time(uint8_t pos)
{
  
  ertc_convert_to_real_time(eRTC_get_second_cnt());
  
  sentence_buffer.gps_buffer[pos] =  '>';
  DecimalUint8ToA ( sentence_buffer.gps_buffer + pos + 1, ertc.hours    , 2, false );
  DecimalUint8ToA ( sentence_buffer.gps_buffer + pos + 3, ertc.minutes  , 2, false );
  DecimalUint8ToA ( sentence_buffer.gps_buffer + pos + 5, ertc.seconds  , 2, false );
  sentence_buffer.gps_buffer[pos + 7] =  'H';
  
  
}




// EOF