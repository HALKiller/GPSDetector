//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //

#include "gps.h"

#include "gps_extensions.h"

#include "extension_strings.h"

#include "Global.h"

#include "io_port_sfr_names.h"

#include "device_driver_config.h"

// #include "common_utils.h" // for the segger debugger...

#include  "UART.h"

#include "timers.h"

// #include "my_ioport_bsp_cfg.h"

#include "gd_states.h"

// #include "leds.h"

#include "handlers.h"

#include "e_rtc.h"

#include "my_assert.h"

#include "extension_strings.h"

// #include <stdint.h>
// #include <stdbool.h>
#include <string.h>




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

    unsigned timeout_tmr_is_running   : 1;
    unsigned rtc_test_first_run       : 1;
		unsigned free										  : 4;		
	};
};

static union udt_UART_GPS_FLGS UART_GPS_FLG;

static uint32_t gps_rtc_time = 0;

typedef struct udt_gps_type{
  
  union udt_UART_GPS_FLGS UART_GPS_FLG;
  uint8_t gps_state;
  uint8_t baudslot;
  uint8_t sentence_id;
  
}gps_t;

static gps_t gps_inst;

static gps_sentence_t sentence_buffer;

static RMC_sentence_t rmc_sentence;

static GSA_sentence_t gsa_sentence;


static const uint32_t gps_standard_baud_rate_settings[] = {
  
  4800,
  9600,
  57600,
  115200,
  
};



//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 

 
#if 1
 

static const char *sentences[] = {
  
  "$GPRMC",
  "$GNRMC",
  "$GPGSA",
  "$GNGSA",
  "$EESLf",
  "$EESLR",
  
};

#else
 
const uint8_t c_GPRMC[] = "$GPRMC";
const uint8_t c_GNRMC[] = "G$NRMC";
const uint8_t c_GPGSA[] = "GP$GSA";


const uint8_t *const sentences[] = {
  
  c_GPRMC,
  c_GNRMC,
  c_GPGSA,
  
};

#endif

//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 
#define GPS_PRINT 0 
 
 
// #define	RX_DATA_SIZE_GPS	64	// we expect 64 bytes...this is the buffer size basically....
#if USE_REDUCED_RAM

#define MAX_DATA_LENGTH_GPS_SENTENCE 15

#else

#define MAX_DATA_LENGTH_GPS_SENTENCE 82

#endif


#if MY_DEBUGGING_IS_ON

#define MAXIMUM_RECONFIGURATIONS_PER_ACTIVATION 1

#else
  #define MAXIMUM_RECONFIGURATIONS_PER_ACTIVATION 4
#endif


#define const_STARTWORDCOUNT_LEN 6


//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //

// *********************************************  G P S   B U F F E R   *****************************************  //
// *********************************************  G P S   B U F F E R   *****************************************  //



// uint8_t GPS_Rx_data[MAX_SENTENCE_LENGTH + 3]; // + 1 because we add a '\r\n\0'

static const uint8_t finisher[] = {"\r\n"};


#if 1



static uint8_t *src_buff_pnt = &sentence_buffer.gps_buffer[0];

static const uint8_t cMax_Sentence_length_GPS = MAX_DATA_LENGTH_GPS_SENTENCE;

// static uint8_t temp_buffer[MAX_DATA_LENGTH_GPS_SENTENCE + 1];	// max.length of any NMEA is 82 + 1 '/n'

static uint8_t *temp_buff_pnt = &sentence_buffer.gps_buffer[0];

static uint8_t temp_buff_pnt_cnt = MAX_DATA_LENGTH_GPS_SENTENCE;  // cMax_Sentence_length_GPS;

static uint8_t endbyte_cnt = 2;




#endif

// *********************************************  G P S   B U F F E R   E N D   *****************************************  //
// *********************************************  G P S   B U F F E R   E N D   *****************************************  //


//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //


static void try_reconfigure_gps(void);

