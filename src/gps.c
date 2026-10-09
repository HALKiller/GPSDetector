// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#line 6 "gps.c"

//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //
 // this version works as it should do...
// $PQTMVER,MODULE_LC86GPANR01A02S,2023/06/14,09:12:59*6C
// ... and this one does not
// $PQTMVER,1,MODULE,LC86GPANR01A05S,2025/09/02,10:50:51*0D

// Furthermore is there on the v66 the possibility that the module is actually a L86
// these modules have a slightly different instruction set and needs adressing 
// specifically in that case

//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //

#include "gps.h"

#include "gps_extensions.h"

#include "extension_strings.h"

#include "Global.h"

#include "io_port_sfr_names.h"

#include "device_driver_config.h"

#include  "UART.h"

#include "timers.h"

#include "gd_states.h"

#include "handlers.h"

#include "e_rtc.h"

#include "my_assert.h"

#include "generic_union_flgs.h"

#include "detector.h"



#include <string.h>

// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  
#ifndef FILE_GPS_DB_ENABLED
#define FILE_GPS_DB_ENABLED 0
#endif
#if FILE_GPS_DB_ENABLED
#define DB_PRINT(str) G_DB_PRINT(str)
#define UART_int(var) G_UART_INT(var)
#else
#define DB_PRINT(str)
#define UART_int(var)
#endif
// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  


//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 
#define GPS_PRINT 1
 
#define LOCK_TIME_COUNTER 6u

#define MAX_DATA_LENGTH_GPS_SENTENCE 82

#define MAXIMUM_RECONFIGURATIONS_PER_ACTIVATION 4

#define const_STARTWORDCOUNT_LEN 6

#define STARTUP_LOCK_TIME MINIMUM_GPS_ON_BEFORE_TRANSMISSION

#define DEFAULT_BAUD (B115200)  // (B9600)  //(B57600)  // 
#define D_BAUD 1

#define GPS_MODULE_L86 1
#define GPS_MODULE_L86G 0


//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //




union	udt_UART_GPS_FLGS {
	uint16_t reg;
	struct
  {
    unsigned debugging_send_sentences : 1;
    unsigned debugging_sync_time      : 1;
    unsigned receiving_chars_is_good  : 1; // every time we swoff we reset this flag and test then for xy seconds or until we have received a certain amoutns of chars 
		unsigned valid_header_received    : 1;
		unsigned gps_sentence_is_good 		: 1;
		unsigned startbyte_found				  : 1;  // this one is used
		unsigned startword_found				  : 1;  // this one is used
		unsigned endbyte_found					  : 1;  // this one is used
		unsigned rmc_time_is_good		      : 1;  // this one is used
    unsigned gsa_position_is_good     : 1;

    // unsigned timeout_tmr_is_running   : 1;
    unsigned rtc_test_first_run       : 1;  // for resetting with the uart for debugging and developing
    unsigned gps_stop_debug_flg       : 1;
    unsigned gps_has_first_lock       : 1;  // that gets set on the first valid lock position --> reset on startup  
		// unsigned use_rmc_time             : 1;
    unsigned free										  : 3;	
	};
};






static union udt_UART_GPS_FLGS UART_GPS_FLG;

static uint32_t gps_rtc_time = 0;

typedef struct udt_gps_type{
  
  uint32_t lock_time_start;
  uint32_t lock_time_end; // gets set  when the gps has  a valid lock --> that includes the cnt for getting a valid pos..-> in case of gps on till tx still uses the valid cnt const 
  uint16_t lock_times[LOCK_TIME_COUNTER];
  uint16_t average_lock_time;
  uint16_t last_lock_time;  // this is only used for testing transmissions
  gps_state_t state;
  uint8_t sentence_id;
  int8_t lock_indexer;
  
}gps_t;

static gps_t gps_module;

gps_sentence_t sentence_buffer;

RMC_sentence_t rmc_sentence;

RMC_sentence_t copy_of_rmc;

static GSA_sentence_t gsa_sentence;

#if USE_POSITION_CNT_VALIDATION
static uint8_t valid_position_cnt = 0;
#endif

#if 0

static const uint32_t gps_standard_baud_rate_settings[] = {
  
  4800,
  9600,
  57600,
  115200,
  
};
#endif


//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 
// * * * * * * *    L 8 6   M O D U L E   S E N T N C E S   * * * * * * * * * * * * *   // 

#if GPS_MODULE_L86

const uint8_t m_cmd_set_115kBaud[] = "$PMTK251,115200*1F\r\n";
const uint8_t m_cmd_set_constelation[] = "$PMTK353,1,0,1,1,1*2B\r\n";
const uint8_t m_cmd_set_sentences[] = "$PMTK314,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n";

#elif GPS_MODULE_L86G
    // UART_GPS_SEND("$PAIR864,0,0,57600*28\r\n");
    // UART_GPS_SEND("$PAIR864,0,0,9600*13\r\n"); 
const uint8_t m_cmd_set_115kBaud[] = "$PAIR864,0,0,115200*1B\r\n";
const uint8_t m_cmd_set_constelation[] = "$PAIR066,1,0,1,1,1,0*3A\r\n";
const uint8_t m_cmd_get_constelation[] = "$PAIR067*3B\r\n";

#else
  

#endif


 
#if 0
 

static const char *sentences[] = {
  
  "$GPRMC",
  "$GNRMC",
  "$GPGSA",
  "$GNGSA",
  "$EESLf", // Reset MCU
  "$EESLr", // prepare for new syncing rtc to gps
  "$EESLs", // set for clock switch  
  "$EESLp", // set for luz on off
  "$EESLa", // DEBUG_FLG_PRINT_TIME
};

#else
 
const uint8_t c_GPRMC[] = "$GPRMC";
const uint8_t c_GNRMC[] = "$GNRMC";
const uint8_t c_GPGSA[] = "$GPGSA";
const uint8_t c_GNGSA[] = "$GNGSA";
const uint8_t c_EERES[] = "$EESLf"; // Reset MCU

const uint8_t *const sentences[] = {
  
  c_GPRMC,
  c_GNRMC,
  c_GPGSA,
  c_GNGSA,
  c_EERES
};

#endif




//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //

// *********************************************  G P S   B U F F E R   *****************************************  //
// *********************************************  G P S   B U F F E R   *****************************************  //





#if 1



static uint8_t *src_buff_pnt = &sentence_buffer.gps_buffer[0];

static const uint8_t cMax_Sentence_length_GPS = MAX_DATA_LENGTH_GPS_SENTENCE;

static uint8_t *temp_buff_pnt = &sentence_buffer.gps_buffer[0];

static uint8_t temp_buff_pnt_cnt = MAX_DATA_LENGTH_GPS_SENTENCE;  // cMax_Sentence_length_GPS;

static uint8_t endbyte_cnt = 2;

static bool process_gps_position(void);

static uint8_t check_validity_of_time_diff(void);


#endif

// *********************************************  G P S   B U F F E R   E N D   *****************************************  //
// *********************************************  G P S   B U F F E R   E N D   *****************************************  //


//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

static void configure_gps(void);

static void try_reconfigure_gps(void);

static void send_recfg_gps_sentences(void);
 
static void disable_constelations(void);

static uint8_t check_against_header(const char *t_buffer);

static void sentence_handler(uint8_t sentence_id);

#if 1
static void reset_uart_handler_flags(void);
#endif

uint8_t GPS_checksum_checker(uint8_t *d_pnt, uint8_t d_length);

static void process_gsa_sentence(void);

static void process_rmc_sentence(void);

static void Debugging_read_out_rmc(void);

static void convert_utc_to_gps_rtc_time(void);

static void copy_rmc_to_from(RMC_sentence_t *const des_pnt,  RMC_sentence_t const *const src_pnt);

#if DEBUGGING_BB_IS_ON

static void send_gps_direct(void);
  
#endif


//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //


  // we need to do all of these things: this is not yet in correct order perhaps!!
  // start up the uart
  // start up the gps
  // receive something perhaps at least 10 (xy) bytes?
  // and then keep on going with the recpetion 
  // full useable sentence received --> set the necessary flags and handle the transfer to the next stage
  
  




