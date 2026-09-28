// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com



//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //
#include "messages.h"

#include "Global.h"

#include "gps_extensions.h"

#include "eeprom.h"

#include "extension_strings.h"

#include "pwm_luz.h"  // for the baterie adc measurement

#include "DDS.h"

#if DEBUGGING_IS_ON
#include "UART.h"
#endif

#include <string.h>

//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 

// #define USE_NEW_VERSION_ID 0


//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //





//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 
const char* month_names[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
}; 
 
 
struct MonthDay {
    const char* month;
    int days;
};

struct MonthDay months[] = {
  {"Jan", 31},
  {"Feb", 29}, // Assuming non-leap year
  {"Mar", 31},
  {"Apr", 30},
  {"May", 31},
  {"Jun", 30},
  {"Jul", 31},
  {"Aug", 31},
  {"Sep", 30},
  {"Oct", 31},
  {"Nov", 30},
  {"Dec", 31}
}; 
 
#if 1

const char gMensajeActivandose  [] = "ACTIVANDOSE ";
const char gMensajeRadiogonio   [] = "RADIOGONIO ";
const char gMensajeNoHay        [] = "NO HAY ";
const char gMensajeBuscando     [] = "BUSCANDO ";
const char gMensajeNoCobertura  [] = "FUERA DE COBERTURA ";
const char gMensajeRevisar      [] = "REVISE ANTENA ";
const char gMensajeApagado      [] = "APAGADO DURANTE XX MINUTOS";
const char gMensajeBateriaBaja  [] = "BATERIA BAJA ";
const char gMensajeLucesActivas [] = "LUCES ACTIVADAS ";
const char gMensajePos          [] = "POS ";
const char gMensajeGps          [] = "GPS ";
const char gMensajeGuion        [] = ">-< ";
const char gMensajeHora         [] = "HORA ";

#if USE_NEW_VERSION_ID
const char gMensajeVersion_H      [] = "V>";
const char gMensajeVersion_T      [] = "< ";
#else
#if PIC_16F1936
const char gMensajeVersion      [] = "V>0267< ";
#else
const char gMensajeVersion      [] = "V>xx67< ";
#endif
#endif

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

//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

static void msg_activation(void);
static void msg_position(void);
static void msg_no_gps(void);
static void msg_no_position_no_time(void);
static void msg_no_gps_reception(void);
static void msg_gps_searches_position(void);
static void msg_low_baterie(void);

static void insert_msg_header(void);
static void insert_baterie(void);
static void insert_time(uint8_t pos);
#if USE_NEW_VERSION_ID
static void insert_version(uint8_t * buf);
#endif


//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

void set_message_for_tx(msg_t next_msg){
  
  msg_id = next_msg;
  
}

// it is much easier to enter a single call to here and have an identifier which gives us the message to produce...
void messages_before_transmission(void){
// void messages_before_transmission(msg_t msg_id){
  
  DDS_flush_buffer();
  
  switch(msg_id)
  {

    case e_Activation:
      msg_activation();
    break;
    case e_send_position:
      msg_position();
    break;
    case e_No_gps:
      msg_no_gps();
    break;
    case e_No_position_no_time:
      msg_no_position_no_time();
    break;
    case e_No_gps_reception:
      msg_no_gps_reception();
    break;
    case e_Gps_searches_position:
      msg_gps_searches_position();
    break;
    case e_Low_baterie:
      msg_low_baterie();
    break;
    
    
  }

}


//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //


// e_Activation
static void msg_activation(void)
{

  insert_msg_header();
  
  strcpy ( (char*)(sentence_buffer.gps_buffer + 23), gMensajeActivandose );
  
#if USE_NEW_VERSION_ID||OV_VERSION_ID

  // strncpy(date_str, __DATE__, 11);

  strcpy ( (char*)(sentence_buffer.gps_buffer + 35), gMensajeVersion_H );
  insert_version((char*)(sentence_buffer.gps_buffer + 37));
  strcpy ( (char*)(sentence_buffer.gps_buffer + 41), gMensajeVersion_T );
  
#else  
  strcpy ( (char*)(sentence_buffer.gps_buffer + 35), gMensajeVersion );
#endif
  
}



// clean up now...
#if 0


int get_month_index(void) {
  
  const char* date_str = __DATE__;
  
  char month_str[4];
  
  uint16_t day_of_year = 0u;
  
  uint8_t day_of_month = 0u;
  
  uint8_t compiled_week = 0u;
  // uint8_t compiled_year = 0u;
  int compiled_year = (date_str[9] - '0') * 10 + (date_str[10] - '0');
  
  strncpy(month_str, &date_str[0], 3);
  
  month_str[3] = '\0';
  
  DB_PRINT(&month_str);
  
  for (int i = 0; i < 12; i++)
  {
    if (strcmp(month_str, months[i].month) == 0) 
    {
      
      day_of_year = day_of_year + get_day_of_month();
      
      UART_int(day_of_year);
      
      UART_CRLF;
      
      compiled_week = day_of_year / 7u;
      
      UART_int(compiled_week);
      
      UART_CRLF;
      
      version_nr = (((uint16_t)(compiled_week)) *100)  + compiled_year;
      
      UART_int(version_nr);
      
      UART_CRLF;
      
      return i;
      
    }
    
    day_of_year = day_of_year + months[i].days;
    UART_int(day_of_year);
    UART_CRLF;
  }
  

  // assert(false); // Month not found
  return -1;
  
}