static void gps_reconfigure_uart(uint8_t slotter);
static void gps_uart_stop(void);

// static uint8_t check_against_header(char *const t_buffer);
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

// static void move_data_to_GPS_struct(void);



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
  
  // NONE
  UART_GPS_FLG.rtc_test_first_run = FALSE;
 
 
 
}


uint32_t gps_rtc_get_second_cnt(void){
  
  bool temp_GIE = GLOBAL_IE;

	GIE = false;

  uint32_t ret_val = gps_rtc_time;
  
  GIE = temp_GIE;
  
  return ret_val;
  
}

void gps_startup_initializer(void){

  // switch on the uart valim pin
  // reset the uart buffer --> that should get perhaps on the initializer of the uart !

  reset_uart_handler_flags();
  
  gps_inst.baudslot = 0u;
  
  gps_reinit();
 
}


void gps_reinit(void){

#if DEBUGGING_IS_ON    
  uint8_t h_flg = 0;
  
  if(UART_GPS_FLG.debugging_send_sentences == TRUE)
  {
    h_flg = 1u;
  }
  
  if(UART_GPS_FLG.debugging_sync_time == TRUE)
  {
    h_flg = h_flg + 2u;
  }

#endif
  
  UART_GPS_FLG.reg = 0u; // reset everything

#if DEBUGGING_IS_ON    
  UART_GPS_FLG.reg = UART_GPS_FLG.reg + h_flg;
#endif  
  // this resets the temp_buffer and temp_buffer_pointer...
  reset_uart_handler_flags();
  
  
#if 0  
  if(UART_GPS_FLG.debugging_send_sentences)
  {
    UART_GPS_FLG.reg = 0u; // reset everything
    UART_GPS_FLG.debugging_send_sentences = true;
  }
  else
  {
    UART_GPS_FLG.reg = 0; // reset everything;
  }
#endif 
  
  CREN = TRUE;
  RX_IE = TRUE;
  
  
#if USE_DEVICE_DRIVER
// switch on the gps valim pin
  IO_Set_channel(IO_GPS_VALIM);

#else  
  // switch on the gps valim pin
  GPS_VALIM = TRUE;

#endif  
  
  
// Becasue in debug mode the uart is always on becasue we are sending and receivng from there  
#if !DEBUGGING_IS_ON  
  // TODO: init UART and GPS
  // uart_initialize(GPS_UART);
  
#endif  
  
  
#if 1 


  // TODO:
  // reset the uart buffer --> that should get perhaps on the initializer of the uart !
  // tmr_handlers_initialize(GPS_TIME_OUT_TIMER);
  
  // tmr_handlers_start(GPS_TIME_OUT_TIMER);
  
  
  
  UART_GPS_FLG.timeout_tmr_is_running = TRUE;
  
#endif  

}


void gps_stop(void){
   
 
  // stop everything --> therefore : switch off  the uart
  // deinit_uart(GPS_UART);  
  // gps_uart_stop();

  // //DB_PRINT("G: \r\n");
  
#if GPS_PRINT   
//DB_PRINT("GPS_Off\r\n");  
#endif 
  // and switch off the valim_pin for the UART_CRLF

  CREN = FALSE;
  RX_IE = FALSE;
  

  // IO_GPS_VALIM_ON_OFF
  
  
}