//  * * * * * * * * * * * * * * * * * * * * * * * * * * * *    G P S       * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * *    G P S       * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
void gps_init(void){
  
  int8_t hlooper = 0;
  
  // NONE
  UART_GPS_FLG.rtc_test_first_run = FALSE;
  // RMC_TIME_IS_VALID = false;
  gps_module.lock_indexer = 0u;
  gps_module.lock_time_start = 0u;
  gps_module.lock_time_end = 0u;
  
  for(hlooper = 0; hlooper < LOCK_TIME_COUNTER; hlooper++)
  {
    gps_module.lock_times[hlooper] = STARTUP_LOCK_TIME;
  }
  gps_module.average_lock_time = STARTUP_LOCK_TIME;

}

void gps_first_run(void){
  
  uart_init_cfg(DEFAULT_BAUD);
  
  UART_on();  // the Peripheric UART
  
  GPS_VALIM = TRUE; // gps gest energy

  configure_gps();
 
  GPS_VALIM = FALSE;
  
  
}

uint32_t gps_rtc_get_second_cnt(void){
  
  bool temp_GIE = GLOBAL_IE;

	GIE = false;

  uint32_t ret_val = gps_rtc_time;
  
  GIE = temp_GIE;
  
  return ret_val;
  
}

uint16_t gps_get_average_lock_time(void){
  
  return gps_module.average_lock_time;
  
}

uint16_t gps_get_last_lock_time(void){
  
  return gps_module.last_lock_time;
  
}


void gps_startup_initializer(void){

  DB_PRINT(" gps_s ");

  uart_init_cfg(DEFAULT_BAUD);   
 
  // gps_module.baudslot = 0u;
  
  gps_reinit();
 
}


#if 1

// we are switchng on and await the time out time to get 
// information if we are actually receiving something usefull...
void gps_reinit(void){

  // DB_PRINT("\r\ngps_reinit\r\n");

  UART_GPS_FLG.reg = 0u; // reset everything

  // this resets the temp_buffer and temp_buffer_pointer...
  reset_uart_handler_flags();
  
  UART_on();
  
  CREN = TRUE;
  
  RX_IE = TRUE;
  
#if USE_DEVICE_DRIVER
  // switch on the gps valim pin
  IO_Set_channel(IO_GPS_VALIM);

#else  
  
  // switch on the gps valim pin
  GPS_VALIM = TRUE;
  
  

#endif  
  my_delay_ms(1000);
  // reset the position_cnt...
  valid_position_cnt = 0;
  

  // TODO:
  // reset the uart buffer --> that should get perhaps on the initializer of the uart !

  timers_set_tmr1_id(GPS_UART_TIMEOUT);
  start_timeout_tmr();


  // save the time for the moment...
  gps_module.lock_time_start = eRTC_get_second_cnt();

#if 0

  my_delay_ms(1500);
  disable_constelations();
  
  send_recfg_gps_sentences();
  
#endif
	
  my_delay_ms(100);
 
}




#else
  
// we are switchng on and await the time out time to get information if we are actually receiving something usefull...
void gps_reinit(void){

  DB_PRINT("\r\ngps_reinit\r\n");

  UART_GPS_FLG.reg = 0u; // reset everything

  // this resets the temp_buffer and temp_buffer_pointer...
  reset_uart_handler_flags();
  
  UART_on();
  
  CREN = TRUE;
  
  RX_IE = TRUE;
  
#if USE_DEVICE_DRIVER
  // switch on the gps valim pin
  IO_Set_channel(IO_GPS_VALIM);

#else  
  
  // switch on the gps valim pin
  GPS_VALIM = TRUE;

#endif  
  my_delay_ms(1000);
  // reset the position_cnt...
  valid_position_cnt = 0;
  
// Becasue in debug mode the uart is always on becasue we are sending and receivng from there  

  // TODO: init UART and GPS
  // uart_initialize(GPS_UART);

 
  // TODO:
  // reset the uart buffer --> that should get perhaps on the initializer of the uart !

  timers_set_tmr1_id(GPS_UART_TIMEOUT);
  start_timeout_tmr();


  // save the time for the moment...
  gps_module.lock_time_start = eRTC_get_second_cnt();

#if 1

  my_delay_ms(1500);
  disable_constelations();
  
  send_recfg_gps_sentences();
  
#endif
	
  my_delay_ms(100);
 
}

#endif



void gps_stop(void){
 
  DB_PRINT("\r\ngps_stop\r\n");
 
  GPS_VALIM = FALSE;
  
#if DEBUGGING_IS_ON 
  // and switch off the valim_pin for the UART_CRLF
  UART_GPS_FLG.gps_stop_debug_flg = true;
  flush_ring_buffer();
  
  reset_uart_handler_flags();  
#else
  
  RX_IE = FALSE;
  
  flush_ring_buffer();
  
  reset_uart_handler_flags();
  

  // UART_off();
  
#endif  
  
}


void gps_calculate_lock_time(void){
  
  
  uint32_t temp_avg_calulator = 0u;
  
  int8_t hlooper = 0;
  
  // we have the indexer and we have a start and a stop time  
  // stop timer < start timer case handlers
  
  if(gps_module.lock_time_end < gps_module.lock_time_start)
  {
    gps_module.lock_time_end = gps_module.lock_time_end + SECONDS_PER_DAY;
  }
  
  gps_module.lock_times[gps_module.lock_indexer] = gps_module.lock_time_end - gps_module.lock_time_start;
  
  gps_module.last_lock_time = gps_module.lock_times[gps_module.lock_indexer];
#if DEBUGGING_BB_IS_ON

  DB_PRINT("\r\nLock_time: ");
  UART_int(gps_module.lock_times[gps_module.lock_indexer]);
  UART_CRLF;

#endif  
  
  
  
  for(hlooper = 0u; hlooper < LOCK_TIME_COUNTER; hlooper++)
  {
    temp_avg_calulator = temp_avg_calulator + gps_module.lock_times[hlooper];
  }
  
  gps_module.average_lock_time = temp_avg_calulator / LOCK_TIME_COUNTER;
  
  gps_module.lock_indexer++;
  
  if(gps_module.lock_indexer >= LOCK_TIME_COUNTER)
  {
    gps_module.lock_indexer = 0u;
  }
  
}


// because during the development its interesting to see where it fails, in the release it makes no difference, it works or it does not
// we are checking here to find out:
// receiving all good? best!
// receiving something but not reading quite --> reconfigure UART_CRLF// receive nothing --> fatal!
gps_state_t gps_check_gps_error_status(void){
  

  stop_timeout_tmr();
  // TMR1_ON = FALSE;
  
  // UART_GPS_FLG.timeout_tmr_is_running = FALSE;
  
  if(UART_GPS_FLG.gps_sentence_is_good)
  {
    gps_module.state = GPS_SENTENCE_RECEIVING;  // GPS_ALL_GOOD
#if GPS_PRINT   
    DB_PRINT("\r\nGPS Works\r\n");
#endif    
  }
  else if(UART_GPS_FLG.receiving_chars_is_good == FALSE)
  {
    
    gps_module.state = NOT_RECEIVING;
#if GPS_PRINT       
    DB_PRINT("\r\nGPS F ERR\r\n");
 #endif       
    // try_reconfigure_gps();
    gps_reinit();
  }
  else if(UART_GPS_FLG.valid_header_received == FALSE)
  {
    
    gps_module.state = NOT_RECEIVING_CORRECTLY;
#if GPS_PRINT           
    DB_PRINT("\r\nGPS B ERR\r\n");
 #endif          
    // try_reconfigure_gps();
    gps_reinit();
  }
  else
  {
    gps_module.state = GPS_SENTENCE_RECEIVING;
#if GPS_PRINT           
    DB_PRINT("\r\nGPS O.K.\r\n");
 #endif          
  }
  
  return gps_module.state;

}

gps_state_t gps_get_gps_state(void){
  
  return gps_module.state;
  
}

RMC_sentence_t *get_pointer_to_rmc(void){
  
  return &rmc_sentence;
  
}


uint8_t gps_buffer_get_len(void){
  
  uint8_t hlooper = 0;
  
  uint8_t *data_pnt = &(sentence_buffer.gps_buffer[0]); // &(sentence_buffer.gps_buffer);
  
  while((*data_pnt != NULL_TERMINATOR) && (hlooper < MAX_SENTENCE_LENGTH))
  {
    hlooper++;
    data_pnt++;
  }
  
  return hlooper;
  
  
}