int get_day_of_month(void){
  
  const char* date_str = __DATE__;
  int day = 0;
  int i = 4;  // becasue for example Nov 15 2024


  // Handle both single-digit and double-digit days
  if (date_str[i] != ' ')
  {
    day = (date_str[i] - '0') * 10 + (date_str[i+1] - '0');
  }
  else
  {
    day = (date_str[i+1] - '0');
  }

  return day;
  
}

#else


void calculate_version_number(void) {
  
  const char* date_str = __DATE__;
  
  char month_str[4];
  
  uint16_t day_of_year = 0u;
  
  uint8_t day_of_month = 0u;
  
  uint8_t compiled_week = 0u;
  
  int compiled_year = (date_str[9] - '0') * 10 + (date_str[10] - '0');
  
  strncpy(month_str, &date_str[0], 3);
  
  month_str[3] = '\0';
  
  
  
  for (int i = 0; i < 12; i++)
  {
    if (strcmp(month_str, months[i].month) == 0u) 
    {

      if (date_str[4] != ' ')
      {
        day_of_month = (date_str[4] - '0') * 10 + (date_str[5] - '0');
      }
      else
      {
        day_of_month = (date_str[5] - '0');
      }
      
      day_of_year = day_of_year + day_of_month;

      
      compiled_week = day_of_year / 7u;
      

      version_nr = (((uint16_t)(compiled_week)) * 100)  + compiled_year;

      
    }
    
    day_of_year = day_of_year + months[i].days;
   
  }
  
  // assert(version_nr != 0u);
  
}

#if 0
int get_day_of_month(void){
  
  const char* date_str = __DATE__;
  int day = 0;
  int i = 4;  // becasue for example Nov 15 2024


  // Handle both single-digit and double-digit days
  if (date_str[i] != ' ')
  {
    day = (date_str[i] - '0') * 10 + (date_str[i+1] - '0');
  }
  else
  {
    day = (date_str[i+1] - '0');
  }

  return day;
  
}
#endif
#endif

// e_send_position
static void msg_position(void)
{

  insert_msg_header();
  
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
  DecimalUint8ToA ( sentence_buffer.gps_buffer + 54, rmc_sentence.UtcOfPosition.Horas    , 2, false );
  DecimalUint8ToA ( sentence_buffer.gps_buffer + 56, rmc_sentence.UtcOfPosition.Minutos  , 2, false );
  DecimalUint8ToA ( sentence_buffer.gps_buffer + 58, rmc_sentence.UtcOfPosition.Segundos , 2, false );
  DecimalUint16ToA( sentence_buffer.gps_buffer + 29, rmc_sentence.Latitude.Decimas       , 4, false );
  DecimalUint16ToA( sentence_buffer.gps_buffer + 45, rmc_sentence.Longitude.Decimas      , 4, false );
  
}



// e_No_gps
static void msg_no_gps(void){
// PreparaMensajeNoHayGps(void)

  insert_msg_header();
  
  strcpy((char*)sentence_buffer.gps_buffer + 23, gMensajeNoHay);
  strcpy((char*)sentence_buffer.gps_buffer + 30, gMensajeGps);
  
#if USE_NEW_VERSION_ID

  strcpy ( (char*)(sentence_buffer.gps_buffer + 35), gMensajeVersion_H );
  insert_version((char*)(sentence_buffer.gps_buffer + 37));
  strcpy ( (char*)(sentence_buffer.gps_buffer + 41), gMensajeVersion_T );
  
#else  
  strcpy((char*)sentence_buffer.gps_buffer + 34, gMensajeVersion);
#endif
  
  strcpy((char*)sentence_buffer.gps_buffer + 42, gMensajeRadiogonio);
}

// e_No_position_no_time
static void msg_no_position_no_time(void){

  insert_msg_header();
  
  strcpy((char *)sentence_buffer.gps_buffer + 23, gMensajeNoHay);
  strcpy((char *)sentence_buffer.gps_buffer + 30, gMensajePos);
  strcpy((char *)sentence_buffer.gps_buffer + 34, gMensajeNoHay);
  strcpy((char *)sentence_buffer.gps_buffer + 41, gMensajeHora);
  strcpy((char *)sentence_buffer.gps_buffer + 46, gMensajeGuion);
  strcpy((char *)sentence_buffer.gps_buffer + 50, gMensajeRadiogonio);
  
}