// we are checking here to find out:
// receiving all good? best!
// receiving something but not reading quite --> reconfigure UART_CRLF// receive nothing --> fatal!
gps_state_t gps_check_gps_error_status(void){
  
  // tmr_handlers_stop(GPS_TIME_OUT_TIMER);
  
  // tmr_handlers_deinit(GPS_TIME_OUT_TIMER); 
  
  UART_GPS_FLG.timeout_tmr_is_running = FALSE;
  
  if(UART_GPS_FLG.gps_sentence_is_good)
  {
    gps_inst.gps_state = GPS_ALL_GOOD;
#if GPS_PRINT   
    //DB_PRINT("\r\nGPS Works\r\n");
#endif    
  }
  else if(UART_GPS_FLG.receiving_chars_is_good == FALSE)
  {
    
    gps_inst.gps_state = NOT_RECEIVING;
    
    // //DB_PRINT("\r\nGPS FATAL ERROR. No reception\r\n");
    
    try_reconfigure_gps();

  }
  else if(UART_GPS_FLG.valid_header_received == FALSE)
  {
    
    gps_inst.gps_state = RECEIVING_NOT_CORRECTLY;
    
    // //DB_PRINT("\r\nGPS ERROR. Baudrate incorrect\r\n");
    
    try_reconfigure_gps();

  }

  return gps_inst.gps_state;

}

RMC_sentence_t *get_pointer_to_rmc(void){
  
  
  return &rmc_sentence;
  
  
}
//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //

static void try_reconfigure_gps(void){
  
  // this one increases whenever we are having looper 
  // over all gps settings in the array and there was no good sentece...
  static uint8_t maximum_reconfigure_cnt = 0;
  
  // gps_uart_stop();
  
  if(gps_inst.baudslot < (sizeof(gps_standard_baud_rate_settings)/sizeof(gps_standard_baud_rate_settings[0])))     
  {

    // //DB_PRINT("B:\r\n")
  
    gps_uart_stop();

    gps_reconfigure_uart(gps_inst.baudslot);
    
    gps_inst.baudslot++;
    
    gps_reinit();
    
  } 
  else
  {
    if(maximum_reconfigure_cnt < MAXIMUM_RECONFIGURATIONS_PER_ACTIVATION)
    {
      
      // //DB_PRINT("C:\r\n")
    
      gps_uart_stop();
      
      maximum_reconfigure_cnt++;
      
      gps_inst.baudslot = 0;
      
      gps_reconfigure_uart(gps_inst.baudslot);
      
      gps_inst.baudslot++;
      
      gps_reinit();
      
    }
    else
    {
      
      // //DB_PRINT("\r\nGPS ERROR. All Baudrates tryed and no reception!\r\n");
    
      gps_inst.gps_state = NO_BAUDSETTING_WORKS;
      
      
      // TODO: FATAL_ERROR_IN_SYSTEM!!!
      // IO_GPS_VALIM_ON_OFF

      
    }

  } 
  
  
}


static void gps_reconfigure_uart(uint8_t slotter){
  
 // TODO: 
  // uart_modifiy_baudrate(GPS_UART, gps_standard_baud_rate_settings[slotter]);
  
  
}

// becasue we are onmly reconfiguring the baudrate and dont
 // want the GPS to startt all over again...
static void gps_uart_stop(void){
  
  static uint8_t rnd_cnt = 0;
  
  // //DB_PRINT("D:\r\n")
    
  // TODO:
// Stop UART
// Swoff GPS  
    
  // deinit_uart(GPS_UART);  
  
  // //DB_PRINT("E:\r\n")
  
  if(rnd_cnt == 3)
  {
    // _BKPT();
    // TODO:
    
    // R_BSP_SoftwareDelay(250, BSP_DELAY_UNITS_MILLISECONDS);
  }  

   rnd_cnt++; 
   
  
   
   
}




#if 1