void copy_position_from_to(fromto_t fromto){
  
  if(fromto == SAVEPOSITION)
  {
    copy_rmc_to_from(&copy_of_rmc, &rmc_sentence );
    COPY_POS_IS_VALID = true;
  }
  else
  {
    copy_rmc_to_from(&rmc_sentence,  &copy_of_rmc);
  }
  
  
}

//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //

// overworking it so that we receive for the time being with 115200
// this works perfectly !!  --> prepared for 9600Baud recfg
static void configure_gps(void){
  
  // this one increases whenever we are having looper 
  // over all gps settings in the array and there was no good sentece...
  static uint8_t maximum_reconfigure_cnt = 0;
  
  uint8_t hlooper = 0;

  uint16_t setter = B115200;
  
  maximum_reconfigure_cnt++;
  
  // TODO --> check if thi sis certain actually...
  GLOBAL_IE = TRUE;
  
  // GPS_VALIM = TRUE;

#if DEBUGGING_BB_IS_on
  // (( idx 0 is 2000ms timeout))
  timers_set_tmr1_id(3);
  
  send_gps_direct();
  CLRWDT();
  send_gps_direct();
  CLRWDT();
  
#endif

  // so speed first up for matching the GPS...
  // uart_init_cfg(B115200);
  for(hlooper = 0; hlooper < 3; hlooper++)
  {
    switch(hlooper)
    {
      case 0:
        setter = B9600;
      break;
      case 1:
        setter = B57600;
      break;
      case 2:
        setter = B115200;
      break;
      
    }
  
    uart_init_cfg(setter);
    
    // now set the GPS to xy baud
    // send_bb_string("\r\ngps_cfg_115200");
    
    // UART_GPS_SEND("$PAIR864,0,0,115200*1B\r\n");
    
    UART_GPS_SEND(m_cmd_set_115kBaud);
    
    // UART_GPS_SEND("$PAIR864,0,0,57600*28\r\n");
    // UART_GPS_SEND("$PAIR864,0,0,9600*13\r\n"); 
#if DEBUGGING_BB_IS_on    
    send_gps_direct();
#endif
    
      
      //give a delay to 
    my_delay_ms(4);
    // my_delay_ms(400);
    
    // swoff --> reboot
    GPS_VALIM = FALSE;
    
        //give a delay to 
    my_delay_ms(100);
    
    // swon --> reboot
    GPS_VALIM = TRUE;
    

  }

  uart_init_cfg(DEFAULT_BAUD);  // (B57600);

  //give a delay to 
  my_delay_ms(1000);
  
  disable_constelations();
  
  my_delay_ms(1000);
  // now send the reduction of sentences from the GPS
  send_recfg_gps_sentences();

  UART_GPS_SEND("$PAIR513*3D\r\n");
  
  my_delay_ms(2000);
  // GPS_VALIM = FALSE;
  // my_delay_ms(100);  
  // GPS_VALIM = TRUE;
  
#if DEBUGGING_BB_IS_on  
  send_gps_direct();
#endif
  
  // my_delay_ms(1000);
  
  GPS_VALIM = FALSE;
  
  
  // DB_PRINT("\r\ncfg_sent\r\nb");
  
  
}

#if DEBUGGING_BB_IS_on

static void try_reconfigure_gps(void){
  

   
  // now send the reduction of sentences from the GPS
  send_recfg_gps_sentences();
  
  DB_PRINT(" recfg_gps ");
  
  configure_gps();
  
  
  
}
#elif USE_115K_BAUD

// overworking it so that we receive for the time being with 115200
// this works perfectly !!  --> prepared for 9600Baud recfg
static void try_reconfigure_gps(void){
  
  // this one increases whenever we are having looper 
  // over all gps settings in the array and there was no good sentece...
  static uint8_t maximum_reconfigure_cnt = 0;
  
  uint8_t hlooper = 0;
  uint16_t setter = B9600;
  
  maximum_reconfigure_cnt++;
  
  // so speed first up for matching the GPS...
  // uart_init_cfg(B115200);
  for(hlooper = 0; hlooper < 3; hlooper++)
  {
    switch(hlooper)
    {
      case 0:
        setter = B9600;
      break;
      case 1:
        setter = B57600;
      break;
      case 2:
        setter = B115200;
      break;
      
    }
  
    uart_init_cfg(setter);
    
    // give a delay to stabilize the baud rate generator...lets start with a 100ms...
    // my_delay_ms(100);
    
    // now set the GPS to xy baud
#if D_BAUD==1

    UART_GPS_SEND("$PAIR864,0,0,115200*1B\r\n");
    // UART_GPS_SEND("$PAIR864,0,0,57600*28\r\n");
      
    // send_bb_string("\r\n115200\r\n"); // ("\r\nMSG: $PAIR864,0,0,57600*28\r\n");
#elif  D_BAUD==2   
    // UART_GPS_SEND("$PAIR864,0,0,115200*1B\r\n");
    UART_GPS_SEND("$PAIR864,0,0,57600*28\r\n");
      
    // send_bb_string("\r\n57600\r\n"); // ("\r\nMSG: $PAIR864,0,0,57600*28\r\n");    
#else
    wat
    UART_GPS_SEND("$PAIR864,0,0,9600*13\r\n");   
    send_bb_string("$PAIR864,0,0,9600*13\r\n");  
    
#endif   
    
    
      //give a delay to 
    my_delay_ms(400);
    
    // swoff --> reboot
    GPS_VALIM = FALSE;
    
        //give a delay to 
    my_delay_ms(200);
    
    // swon --> reboot
    GPS_VALIM = TRUE;
    
    my_delay_ms(500);
    
  }

  uart_init_cfg(DEFAULT_BAUD);  // (B57600);


  disable_constelations();
  

  // now send the reduction of sentences from the GPS
  send_recfg_gps_sentences();

 
  
}

static void disable_constelations(void){
  
  
  // UART_GPS_SEND("$PAIR066,1,0,1,1,1,0*3A\r\n");
  
  UART_GPS_SEND(m_cmd_set_constelation);
  
  // UART_GPS_SEND("$PAIR066,1,0,0,0,0,0*3B\r\n");
  
  my_delay_ms(50);
  
  UART_GPS_SEND("$PAIR067*3B\r\n");
  
 
  
  
}




#else
// this works perfectly !!  --> prepared for 9600Baud recfg
static void try_reconfigure_gps(void){
  
  // this one increases whenever we are having looper 
  // over all gps settings in the array and there was no good sentece...
  static uint8_t maximum_reconfigure_cnt = 0;
  
  maximum_reconfigure_cnt++;
  
  // so speed first up for matching the GPS...
  uart_init_cfg(B115200);
  
  //give a delay to stabilize the baud rate generator...lets start with a 100ms...
  my_delay_ms(100);
  
  // now set the GPS to xy baud
#if COMPILE_FOR_DEBUG&&0

  UART_GPS_SEND("$PAIR864,0,0,115200*1B\r\n");
  // DB_PRINT("$PAIR864,0,0,115200*1B\r\n");
  
#else
  
  UART_GPS_SEND("$PAIR864,0,0,9600*13\r\n");   
  // DB_PRINT("$PAIR864,0,0,9600*13\r\n");  
  
#endif   
  
  
    //give a delay to 
  my_delay_ms(100);
  
  // swoff --> reboot
  GPS_VALIM = FALSE;
  
      //give a delay to 
  my_delay_ms(500);
  
  // swon --> reboot
  GPS_VALIM = TRUE;
  
  my_delay_ms(500);
  
  // so slow down again to matching the GPS...
#if COMPILE_FOR_DEBUG&&0
  uart_init_cfg(B115200);
#else
  uart_init_cfg(B9600);   
#endif 
  
  //give a delay to 
  my_delay_ms(100);
  
  // now send the reduction of sentences from the GPS
  send_recfg_gps_sentences();

  my_delay_ms(100);
  // and try again --> we call taht from the calling function now...
  // gps_reinit();
  
#if DISABLE_GLONASS

  // swoff glonass
  UART_GPS_SEND("$PAIR066,1,0,1,1,1,0*3A\r\n");
  
  my_delay_ms(500);
  
  UART_GPS_SEND("$PAIR067*3B\r\n");
  
  my_delay_ms(100);  

#elif DISABLE_NOT_GLONASS

  // swoff glonass
  UART_GPS_SEND("$PAIR066,0,1,0,0,0,0*3B\r\n");
  
  DB_PRINT("$PAIR066,0,1,0,0,0,0*3B\r\n");
  
  my_delay_ms(500);
  
  UART_GPS_SEND("$PAIR067*3B\r\n");
  
  my_delay_ms(100);  
  
#endif    
  
  DB_PRINT("\r\ncfg_sent\r\nb");
  
  
}