// e_No_gps_reception
static void msg_no_gps_reception(void){
//  PreparaMensajeFueraDeCobertura(void)

  insert_msg_header();
  strcpy((char*)sentence_buffer.gps_buffer + 23, gMensajeNoCobertura);
  strcpy((char*)sentence_buffer.gps_buffer + 42, gMensajeGuion);
  strcpy((char*)sentence_buffer.gps_buffer + 46, gMensajeBuscando);
  strcpy((char*)sentence_buffer.gps_buffer + 55, gMensajeGps);
  insert_time(59);
}

// e_Gps_searches_position
static void msg_gps_searches_position(void){  // nline PreparaMensajeBuscandoGps(void){
  insert_msg_header();
  strcpy((char*)sentence_buffer.gps_buffer + 23, gMensajeBuscando);
  strcpy((char*)sentence_buffer.gps_buffer + 32, gMensajeGps);
  insert_time(36);
}

// e_Low_baterie
static void msg_low_baterie(void){
  // PreparaMensajeBateriaBaja(void){
  insert_msg_header();
  strcpy((char*)sentence_buffer.gps_buffer + 23, gMensajeBateriaBaja);
  insert_time(36);
}

void inline PreparaMensajeLucesActivas(void)
{
//  insert_msg_header();
//  strcpy(sentence_buffer.gps_buffer + 23, gMensajeLucesActivas);
//  
//  sentence_buffer.gps_buffer[39] =  '>';
//  sentence_buffer.gps_buffer[46] =  'H';
//  DecimalUint8ToA ( sentence_buffer.gps_buffer + 40, rmc_sentence.UtcOfPosition.Horas    , 2, false );
//  DecimalUint8ToA ( sentence_buffer.gps_buffer + 42, rmc_sentence.UtcOfPosition.Minutos  , 2, false );
//  DecimalUint8ToA ( sentence_buffer.gps_buffer + 44, rmc_sentence.UtcOfPosition.Segundos , 2, false );
}


void inline PreparaMensajeRevisarAntena(void)
{
  insert_msg_header();
  strcpy((char*)sentence_buffer.gps_buffer + 23, gMensajeRevisar);
  strcpy((char*)sentence_buffer.gps_buffer + 37, gMensajeGps);
  insert_time(41);
}

// 11052023 --> this seems unused...
void PreparaMensajeRadiogonio(uint8_t val)
{
//  DDS_flush_buffer();
  insert_msg_header();
//  strcpy ( sentence_buffer.gps_buffer + 22, " RADIOGONIO N>0<" );
  strcpy ((char*)sentence_buffer.gps_buffer + 23, gMensajeRadiogonio );
  DecimalUint8ToA ( sentence_buffer.gps_buffer + 36, val, 1, false );
}



static void insert_msg_header(void)
{
  
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
  sentence_buffer.gps_buffer[22] =  ' ';
}




static void insert_baterie(void)
{
  
#if DEBUGGING_IS_ON&&0

  sentence_buffer.gps_buffer[14] = '5'; // vbat[0];
  sentence_buffer.gps_buffer[15] = '4'; // vbat[1];
  sentence_buffer.gps_buffer[17] = '2'; //vbat[2];

  
#else  
  
  char vbat[3];
  
  LeerValorBateria(true);
  
  
  
  DecimalUint8ToA( (uint8_t*)&vbat[0], baterie_mV, 3, false );

  sentence_buffer.gps_buffer[14] = vbat[0];
  sentence_buffer.gps_buffer[15] = vbat[1];
  sentence_buffer.gps_buffer[17] = vbat[2];
  
#endif
  
}

static void insert_time(uint8_t pos)
{
  sentence_buffer.gps_buffer[pos] =  '>';
  DecimalUint8ToA ( sentence_buffer.gps_buffer + pos + 1, rmc_sentence.UtcOfPosition.Horas    , 2, false );
  DecimalUint8ToA ( sentence_buffer.gps_buffer + pos + 3, rmc_sentence.UtcOfPosition.Minutos  , 2, false );
  DecimalUint8ToA ( sentence_buffer.gps_buffer + pos + 5, rmc_sentence.UtcOfPosition.Segundos , 2, false );
  sentence_buffer.gps_buffer[pos + 7] =  'H';
}


#if USE_NEW_VERSION_ID||OV_VERSION_ID
static void insert_version(uint8_t * buf){
  
  DecimalUint16ToA(buf, (uint16_t)version_nr, 4, false);
  // DecimalUint16ToA(buf, (uint16_t)COMBINED_CODE, 4, false);
  
  
}
#endif


#if 0


void int_to_4digit_string(int number, char *buffer) {
    buffer[0] = (number / 1000) % 10 + '0'; // Thousands place
    buffer[1] = (number / 100) % 10 + '0';  // Hundreds place
    buffer[2] = (number / 10) % 10 + '0';   // Tens place
    buffer[3] = number % 10 + '0';          // Units place
    buffer[4] = '\0';                       // Null terminator
}

int main() {
    char formatted_code[5]; // 4 digits + null terminator
    int_to_4digit_string(COMBINED_CODE, formatted_code);
    printf("Combined value as string: %s\n", formatted_code);
    return 0;
}

#endif

// EOF