//  this fucntion gets called when there has been another char been detected in the ring buffer
// 1. looking for the startbyte of a sentence-->"$" when found set startbyte found  = true
// 2. now looking for the next 5 chars and comparing them to "GPRMC" --> if they coincide we set
// 3. startword found --> therefore, we have the beginning of the correct sentence.
// otherwise reset to looking for the next startbyte.
void values_to_gps_rx_buffer(uint8_t n_char){
	

  static uint8_t startwordcnt = 1;	

#if DEBUGGING_IS_ON&&0
  char db_char[2];
  
  db_char[0] = n_char;
  db_char[1] = NULL_TERMINATOR;

  DB_PRINT(db_char);
#endif  

  DB_LED2_SWAP;

  UART_GPS_FLG.receiving_chars_is_good = TRUE;

#if 1
    *temp_buff_pnt = n_char;
    temp_buff_pnt++;
    temp_buff_pnt_cnt--;
#endif


  if(UART_GPS_FLG.endbyte_found)
  {

    endbyte_cnt--;
    
    if(endbyte_cnt == 0u)
    {
      // we are finished extracting the string!--> now we need to check the sum and if this is also good send it to its 
      if(GPS_checksum_checker(src_buff_pnt, cMax_Sentence_length_GPS - temp_buff_pnt_cnt) == TRUE)
      {
        
        UART_GPS_FLG.gps_sentence_is_good = TRUE;
        // we have now the full string in memory--> therefore we should be able to extract the different sub strings into the GPS_struct...
#if 1

        DB_LED3_SWAP;
#endif        

        sentence_handler(gps_inst.sentence_id);

#if 1
        DB_LED3_SWAP;
        
#endif


#if MY_DEBUGGING_IS_ON
        
        if(UART_GPS_FLG.debugging_send_sentences == TRUE)
        {
          strcat((char*)&sentence_buffer.gps_buffer, (char*)&finisher[0]);
          UWT(DEBUG_UART, sentence_buffer.gps_buffer);
          
          Debugging_read_out_rmc();
 
        }
        
        // rtc_time_got_set
        
        // if( (UART_GPS_FLG.debugging_sync_time == true) && (UART_GPS_FLG.rmc_time_is_good == true) )
          
        if( (UART_GPS_FLG.rmc_time_is_good == TRUE) && ((UART_GPS_FLG.debugging_sync_time == TRUE) || (rtc_time_got_set() == FALSE) ))
        
        {
          UART_GPS_FLG.debugging_sync_time = FALSE;
          rtc_sync_rtc_to_gps_time(get_pointer_to_rmc());
        }
#endif          
      }
#if DEBUGGING_IS_ON      
      else
      {
        if((gps_inst.sentence_id == 4) || (gps_inst.sentence_id == 5))
        {
          
          sentence_handler(gps_inst.sentence_id);
          
        }
          
        // DB_PRINT("\r\n");
        // DB_PRINT(header_buffer);
        DB_PRINT("  CKerr\r\n"); 
        // the chcksum failed!!! we therefore just reset afterwards everything but do not save the received data...
      }
#endif

      reset_uart_handler_flags();
			
    }
  }
  else if(UART_GPS_FLG.startword_found)
  {
    if(n_char == '*')
    {
  // the next startbyte was found and therefore the last 
  // sentence got received completely--> we need to check 
  // the data integrity and if good keep it free for further processing      
      UART_GPS_FLG.endbyte_found = TRUE;	
      
    }


  }
  else if(UART_GPS_FLG.startbyte_found)
  {	

    startwordcnt++;
    
    if(const_STARTWORDCOUNT_LEN == startwordcnt)
    {

      *temp_buff_pnt = NULL_TERMINATOR;

      if(check_against_header((const char *)src_buff_pnt) == TRUE)
        
      {
 
        // there wa a valid header found!
        UART_GPS_FLG.startword_found = TRUE;
        UART_GPS_FLG.valid_header_received = TRUE;
        
      }
      else
      {
        
        reset_uart_handler_flags();
        
      }
    }
  }
  else if(n_char == '$')
  {	
    // if we did not enter so far any of the above if statements we are looking fot the startbyte
    UART_GPS_FLG.startbyte_found = TRUE;	// we have a startbyte!
    startwordcnt = 1u;	// set the startwordcnt 
  }
  else
  {
    reset_uart_handler_flags();

  }
  
  if(temp_buff_pnt_cnt == FALSE)
  {					
// DB_PRINT("Z\r\n");
    // something went seriously wrong because we are on the end of the buffer but have not found the stopbyte-->
    // therefore we need to reset the complete thing and start to keep searching for a "$" startbyte and thats all there is to it...
//---> reset everything for a new search
#if 1

    reset_uart_handler_flags();
    
#endif      
    
  }

#if RUN_ERTC_TEST
// I need this timeout_tmr_is_running flag to avoid race conditions in the handlers	

  if((UART_GPS_FLG.rtc_test_first_run == FALSE) && 
     (UART_GPS_FLG.gsa_position_is_good == TRUE) && (UART_GPS_FLG.rmc_time_is_good == TRUE))

  
  {
    
    DB_PRINT("Syncing\r\n");
    UART_GPS_FLG.rtc_test_first_run = TRUE;
    // convert_utc_to_gps_rtc_time();
    eRTC_clock_sync_to_gps(gps_rtc_time);
    
    // handlers_generic_set_handler_FLG(e_gps_has_full_position_h);
    
    // TODO:
    // rtc_sync_rtc_to_gps_time(get_pointer_to_rmc());
    // and then we should allready switch it off and save the sentence becasue we are all done...
  }
#endif
  // DB_LED3_SWAP;
}