#endif



#if GPS_MODULE_L86

static void send_recfg_gps_sentences(void){
  
  UART_GPS_SEND(m_cmd_set_sentences);
  
  DB_PRINT("\r\nSent cfg\r\n");
  
}


#elif GPS_MODULE_L86G
  

static void send_recfg_gps_sentences(void){
  
  
  
static uint8_t gps_out_sentence[] = {
  '0', '1', '3', '5', '6', '7', '8', '9'
};

static  uint8_t gps_out_sentence_chck[] = {
  'E', 'F', 'D', 'B', '8', '9', '6', '7'
};



  static const char gps_front[] = "$PAIR062,";
  static const char gps_mid[] = ",0*3";
  static const char gps_end[] = "\r\n";


  uint8_t hlooper = 0;
  
#if 0

  UART_GPS_SEND("$PAIR067*3B\r\n");
  
  my_delay_ms(100);

#endif    
  
  // DB_PRINT("\r\n");

  strcpy((char *)(sentence_buffer.gps_buffer), gps_front);
  
  strcpy((char *)&sentence_buffer.gps_buffer[10], gps_mid);
  
  strcpy((char *)&sentence_buffer.gps_buffer[15], gps_end);
  
  for(hlooper = 0; hlooper < 8; hlooper++)
  {
    
    sentence_buffer.gps_buffer[9] = gps_out_sentence[hlooper];
    
    sentence_buffer.gps_buffer[14] = gps_out_sentence_chck[hlooper];

    DB_PRINT(&sentence_buffer.gps_buffer[0]);
    
    UART_GPS_SEND(&sentence_buffer.gps_buffer[0]);
    
    my_delay_ms(100);
    
  }

  // my_delay_ms(100);

  DB_PRINT(" S_cfg ");
  
}

#else
  
wat?

#endif







#if RUN_GPS_TILL_TX // DEBUGGING_IS_ON// using a db_flg to indicate that the gps is switched off...theoretically...



#if USE_115K_BAUD

/* Consume one UART byte. A new '$' always abandons an incomplete sentence. */
void values_to_gps_rx_buffer(uint8_t n_char){

#if GLONASS_BUG
  unsigned char t_str[2];
  t_str[1] = NULL_TERMINATOR;
#endif
  

  if(UART_GPS_FLG.gps_stop_debug_flg == true)
  {
    return;
  }

#if GLONASS_BUG&&0

  t_str[0] = n_char;
  send_bb_string(t_str);
  
#endif
  

  UART_GPS_FLG.receiving_chars_is_good = TRUE;

  if(n_char == '$')
  {
    reset_uart_handler_flags();
    UART_GPS_FLG.startbyte_found = TRUE;
    *temp_buff_pnt++ = n_char;
    temp_buff_pnt_cnt--;
    return;
  }

  if(UART_GPS_FLG.startbyte_found == FALSE)
  {
    return;
  }

  *temp_buff_pnt++ = n_char;
  temp_buff_pnt_cnt--;

  if(UART_GPS_FLG.endbyte_found)
  {
    endbyte_cnt--;
    if(endbyte_cnt == 0u)
    {
      /* Synchronization belongs only to the sentence processed below. */
      GPS_HAS_SYNCED = false;
      if(GPS_checksum_checker(src_buff_pnt, cMax_Sentence_length_GPS - temp_buff_pnt_cnt) == TRUE)
      {
        UART_GPS_FLG.gps_sentence_is_good = TRUE;
#if DEBUGGING_BB_IS_ON&&1
        /* The buffer may be completely full; do not write past its end. */
        if(temp_buff_pnt_cnt > 0u)
        {
          *temp_buff_pnt = NULL_TERMINATOR;
          // send_bb_string(sentence_buffer.gps_buffer);
        }
#endif
        
        sentence_handler(gps_module.sentence_id);
      }
#if DEBUGGING_IS_ON
      else
      {
        if(gps_module.sentence_id > 3)
        {
          sentence_handler(gps_module.sentence_id);
        }
        DB_PRINT("Cerr\r\n");
      }
#endif
      reset_uart_handler_flags();
      /* Full-position handling already reschedules the alarm. */
      if(process_gps_position() == false)
      {
        if(GPS_HAS_SYNCED == true)
        {
          handlers_generic_set_handler_FLG(e_gps_has_time_h);
        }
      }
      GPS_HAS_SYNCED = false;
      return;
    }
  }
  else if(UART_GPS_FLG.startword_found)
  {
    if(n_char == '*')
    {
      UART_GPS_FLG.endbyte_found = TRUE;
    }
  }
  else if(temp_buff_pnt_cnt == (MAX_DATA_LENGTH_GPS_SENTENCE - const_STARTWORDCOUNT_LEN))
  {
    *temp_buff_pnt = NULL_TERMINATOR;
    if(check_against_header((const char *)src_buff_pnt) == TRUE)
    {
      UART_GPS_FLG.startword_found = TRUE;
      UART_GPS_FLG.valid_header_received = TRUE;
    }
    else
    {
      reset_uart_handler_flags();
    }
  }

  if(temp_buff_pnt_cnt == 0u)
  {
    reset_uart_handler_flags();
  }
}


/* Position flags can change only when a complete sentence is processed. */
static bool process_gps_position(void){
#if DEBUGGING_IS_ON
  if((UART_GPS_FLG.rtc_test_first_run == FALSE) &&
     (UART_GPS_FLG.gsa_position_is_good == TRUE) && (UART_GPS_FLG.rmc_time_is_good == TRUE))
#else
  if((UART_GPS_FLG.gsa_position_is_good == TRUE) && (UART_GPS_FLG.rmc_time_is_good == TRUE))
#endif
  {

#if USE_POSITION_CNT_VALIDATION

    UART_GPS_FLG.gsa_position_is_good = FALSE;

    UART_GPS_FLG.rmc_time_is_good = FALSE;

    valid_position_cnt++;

    if(valid_position_cnt >= POSITION_CNT_BEFORE_VALID)
    {

#if RUN_GPS_TILL_TX
      // avoid overflow
      valid_position_cnt--;
      
      reset_rmc_valid_time_cnt();
      
      RMC_TIME_IS_VALID = true;
      
      if(UART_GPS_FLG.gps_has_first_lock == false)
      {
#if DEBUGGING_BB_IS_ON        
        send_bb_string("\r\nSYNC\r\n");
#endif
        UART_GPS_FLG.rtc_test_first_run = TRUE;

        // this sets the time when we got a valid lock time...
        stop_gps_lock_time_cnt();

        // that was inside the actual handler...
        gps_calculate_lock_time();

        UART_GPS_FLG.gps_has_first_lock = true;
        
      }

#else
      DB_PRINT("\r\nSync\r\n");
      UART_GPS_FLG.rtc_test_first_run = TRUE;
      stop_gps_lock_time_cnt();
#endif

      if(GPS_HAS_SYNCED == false)
      {
        // convert_utc_to_gps_rtc_time();
        eRTC_clock_sync_to_gps(gps_rtc_time);
      }
      
      GPS_HAS_SYNCED = false;

      // set the handler flag...
      handlers_generic_set_handler_FLG(e_gps_has_full_position_h);
      return true;

    }


#else

    DB_PRINT(" SY ");

    UART_GPS_FLG.rtc_test_first_run = TRUE;

    stop_gps_lock_time_cnt();


    // convert_utc_to_gps_rtc_time();
    eRTC_clock_sync_to_gps(gps_rtc_time);

    // gd_states_set_next_state(E_TRANSMISSION_STATE);
    handlers_generic_set_handler_FLG(e_gps_has_full_position_h);

    UART_GPS_FLG.gsa_position_is_good = FALSE;
    UART_GPS_FLG.rmc_time_is_good = FALSE;
    return true;

#endif

  }

  return false;
}


#endif


void stop_gps_lock_time_cnt(void){
  
  gps_module.lock_time_end = eRTC_get_second_cnt();
  
}



void set_max_lock_time(void){
  
  gps_module.lock_time_end = gps_module.lock_time_start + gd.time_between_tx;
  
}