static uint8_t check_against_header(const char *t_buffer){
  
  uint8_t hlooper = 0;
  uint8_t ret_value = false;
  
  for(hlooper = 0; hlooper < sizeof(sentences) / sizeof(sentences[0]); hlooper++)
  {
    if(strcmp(( char *)t_buffer, ( char *) sentences[hlooper]) == false)
    {
      ret_value = true;
      
      gps_inst.sentence_id = hlooper;
      
      break;
    }
  }
  
  if(ret_value == false)
  {
    
  }

  return ret_value;
  
  
}

#else

void values_to_gps_rx_buffer(uint8_t n_char){
  
  
}


static uint8_t check_against_header(char *const t_buffer){
  
  return true;
  
}

#endif

#if 1

static void reset_uart_handler_flags(void){
  
  // DB_PRINT("\r\nUR\r\n");
  
  UART_GPS_FLG.startbyte_found = FALSE;
  UART_GPS_FLG.startword_found = FALSE;
  UART_GPS_FLG.endbyte_found = FALSE;
  // temp_buff_pnt = temp_buffer;
  temp_buff_pnt = &(sentence_buffer.gps_buffer[0]);
  temp_buff_pnt_cnt = cMax_Sentence_length_GPS;
  endbyte_cnt = 2u;
  
  
  
  
}

#endif




// takes a pointer to the first character, the length of the sentence inclusive checksum delimiter and checksum
// returns true if check is good or false if it does not sum up
uint8_t GPS_checksum_checker(uint8_t *d_pnt, uint8_t d_length){
	
#define KLEIN_GROSS_SCHREIBUNG	0xDF
	
#define SEND_GPS_UART_DB_DATA	0	
uint8_t hlooper = 0;
uint8_t chcksum = 0x00;
uint8_t received_chcksum = 0;
uint8_t temp_char = 0;


#if SEND_GPS_UART_DB_DATA
	UART_CRLF;
	UWT("***********************************************");
	UART_CRLF;
	UWT("Checksum Function Start:");
	
	UWT("Data_length: ");
	UART_int(d_length);
	UWC(*d_pnt);
#endif	

	d_pnt++;
	for(hlooper = 1; hlooper < (d_length - 3); hlooper++){
		// UWC(*d_pnt);
		chcksum = chcksum ^ *d_pnt;
		d_pnt++;	
	}
	d_pnt++;
	
	// that converts to the same output independent of mayuscula or not
	if(*d_pnt > 70){
		temp_char = (*d_pnt & KLEIN_GROSS_SCHREIBUNG)  - 0x30;
	}else{
		temp_char = *d_pnt  - 0x30;
	}
	// temp_char = (*d_pnt & KLEIN_GROSS_SCHREIBUNG)  - 0x30;
	// temp_char = *d_pnt  - 0x30;
	
	if(temp_char > 9){
		temp_char = temp_char - 7;	
	}
#if SEND_GPS_UART_DB_DATA	
	UWT("received high nibble: ");
	UWC(*d_pnt);
	UART_CRLF;
	UART_int(temp_char);
#endif	
	
	received_chcksum = (uint8_t)(temp_char << 4);
  
	d_pnt++;
	// temp_char = (*d_pnt & KLEIN_GROSS_SCHREIBUNG) - 0x30;
	
	if(*d_pnt > 70){
		temp_char = (*d_pnt & KLEIN_GROSS_SCHREIBUNG)  - 0x30;
	}else{
		temp_char = *d_pnt  - 0x30;
	}
	
	
	// temp_char = *d_pnt  - 0x30;	
	if(temp_char > 9){
		temp_char = temp_char - 7;	
	}
	
#if SEND_GPS_UART_DB_DATA	
	UWT("received low nibble: ");
	UWC(*d_pnt);
	UART_CRLF;
	UART_int(temp_char);
#endif	
	
	
	received_chcksum = received_chcksum + temp_char;

#if	SEND_GPS_UART_DB_DATA	// SEND_GPS_UART_DB_DATA	
	UWT("Calculated chcksum: ");
	UART_int(chcksum);
	UWT("Received chcksum: ");
	UART_int(received_chcksum);	
#endif	

	if(received_chcksum == chcksum){
		return true;
	}else{
		return false;
	}
	

}