#endif


static uint8_t check_against_header(const char *t_buffer){
  
  uint8_t hlooper = 0;
  uint8_t ret_value = false;
  
  for(hlooper = 0; hlooper < sizeof(sentences) / sizeof(sentences[0]); hlooper++)
  {
    if(strcmp(( char *)t_buffer, ( char *) sentences[hlooper]) == false)
    {
      ret_value = true;
      
      gps_module.sentence_id = hlooper;
      
      break;
    }
  }
  
  if(ret_value == false)
  {
    
  }

  return ret_value;
  
  
}



#if 1

static void reset_uart_handler_flags(void){
  
  // DB_PRINT("\r\nUR\r\n");
  
  UART_GPS_FLG.startbyte_found = FALSE;
  UART_GPS_FLG.startword_found = FALSE;
  UART_GPS_FLG.endbyte_found = FALSE;
  temp_buff_pnt = &(sentence_buffer.gps_buffer[0]);
  temp_buff_pnt_cnt = cMax_Sentence_length_GPS;
  endbyte_cnt = 2u;

  
}

#endif




// takes a pointer to the first character, the length of the sentence inclusive checksum delimiter and checksum
// returns true if check is good or false if it does not sum up
uint8_t GPS_checksum_checker(uint8_t *d_pnt, uint8_t d_length){
	
#define KLEIN_GROSS_SCHREIBUNG	0xDF
	
  uint8_t hlooper = 0;
  uint8_t chcksum = 0x00;
  uint8_t received_chcksum = 0;
  uint8_t temp_char[2];


	d_pnt++;
  
	for(hlooper = 1; hlooper < (d_length - 3); hlooper++)
  {
		
		chcksum = chcksum ^ *d_pnt;
		d_pnt++;
    
	}

  for(hlooper = 0; hlooper < 2; hlooper++)
  {
    
    d_pnt++;
    
    // that converts to the same output independent of mayuscula or not
    if(*d_pnt > 70u)
    {
      temp_char[hlooper] = (*d_pnt & KLEIN_GROSS_SCHREIBUNG)  - 0x30u;
    }
    else
    {
      temp_char[hlooper] = *d_pnt  - 0x30u;
    }

    if(temp_char[hlooper] > 9u)
    {
      temp_char[hlooper] = temp_char[hlooper] - 7u;	
    }
    
  }

	received_chcksum = (uint8_t)(temp_char[0] << 4) | temp_char[1];
 
	if(received_chcksum == chcksum)
  {
		return true;
	}
  else
  {
		return false;
	}
	

}





#if DEBUGGING_IS_ON||DEBUGGING_BB_IS_ON    
static void sentence_handler(uint8_t sentence_id){
  
  switch(sentence_id)
  {
    
    case 0:
    case 1:
      process_rmc_sentence();
      // send_bb_string(" RMC ");
    break;
    
    case 2:   
    case 3:
      process_gsa_sentence();
      // send_bb_string(" GSA ");
    break;
    case 4:
      RESET();
    break;
    case 5:
      UART_GPS_FLG.rtc_test_first_run = FALSE;
      UART_GPS_FLG.gsa_position_is_good = FALSE;
      UART_GPS_FLG.rmc_time_is_good = FALSE;
    break;
    case 6:
    
      SWITCH_CLOCK = TRUE;
      
    break;
    case 7:
      // swap_luz_on_off();
    break;
    case 8:
      DEBUG_FLG_PRINT_TIME = !DEBUG_FLG_PRINT_TIME;
    break;
    default:
      assert(false);
    break;

  }
  
}

#else
  
static void sentence_handler(uint8_t sentence_id){
  
  switch(sentence_id)
  {
    
    case 0:
    case 1:
      process_rmc_sentence();
      
    break;
    
    case 2:   
    case 3:
      process_gsa_sentence();
      
    break;
    case 4:
      RESET();
    break;
    default:
      assert(false);
    break;

  }
  
}

#endif



#if EMULATE_GPS_TIME_POSITION

static void process_gsa_sentence(void){
  
  gsa_sentence.ModoFijacion = FIX_3D;
  
  UART_GPS_FLG.gsa_position_is_good = true;


}

#elif 1

static void process_gsa_sentence(void){
  
  uint8_t *search_pnt;

#if DEBUGGING_BB_IS_ON&&1
  static uint8_t r_cnt = 0;
  
  r_cnt++;
  

 
  
  if( r_cnt >= 11)
  {
    r_cnt = 0;
    send_bb_string("\r\n");
    send_bb_string(sentence_buffer.gps_buffer);
    send_bb_string("\r\n");
  }
  
#endif  
 

    UART_GPS_FLG.gsa_position_is_good = false;
    
    search_pnt = Uint8_tStrchr( sentence_buffer.gps_buffer, ',' ) + 1;

    // El primer valor de la trama GSA es el modo: Manual o Automático
    if ( search_pnt[0] == 'M' )
    {
      gsa_sentence.ModoReceptor = RM;
      return; // we need automatic mode!
    }

 
    search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
    
    if ( search_pnt[0]  == '3' )
    {
      
      gsa_sentence.ModoFijacion = FIX_3D;
      
      UART_GPS_FLG.gsa_position_is_good = true;
      
    }

    
 #if 0 
    
    gSeHaProcesadoGsa = true;
    gNoProcesaMasGsa  = true;

#endif    

  
}



#else
  
static void process_gsa_sentence(void){
  
  uint8_t *search_pnt;

    UART_GPS_FLG.gsa_position_is_good = false;
    
    search_pnt = Uint8_tStrchr( sentence_buffer.gps_buffer, ',' ) + 1;

    // El primer valor de la trama GSA es el modo: Manual o Automático
    if ( search_pnt[0] == 'M' )
    {
      gsa_sentence.ModoReceptor = RM;
    }
    else if ( search_pnt[0] == 'A' )
    {
      gsa_sentence.ModoReceptor = RA;
    }
 
    search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
    
    if ( search_pnt[0] == 49 )
    {
      gsa_sentence.ModoFijacion = FIX_NOT_AVAILABLE;
    }
    else if ( search_pnt[0] == 50 )
    {
      gsa_sentence.ModoFijacion = FIX_2D;
    }
    else if ( search_pnt[0]  == 51 )
    {
      
      gsa_sentence.ModoFijacion = FIX_3D;
      
      UART_GPS_FLG.gsa_position_is_good = true;
      
    }
    
 #if 0 
    
    gSeHaProcesadoGsa = true;
    gNoProcesaMasGsa  = true;

#endif    

  
}

#endif


#if 1

static void convert_utc_to_gps_rtc_time(void){
  

  gps_rtc_time = (uint32_t)rmc_sentence.UtcOfPosition.Horas * SECONDS_PER_HOUR;
  
  gps_rtc_time = gps_rtc_time + (uint16_t)rmc_sentence.UtcOfPosition.Minutos * SECONDS_PER_MINUTE; 

  gps_rtc_time = (gps_rtc_time + rmc_sentence.UtcOfPosition.Segundos);
  
  
}

#else
  

static void convert_utc_to_gps_rtc_time(void){
  
  
  DB_PRINT("g_t: ");
  
  UART_int(rmc_sentence.UtcOfPosition.Horas);
  DB_PRINT(" ");
  UART_int(rmc_sentence.UtcOfPosition.Minutos);
  DB_PRINT(" ");
  UART_int(rmc_sentence.UtcOfPosition.Segundos);
  
  
  
  DB_PRINT(" ");
  
  gps_rtc_time = (uint32_t)rmc_sentence.UtcOfPosition.Horas * SECONDS_PER_HOUR;
  
  UART_int(gps_rtc_time);
  DB_PRINT(" ");
  
  gps_rtc_time = gps_rtc_time + (uint16_t)rmc_sentence.UtcOfPosition.Minutos * SECONDS_PER_MINUTE; 
  
  UART_int(gps_rtc_time);
  DB_PRINT(" ");
  
  gps_rtc_time = (gps_rtc_time + rmc_sentence.UtcOfPosition.Segundos);
  
  UART_int(gps_rtc_time);
  
}

#endif


#if EMULATE_GPS_TIME_POSITION

static void process_rmc_sentence(void){
  
  uint8_t *search_pnt;
  
  uint8_t* comma_pnt;
  
  
#if DB_V69_PCB
  static uint8_t r_cnt = 10;
 
#endif  
  // $GPRMC,102736.420,A,4245.033333,N,02045.033333,W,1.62,125,211124,1,E,A*23
  
  // opimising...
  UART_GPS_FLG.rmc_time_is_good = true;
  rmc_sentence.HayLatitud = true;
  rmc_sentence.HayLongitud = true;
  
 // El primer valor que se encuentra es la hora UTC, que siempre aparece en todo tipo de tramas RMC
  rmc_sentence.UtcOfPosition.Horas    = 13; //AToUint8_t( search_pnt, 2 );
  rmc_sentence.UtcOfPosition.Minutos  = 30;  // AToUint8_t( search_pnt + 2, 2 );
  rmc_sentence.UtcOfPosition.Segundos =   25; //AToUint8_t( search_pnt + 4, 2 );

  rmc_sentence.Status = SA;
  UART_GPS_FLG.rmc_time_is_good = true;

  rmc_sentence.HayLatitud = true;
  rmc_sentence.Latitude.Grados  = 42;  // AToUint8_t( search_pnt, 2 );
  rmc_sentence.Latitude.Minutos = 15; // AToUint8_t( search_pnt + 2, 2 );
  rmc_sentence.Latitude.Decimas = 25; // AToUint8_t( search_pnt + 5, 2 ) * 0x0064 + AToUint8_t( search_pnt + 7, 2 );

  rmc_sentence.LatiDirection = eNORTH;

  rmc_sentence.HayLongitud = true;
  rmc_sentence.Longitude.Grados  = 8; //  AToUint8_t( search_pnt, 3);
  rmc_sentence.Longitude.Minutos = 15;  // AToUint8_t( search_pnt + 3, 2);
  rmc_sentence.Longitude.Decimas = 22;  // AToUint8_t( search_pnt + 6, 2 ) * 0x0064 + AToUint8_t( search_pnt + 8, 2 );

  // $GPRMC,102736.420,A,4245.033333,N,02045.033333,W,1.62,125,211124,1,E,A*23

  rmc_sentence.LongDirection = eEAST;

  rmc_sentence.Date.Dia  = 10;  // AToUint8_t( search_pnt, 2 );
  rmc_sentence.Date.Mes  = 11;  // AToUint8_t( search_pnt + 2, 2 );
  rmc_sentence.Date.Anyo = 2025;  // AToUint8_t( search_pnt + 4, 2 );


}


#else

// optimizing...
static void process_rmc_sentence(void){
  
  uint8_t *search_pnt;
  
  uint8_t* comma_pnt;
  
  uint8_t l_flg = true;
#if DEBUGGING_BB_IS_ON&&1

  static uint8_t r_cnt = 10;
  
#endif  
  // $GPRMC,102736.420,A,4245.033333,N,02045.033333,W,1.62,125,211124,1,E,A*23
  

  
  
  
  // selectortrama = 0x10; // 0b0001 0000
  search_pnt = Uint8_tStrchr( sentence_buffer.gps_buffer, ',' ) + 1;
  // opimising...
  UART_GPS_FLG.rmc_time_is_good = false;
  rmc_sentence.HayLatitud = false;
  rmc_sentence.HayLongitud = false;
  
 // El primer valor que se encuentra es la hora UTC, que siempre aparece en todo tipo de tramas RMC
  rmc_sentence.UtcOfPosition.Horas    = AToUint8_t( search_pnt, 2 );
  rmc_sentence.UtcOfPosition.Minutos  = AToUint8_t( search_pnt + 2, 2 );
  rmc_sentence.UtcOfPosition.Segundos = AToUint8_t( search_pnt + 4, 2 );





  // Haya o no haya posición, siempre estará un indicador sobre el estado:
  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
  if ( search_pnt[0] == 'V' )
  {
    rmc_sentence.Status = SV;
  }
  else if ( search_pnt[0] == 'A' )
  {
    rmc_sentence.Status = SA;
    UART_GPS_FLG.rmc_time_is_good = true;
  }
  

  if(!((rmc_sentence.UtcOfPosition.Horas < 24) && 
    (rmc_sentence.UtcOfPosition.Minutos < 60) &&
    (rmc_sentence.UtcOfPosition.Segundos < 60)))
  {
    l_flg = false;
    UART_GPS_FLG.rmc_time_is_good = false;
  }

  
 // $GPRMC,102736.420,A,4245.033333,N,02045.033333,W,1.62,125,211124,1,E,A*23
  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
  // Si lo siguiente que se encuentra es con un carácter `,`, quiere decir que no hay valor de latitud válido
  
  if ( search_pnt[0] != ',' )
  {

    rmc_sentence.HayLatitud = true;
    rmc_sentence.Latitude.Grados  = AToUint8_t( search_pnt, 2 );
    rmc_sentence.Latitude.Minutos = AToUint8_t( search_pnt + 2, 2 );
    rmc_sentence.Latitude.Decimas = AToUint8_t( search_pnt + 5, 2 ) * 0x0064 + AToUint8_t( search_pnt + 7, 2 );
  }

  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
  if ( true == rmc_sentence.HayLatitud )
  {

    if ( search_pnt[0] == 'N' )
    {
      rmc_sentence.LatiDirection = eNORTH;
    }
    else if ( search_pnt[0] == 'S' )
    {
      rmc_sentence.LatiDirection = eSOUTH;
    }
  }

  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
  if ( search_pnt[0] != ',' )
  {

    rmc_sentence.HayLongitud = true;
    rmc_sentence.Longitude.Grados  = AToUint8_t( search_pnt, 3);
    rmc_sentence.Longitude.Minutos = AToUint8_t( search_pnt + 3, 2);
    rmc_sentence.Longitude.Decimas = AToUint8_t( search_pnt + 6, 2 ) * 0x0064 + AToUint8_t( search_pnt + 8, 2 );
  }
 // $GPRMC,102736.420,A,4245.033333,N,02045.033333,W,1.62,125,211124,1,E,A*23
  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
  if ( true == rmc_sentence.HayLongitud )
  {

    if ( search_pnt[0] == 'W' )
    {
      rmc_sentence.LongDirection = eWEST;
    }
    else if ( search_pnt[0] == 'E' )
    {
      rmc_sentence.LongDirection = eEAST;
    }
  }

// $GPRMC,102736.420,A,4245.033333,N,02045.033333,W, 1.62 ,125,211124,1,E,A*23

#if !DEBUGGING_BB_IS_ON
  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;

  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;

  // La fecha
  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
  rmc_sentence.Date.Dia  = AToUint8_t( search_pnt, 2 );
  rmc_sentence.Date.Mes  = AToUint8_t( search_pnt + 2, 2 );
  rmc_sentence.Date.Anyo = AToUint8_t( search_pnt + 4, 2 );
#endif

  // because the rmc time quite often is very accurate even without a proper 
  // lock we can check on that in case...
  // RMC_TIME_IS_VALID
  
  if((RMC_TIME_IS_VALID == true) && (l_flg == true))
  {
    convert_utc_to_gps_rtc_time();
    
    if(check_validity_of_time_diff() == true)
    {
      eRTC_clock_sync_to_gps(gps_rtc_time);
      GPS_HAS_SYNCED = true;
    }
  }
  else if(UART_GPS_FLG.rmc_time_is_good == TRUE)
  {
    convert_utc_to_gps_rtc_time();

#if DB_V69_PCB    
    UART_CRLF;
    DB_PRINT(sentence_buffer.gps_buffer);
    UART_CRLF;
    r_cnt = 0;
#endif    

  }
  
  
#if DEBUGGING_BB_IS_ON&&1
  
  r_cnt++;
  
  if( r_cnt >= 10)
  {
    r_cnt = 0;
    send_bb_string("\r\n");
    send_bb_string(sentence_buffer.gps_buffer);
    send_bb_string("\r\n");
  }
  
#endif    

  
  
}






#endif


#if DEBUGGING_BB_IS_ON&&0

static uint8_t check_validity_of_time_diff(void){
  
  // #define MAX_PERMITTED_DELTA 18u
  
  // uint32_t delta;
  // uint32_t get_rtc = eRTC_get_second_cnt();
  
  // if(get_rtc > gps_rtc_time)
  // {
    // delta = get_rtc - gps_rtc_time;
  // }
  // else
  // {
    // delta = gps_rtc_time - get_rtc;
  // }
  
  // if((delta <= MAX_PERMITTED_DELTA) || (delta >= (SECONDS_PER_DAY - MAX_PERMITTED_DELTA)))
  // {
    // return true;
  // }
  
  return false;

}