#if DEBUGGING_IS_ON

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
    case 5:
      UART_GPS_FLG.rtc_test_first_run = FALSE;
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
    
    default:
      assert(false);
    break;

  }
  
}

#endif


static void process_gsa_sentence(void){
  
  uint8_t *search_pnt;
  // selectortrama = 0x04; // 0b0000 0100
			
  // if ( gNoProcesaMasGsa == false )
  // {
    
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
    //UART_WRITE('S');

  // }
  
  
}


static void convert_utc_to_gps_rtc_time(void){
  
  gps_rtc_time = rmc_sentence.UtcOfPosition.Horas * SECONDS_PER_HOUR;
  gps_rtc_time = gps_rtc_time + rmc_sentence.UtcOfPosition.Minutos * SECONDS_PER_MINUTE; 
  gps_rtc_time = (gps_rtc_time + rmc_sentence.UtcOfPosition.Segundos);
  
  
  
}



#if 0

static void process_rmc_sentence(void){
  
  
}
#else

static void process_rmc_sentence(void){
  
  uint8_t *search_pnt;
      // selectortrama = 0x10; // 0b0001 0000
  search_pnt = Uint8_tStrchr( sentence_buffer.gps_buffer, ',' ) + 1;
  
  UART_GPS_FLG.rmc_time_is_good = false;
  
 // El primer valor que se encuentra es la hora UTC, que siempre aparece en todo tipo de tramas RMC
  rmc_sentence.UtcOfPosition.Horas    = AToUint8_t( search_pnt + 0, 2 );
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
  

  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
  // Si lo siguiente que se encuentra es con un carácter `,`, quiere decir que no hay valor de latitud válido
  if ( search_pnt[0] == ',' )
  {
    rmc_sentence.HayLatitud = false;
  }
  else
  {
    rmc_sentence.HayLatitud = true;
    rmc_sentence.Latitude.Grados  = AToUint8_t( search_pnt + 0, 2 );
    rmc_sentence.Latitude.Minutos = AToUint8_t( search_pnt + 2, 2 );
    rmc_sentence.Latitude.Decimas = AToUint8_t( search_pnt + 5, 2 ) * 0x0064 + AToUint8_t( search_pnt + 7, 2 );
  }

  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
  if ( true == rmc_sentence.HayLatitud )
  {
//          rmc_sentence.LatiDirection = (enum eLatitude) ( search_pnt + 1 )[0] == 'S';
    if ( search_pnt[0] == 'N' )
    {
      rmc_sentence.LatiDirection = N;
    }
    else if ( search_pnt[0] == 'S' )
    {
      rmc_sentence.LatiDirection = S;
    }
  }

  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
  if ( search_pnt[0] == ',' )
  {
    rmc_sentence.HayLongitud = false;
  }
  else
  {
    rmc_sentence.HayLongitud = true;
    rmc_sentence.Longitude.Grados  = AToUint8_t( search_pnt + 0, 3);
    rmc_sentence.Longitude.Minutos = AToUint8_t( search_pnt + 3, 2);
    rmc_sentence.Longitude.Decimas = AToUint8_t( search_pnt + 6, 2 ) * 0x0064 + AToUint8_t( search_pnt + 8, 2 );
  }

  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
  if ( true == rmc_sentence.HayLongitud )
  {
//          rmc_sentence.LongDirection = (enum eLongitude) ( search_pnt + 1 )[0] == 'W';
    if ( search_pnt[0] == 'W' )
    {
      rmc_sentence.LongDirection = W;
    }
    else if ( search_pnt[0] == 'E' )
    {
      rmc_sentence.LongDirection = E;
    }
  }

  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
  if ( search_pnt[0] != ',' )
  {
    rmc_sentence.SpeedOvertheGround.ParteEntera  = AToUint8_t( search_pnt + 0, Uint8_tStrchr( search_pnt, '.' ) - search_pnt );
    search_pnt = Uint8_tStrchr( search_pnt, '.' ) + 1;
    rmc_sentence.SpeedOvertheGround.ParteDecimal  = AToUint8_t( search_pnt + 0, Uint8_tStrchr( search_pnt, ',' ) - search_pnt );
  }

  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
  if ( search_pnt[0] != ',' )
  {
    if ( Uint8_tStrchr( search_pnt, '.' ) - search_pnt <= 2 )
    {
      rmc_sentence.Degrees.ParteEntera = AToUint8_t( search_pnt + 0, Uint8_tStrchr( search_pnt, '.' ) - search_pnt );
    }
    else
    {
      rmc_sentence.Degrees.ParteEntera = AToUint8_t( search_pnt + 0, 1 ) * 0x0064 + AToUint8_t( search_pnt + 1, Uint8_tStrchr( search_pnt, '.' ) - search_pnt - 1 );
    }
    search_pnt = Uint8_tStrchr( search_pnt, '.' ) + 1;
    rmc_sentence.Degrees.ParteDecimal  = AToUint8_t( search_pnt + 0, Uint8_tStrchr( search_pnt, ',' ) - search_pnt );
  }

  // La fecha
  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
  rmc_sentence.Date.Dia  = AToUint8_t( search_pnt + 0, 2 );
  rmc_sentence.Date.Mes  = AToUint8_t( search_pnt + 2, 2 );
  rmc_sentence.Date.Anyo = AToUint8_t( search_pnt + 4, 2 );

  // Variación magnética
  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;

  // Dirección de la variación magnética
  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;

  search_pnt = Uint8_tStrchr( search_pnt, ',' ) + 1;
  switch( search_pnt[0] )
  {
    case 'A':
      rmc_sentence.ModeIndicator = PA;
      break;
    case 'D':
      rmc_sentence.ModeIndicator = PD;
      break;
    case 'E':
      rmc_sentence.ModeIndicator = PE;
      break;
    case 'M':
      rmc_sentence.ModeIndicator = PM;
      break;
    case 'S':
      rmc_sentence.ModeIndicator = PS;
      break;
    case 'N':
      // Fall through (tanto el caso default como N son el mismo)
    default:
      rmc_sentence.ModeIndicator = PN;
      break;
  }

#if 0  
  gSeHaProcesadoRmc = true;
  gNoProcesaMasGsa = false;
#endif

  if(UART_GPS_FLG.rmc_time_is_good == TRUE)
  {
    convert_utc_to_gps_rtc_time();
  }
  

  
  
}

#endif

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