#else


static uint8_t check_validity_of_time_diff(void){
  
  #define MAX_PERMITTED_DELTA 18u
  
  uint32_t delta;
  uint32_t get_rtc = eRTC_get_second_cnt();
  
  if(get_rtc > gps_rtc_time)
  {
    delta = get_rtc - gps_rtc_time;
  }
  else
  {
    delta = gps_rtc_time - get_rtc;
  }
  
  if((delta <= MAX_PERMITTED_DELTA) || (delta >= (SECONDS_PER_DAY - MAX_PERMITTED_DELTA)))
  {
    return true;
  }
  
  return false;

}

#endif



static void copy_rmc_to_from(RMC_sentence_t *const des_pnt,  RMC_sentence_t const *const src_pnt){
  
  if (des_pnt == NULL || src_pnt == NULL)
  {
    return; // Handle null pointers gracefully
  }

  // Use memcpy to copy the entire structure
  memcpy(des_pnt, src_pnt, sizeof(RMC_sentence_t));
  

}












static void Debugging_read_out_rmc(void){
  
#if 0 
 
  //DB_PRINT("RMC Long: %d %d %d \r\n", rmc_sentence.Longitude.Grados, rmc_sentence.Longitude.Minutos, rmc_sentence.Longitude.Decimas)
  
  //DB_PRINT("RMC Lat: %d %d %d \r\n", rmc_sentence.Latitude.Grados, rmc_sentence.Latitude.Minutos, rmc_sentence.Latitude.Decimas)



  //DB_PRINT("RMC Date: %d %d %d \r\n", rmc_sentence.Date.Dia, rmc_sentence.Date.Mes , rmc_sentence.Date.Anyo);
  
  //DB_PRINT("RMC Time: %d %d %d \r\n\r\n", rmc_sentence.UtcOfPosition.Horas, rmc_sentence.UtcOfPosition.Minutos, rmc_sentence.UtcOfPosition.Segundos);
  
  #endif
  
   
  
}






uint8_t gps_rmc_time_is_good_test(void){
  
  return UART_GPS_FLG.rmc_time_is_good;
  
}

void toggle_uart_readout_gps_sentence(void){
  
  UART_GPS_FLG.debugging_send_sentences = !UART_GPS_FLG.debugging_send_sentences;
  
  
  
}

void gps_set_debbugging_sync_time_flg(void){
  
  UART_GPS_FLG.debugging_sync_time = true;
  
}

#if DEBUGGING_BB_IS_ON

#if 1
static void send_gps_direct(void){
  
  return;
  
}

#else


static void send_gps_direct(void){

  LED = true;
  
  start_timeout_tmr();
  
  while(TIMEOUT_FLG == false)
  {

    BB_DIRECT = UART_RX_PC;

  }
  stop_timeout_tmr();
  LED = false;
  send_bb_string(" . . .done\r\n");
  
}
#endif

#endif






// ******************************************  EOC  *******************************************************************************************************************
// ******************************************  EOC  *******************************************************************************************************************
// ******************************************  EOC  *******************************************************************************************************************



#if 0


// A struct we use for the extracted GPS data --> writes into this struct and uses these fields to 
// present then on the GUI
struct udt_gps{
	char time[9];	// 8 digits + NULL_TERMINATOR	// "hh:mm:ss"
	char validez[2];	// 1 digit + 
	char current_Latitude[16];
	char north_south[2];
	char current_Longitude[17];
	char cl_east_west[2];
	char speed_in_knots[6];
	char real_heading[6];	// i shall not use true as a wording because of true false defines
	char date[9];		// "dd.mm.yy"
	char variation[9];
	char var_east_west[2];
};

// "dd.mm.yy" "hh:mm:ss"

// static struct udt_gps GPS_data;
static struct udt_gps GPS_data;



// hardcoding because that is the fastes to develop and in execution-->
//  and the sentence RMC should nbot chage for another 200 years i reckon
static void move_data_to_GPS_struct(void){
uint8_t hlooper = 0;
uint8_t *str_pnt;
const char doppelpunkt = 58;	// char *doppelpunkt[] = ":";
const char punkt = 46;	// char punt[] = ".";
const char beistrich = 44;	// char beistrich[] = ",";
const char cSpace = 32;
const char cDegree = 176;
char *d_pnt;	// destination_pnt
uint8_t bstrich_cnt = 0;
// uint8_t t_looper = 0; // a helping var for testing loop of 2 for the dot in date...
// 0 1 2 3 4 5 6 7 8 9 10 11
// $ G P R M C , 0 8 1 8 3 6 , A , 3 7 5 1 . 6 5 , S , 1 4 5 0 7 . 3 6 , E , 0 0 0 . 0 , 3 6 0 . 0 , 1 3 0 9 9 8 , 0 1 1 . 3 , E * 62

//					1		 2     3   4     5    6    7     8     9     10  11
// $GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E*62
// $GPRMC,081 836,A,3751 .65,S,1450 7.36,E,000 .0,360.0,1 30998,011. 3,E*62
// $GPRMC,225 446,A,4916 .45,N,1231 1.12,W,000 .5,054.7,1 91194,020. 3,E*68
//      0				10			 20					30					40				50					60			
// $GPRMC,075510.700,V,,,,,0.00,0.00,120623,,,N,V*32

	

	
	// lets check first on the data integrity so that there are at least the 10 ','...
	str_pnt = &GPS_Rx_data;
	while(bstrich_cnt < 10 && hlooper < MAX_SENTENCE_LENGTH){
		if(*str_pnt == beistrich){
			bstrich_cnt++;
		}
		str_pnt++;
		hlooper++;
	}
	if(bstrich_cnt >= 10){
		
		str_pnt = &GPS_Rx_data;	// set to the first character...
		d_pnt = &GPS_data.time;
		bstrich_cnt = 0;
		while(bstrich_cnt < 1){
			if(*str_pnt == beistrich){
				bstrich_cnt++;
			}
			str_pnt++;
		}
	// ***************************   GPS_time	/1/  ************************  //
		*d_pnt = *str_pnt;
		d_pnt++;
		str_pnt++;
		*d_pnt = *str_pnt;
		d_pnt++;
		str_pnt++;
		*d_pnt = doppelpunkt;	// we have "hh:"
		d_pnt++;
		*d_pnt = *str_pnt;
		d_pnt++;
		str_pnt++;
		*d_pnt = *str_pnt;
		d_pnt++;
		str_pnt++;
		*d_pnt = doppelpunkt;	// we have "hh:mm:"
		d_pnt++;
		*d_pnt = *str_pnt;
		d_pnt++;
		str_pnt++;
		*d_pnt = *str_pnt;	// we have "hh:mm:ss"
		d_pnt++;
		*d_pnt = NULL_TERMINATOR;
		
	// ***************************   GPS_validez	/2/  ************************  //	
		str_pnt = &GPS_Rx_data;	// set to the first character...	
		d_pnt = &GPS_data.validez;
		bstrich_cnt = 0;
		while(bstrich_cnt < 2){
			if(*str_pnt == beistrich){
				bstrich_cnt++;
			}
			str_pnt++;
		}
			
		*d_pnt = *str_pnt;
		d_pnt++;
		*d_pnt = NULL_TERMINATOR;
		
	// ***************************   GPS_current Latidtude  /3/	  ************************  //	
		
		str_pnt = &GPS_Rx_data;	// set to the first character...
		d_pnt = &GPS_data.current_Latitude;	
		bstrich_cnt = 0;
		while(bstrich_cnt < 3){
			if(*str_pnt == beistrich){
				bstrich_cnt++;
			}
			str_pnt++;
		}
		
		#if 1
	// change the for loop to a while loop...?
		hlooper = 0;
		while(hlooper < 7 && *str_pnt != beistrich){
			*d_pnt = *str_pnt;
			d_pnt++;
			str_pnt++;
			if(hlooper == 1){
				*d_pnt = cDegree;
				d_pnt++;
				*d_pnt = cSpace;
				d_pnt++;
			}
				
				
			hlooper++;
		}
	#else	
		for(hlooper = 0; hlooper < 7; hlooper++){
			*d_pnt = *str_pnt;
			d_pnt++;
			str_pnt++;
		}
	#endif
	// we add a space inbetween...
		*d_pnt = cSpace;
		d_pnt++;
		
		
	// we got now the first seven characters...we are missing N or S
		str_pnt = &GPS_Rx_data;	// set to the first character...
		
		bstrich_cnt = 0;
		while(bstrich_cnt < 4){
			if(*str_pnt == beistrich){
				bstrich_cnt++;
			}
			str_pnt++;
		}
		*d_pnt = *str_pnt;
		d_pnt++;
		*d_pnt = NULL_TERMINATOR;
		
	// ***************************   GPS_current Longitude	/5/  ************************  //	

		str_pnt = &GPS_Rx_data;	// set to the first character...
		d_pnt = &GPS_data.current_Longitude;	
		bstrich_cnt = 0;
		while(bstrich_cnt < 5){
			if(*str_pnt == beistrich){
				bstrich_cnt++;
			}
			str_pnt++;
		}
		
		#if 1
		hlooper = 0;
		while(hlooper < 8 && *str_pnt != beistrich){
			*d_pnt = *str_pnt;
			d_pnt++;
			str_pnt++;
			if(hlooper == 2){
				*d_pnt = cDegree;
				d_pnt++;
				*d_pnt = cSpace;
				d_pnt++;
			}
			hlooper++;
		}
		#else

		
		for(hlooper = 0; hlooper < 8; hlooper++){
			*d_pnt = *str_pnt;
			d_pnt++;
			str_pnt++;
		}
		#endif
	// we add a space inbetween...
		*d_pnt = cSpace;
		d_pnt++;
		
		// we got now the first seven characters...we are missing N or S
		str_pnt = &GPS_Rx_data;	// set to the first character...	
		bstrich_cnt = 0;
		
		while(bstrich_cnt < 6){
			if(*str_pnt == beistrich){
				bstrich_cnt++;
			}
			str_pnt++;
		}
		*d_pnt = *str_pnt;
		d_pnt++;
		*d_pnt = NULL_TERMINATOR;
		
		
	// ***************************   GPS_current speed  /7/	  ************************  //	

		str_pnt = &GPS_Rx_data;	// set to the first character...
		d_pnt = &GPS_data.speed_in_knots;	
		bstrich_cnt = 0;
		
		while(bstrich_cnt < 7){
			if(*str_pnt == beistrich){
				bstrich_cnt++;
			}
			str_pnt++;
		}
		
		#if 1
		hlooper = 0;
		while(hlooper < 5 && *str_pnt != beistrich){
			*d_pnt = *str_pnt;
			d_pnt++;
			str_pnt++;
			hlooper++;
		}
		#else
		
		for(hlooper = 0; hlooper < 5; hlooper++){
			*d_pnt = *str_pnt;
			d_pnt++;
			str_pnt++;
		}
	#endif
		*d_pnt = NULL_TERMINATOR;
		
	// ***************************   GPS_real heading  /8/	  ************************  //	
		str_pnt = &GPS_Rx_data;	// set to the first character...
		d_pnt = &GPS_data.real_heading;	
		bstrich_cnt = 0;
		
		while(bstrich_cnt < 8){
			if(*str_pnt == beistrich){
				bstrich_cnt++;
			}
			str_pnt++;
		}
		
		#if 1
		hlooper = 0;
		while(hlooper < 5 && *str_pnt != beistrich){
			*d_pnt = *str_pnt;
			d_pnt++;
			str_pnt++;
			hlooper++;
		}
		#else
		for(hlooper = 0; hlooper < 5; hlooper++){
			*d_pnt = *str_pnt;
			d_pnt++;
			str_pnt++;
		}
		#endif
		*d_pnt = NULL_TERMINATOR;	
		
	// ***************************   GPS_date  /9/	  ************************  //	
		str_pnt = &GPS_Rx_data;	// set to the first character...
		d_pnt = &GPS_data.date;	
		bstrich_cnt = 0;
		
		while(bstrich_cnt < 9){
			if(*str_pnt == beistrich){
				bstrich_cnt++;
			}
			str_pnt++;
		}
		#if 1
		hlooper = 0;
		// t_looper = 0;
		while(hlooper < 6 && *str_pnt != beistrich){
			*d_pnt = *str_pnt;
			d_pnt++;
			str_pnt++;
			hlooper++;
			// t_looper++;
			if(hlooper == 2 || hlooper == 4){	// that adds a point ibetween to distinguish between month, day, year
				*d_pnt = punkt;
				d_pnt++;
			}
		}
		#else
		for(hlooper = 0; hlooper < 6; hlooper++){
			*d_pnt = *str_pnt;
			d_pnt++;
			str_pnt++;
		}
	#endif
		*d_pnt = NULL_TERMINATOR;	
		

	// ***************************   GPS_variation  /10/	  ************************  //	
		str_pnt = &GPS_Rx_data;	// set to the first character...
		d_pnt = &GPS_data.variation;	
		bstrich_cnt = 0;
		
		while(bstrich_cnt < 10){
			if(*str_pnt == beistrich){
				bstrich_cnt++;
			}
			str_pnt++;
		}	
			#if 1
		hlooper = 0;
		while(hlooper < 7 && *str_pnt != beistrich){
			*d_pnt = *str_pnt;
			d_pnt++;
			str_pnt++;
			hlooper++;
		}
		#else
		for(hlooper = 0; hlooper < 7; hlooper++){
			*d_pnt = *str_pnt;
			d_pnt++;
			str_pnt++;
		}
	#endif


		*d_pnt = NULL_TERMINATOR;	
		
	}


	// UART_GPS_FLG.rmc_time_is_good = true;

	


}






// we are pre loading the GPS structures with values so that there is always something to show
void load_GPS_struct(){
const char doppelpunkt = 58;	// char *doppelpunkt[] = ":";
const char punkt = 46;	// char punt[] = ".";
const char beistrich = 44;	// char beistrich[] = ",";


#if 1

	strcpy(GPS_data.time, "--:--:--");
	strcpy(GPS_data.validez, "-"); 
	strcpy(GPS_data.current_Latitude, "---.--, -");
	strcpy(GPS_data.current_Longitude, "----.--, -"); 
	strcpy(GPS_data.speed_in_knots, "---.-");
	strcpy(GPS_data.real_heading, "---.-");
	strcpy(GPS_data.date, "--.--.--");
	
	
	
#else

	strcpy(GPS_data.time, "02:10:16");
	strcpy(GPS_data.validez, "V"); 
	strcpy(GPS_data.current_Latitude, "000.00, N");
	// strcpy(GPS_data.north_south, 
	strcpy(GPS_data.current_Longitude, "0000.00, E"); 
	// strcpy(GPS_data.cl_east_west, 
	strcpy(GPS_data.speed_in_knots, "000.0");
	strcpy(GPS_data.real_heading, "000.0");
	strcpy(GPS_data.date, "26.01.22");
	// strcpy(GPS_data.variation, 
	// strcpy(GPS_data.var_east_west, 
	
#endif	
	
}





uint8_t *gps_get_pnt_to_gps_data_member(uint8_t member_id){
uint8_t *ret_value;
	
	switch(member_id)
	{
		case e_TIME:
			ret_value = &GPS_data.time;
		break;
		case e_VALIDEZ:
			ret_value = &GPS_data.validez;
		break;
		case e_CURRENT_LATITUDE:
			ret_value = &GPS_data.current_Latitude;
		break;
		case e_NORTH_SOUTH:
			ret_value = &GPS_data.north_south;
		break;
		case e_CURRENT_LONGITUDE:
			ret_value = &GPS_data.current_Longitude;
		break;
		case e_CL_EAST_WEST:
			ret_value = &GPS_data.cl_east_west;
		break;
		case e_SPEED_IN_KNOTS:
			ret_value = &GPS_data.speed_in_knots;
		break;
		case e_REAL_HEADING:
			ret_value = &GPS_data.real_heading;
		break;
		case e_DATE:
			ret_value = &GPS_data.date;
		break;
		case e_VARIATION:
			ret_value = &GPS_data.variation;
		break;
		case e_VAR_EAST_WEST:
			ret_value = &GPS_data.var_east_west;
		break;
		default:
		ret_value = NULL;
		break;
	}
	
	return ret_value;
}

#endif





// EOF
