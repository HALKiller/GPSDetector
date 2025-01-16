// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //
// this is the api for the handling of the tilt sensor ->
// the sensor needs thes input output  and periferics to make it work:
// Comparator


// timing characteristics:
// we need to have a few different timing setups:
// how often do we test the sensor? 
// for how long do we need the sensor to be active to get a stable reading?



//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //

#include <stdint.h>

#include "Global.h"

#include "tilt_sensor.h"

#include "device_driver_config.h"

#include "io_port_sfr_names.h"

#include "gd_states.h"

#include "generic_union_flgs.h"

#include "timers.h"

#if DEBUGGING_IS_ON || DEBUGGING_BB_IS_ON
#include "UART.h"
#endif



//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 

#define USE_NOP 1

#if USE_SPI_TILT

  // TODO: the _delay_us need replacing by tmr2 type of


// S2D Register adresses
#define S2D_OUT_T_L       0x0D
#define S2D_OUT_T_H       0x0E
#define S2D_WHO_AM_I      0x8F  // only read therefore set the bit 7 always 1

#define S2DCTRL0          0x1E

#define S2DCTRL1          0x20
#define S2DCTRL2          0x21
#define S2DCTRL3          0x22
#define S2DCTRL4          0x23
#define S2DCTRL5          0x24
#define S2DCTRL6          0x25

#define S2D_OUT_T         0x26
#define S2D_STATUS        0x27
#define S2D_STATUS_READ   0xA7

#define S2D_OUT_XL        0x28
#define S2D_OUT_READ_XL   0xA8
#define S2D_OUT_XH        0x29
#define S2D_OUT_YL        0x2A
#define S2D_OUT_YH        0x2B
#define S2D_OUT_ZL        0x2C
#define S2D_OUT_ZH        0x2D
#define S2D_OUT_READ_ZH   0xAD

#define S2D_FIFO_CTRL     0x2E
#define S2D_FIFO_SAMPLES  0x2F

#define S2D_TAP_THS_X     0x30
#define S2D_TAP_THS_Y     0x31
#define S2D_TAP_THS_Z     0x32
#define S2D_INT_DUR       0x33

#define S2D_WAKE_UP_THS   0x34
#define S2D_WAKE_UP_DUR   0x35

#define S2D_FREE_FALL     0x36
#define S2D_STATUS_DUP    0x37
#define S2D_WAKE_UP_SRC   0x38
#define S2D_TAP_SRC       0x39
#define S2D_SIXD_SRC      0x3A
#define S2D_ALL_INT_SRC   0x3B
#define S2D_X_OFS_USR     0x3C
#define S2D_Y_OFS_USR     0x3D
#define S2D_Z_OFS_USR     0x3E
#define S2DCTRL7          0x3F 
 
 
#define RESET_VALUE 0x00u
 
 
 
// these defines are needed but that can be existing SPI lines from other peripherics
#if 1

#define BB_SPI_CS   LATCbits.LATC0
#define BB_SPI_CLCK LATCbits.LATC1
#define BB_SPI_SDO  LATCbits.LATC3
#define BB_SPI_SDI  PORTCbits.RC4

#define VALIM_TILT_ON LATBbits.LATB7

#endif

#endif



#define SENSOR_READINGS_PER_SECOND ((uint16_t)5u)
#define CONST_ON_CNT_DEBOUNCED (uint8_t)(TIME_THRESHOLD_FOR_DETECTOR_IS_ON * SENSOR_READINGS_PER_SECOND)
#define CONST_OFF_CNT_DEBOUNCED (uint8_t)(TIME_THRESHOLD_FOR_DETECTOR_IS_OFF * SENSOR_READINGS_PER_SECOND)
 




//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

struct udt_tilt_sensor_type{
  
  tilt_sensor_states_t detector_is_on;
  uint8_t on_cnt;
  uint8_t off_cnt;
  
};

static struct udt_tilt_sensor_type tilt_sensor;


//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
#if USE_SPI_TILT&&0

 const uint8_t shifter[8] = {
	
	(1U << 0), (1U << 1), (1U << 2), (1U << 3), 
	(1U << 4), (1U << 5), (1U << 6), (1U << 7), 

}; 

#endif


//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //
#if USE_SPI_TILT

uint8_t tx_buffer[4] = { 

  S2D_OUT_READ_ZH, 
  RESET_VALUE,

};

uint8_t rx_buffer[4];

#endif

//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //


#if USE_SPI_TILT

static uint8_t get_tilt_data(void);
static void rw_data_bb_spi(uint8_t len_to_send);
static uint8_t spi_transmit(uint8_t data);
static void spi_tilt_timer(uint8_t us_time);


#endif

//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //



void tilt_sensor_init(void){
  
  tilt_sensor.detector_is_on = TS_STARTUP_STATE;
  
  tilt_sensor.on_cnt = 0u;
  
  tilt_sensor.off_cnt = 0u;

}




//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //


#if TILT_IS_ALWAYS_ON

// a count algorithm in function of the last read state --> therefroe we are changoing the state only on 
// count > than threshold. and there can be two different thresholds for up and downcount.
uint8_t update_tilt_sensor_state(void){


#if USE_SPI_TILT
  uint8_t read_state = get_tilt_data();
#else
  uint8_t read_state = TILT_SENSOR;
#endif

  if(tilt_sensor.detector_is_on != TS_ON_STATE)
  {

    tilt_sensor.detector_is_on = TS_ON_STATE;
    gd_states_switch_to_next_state(E_GPS_CHECK_ON_ACTIVATION);
   
  }


}


#else


// a count algorithm in function of the last read state --> therefroe we are changoing the state only on 
// count > than threshold. and there can be two different thresholds for up and downcount.
uint8_t update_tilt_sensor_state(void){

#if USE_SPI_TILT
  uint8_t read_state = get_tilt_data();
#else
  uint8_t read_state = TILT_SENSOR;
#endif

  uint8_t ret_value = false;


	if(read_state == SENSOR_IS_TOP_MOUNTED)
	{
    
    ret_value = true;
    
    if(tilt_sensor.detector_is_on != TS_ON_STATE)
    {
      tilt_sensor.on_cnt++;
      tilt_sensor.off_cnt = (uint8_t)0u;
      
      if(tilt_sensor.on_cnt > CONST_ON_CNT_DEBOUNCED)
      {
        tilt_sensor.detector_is_on = TS_ON_STATE;
        gd_states_switch_to_next_state(E_GPS_CHECK_ON_ACTIVATION);
      }
      
    }

	}
	else
	{
    
    if(tilt_sensor.detector_is_on != TS_OFF_STATE)
    {
      tilt_sensor.off_cnt++;
      tilt_sensor.on_cnt = (uint8_t)0u;
      
      if(tilt_sensor.off_cnt > CONST_OFF_CNT_DEBOUNCED)
      {
        tilt_sensor.detector_is_on = TS_OFF_STATE;
        gd_states_switch_to_next_state(E_OFF_STATE);
      }
      
    }
    

	}
#if 0  
  DB_PRINT("\r\nOn_cnt: ");
  UART_int(tilt_sensor.on_cnt);
#endif	
	return ret_value;
  
}






#endif

#if USE_SPI_TILT




#if 1

// because we are switching off every time we need to configure the sensor anew every time...
static uint8_t get_tilt_data(void){
  
  
 uint8_t temp_PR = PR6;
 
 uint8_t ret_value = 0u; 
 uint8_t val_pos = false;
 uint8_t r_cnt = 0;
  
  // DB_PRINT("\r\nVALIM_ON\r\n");
  
  // configure here the timer 

  VALIM_TILT_ON = true;

  if(FAST_CLOCK == false)  
  {
    __delay_us(625);
    PR6 = 31;
  }
  else
  {
    __delay_ms(5);
    PR6 = 250;
    
  }
  
  T6_POSTSCALER = TMR6_16_POSTSCALER; // 0x01;	
  TMR6 = 0;
  TMR6_IF = false; 
  
  BB_SPI_CS = true;


#if 0
// that switches of the internal pull ups
  tx_buffer[0] = S2DCTRL1;
  tx_buffer[1] = 0x1C;  // 0x10;  // 0x10u;
  tx_buffer[2] = S2DCTRL0;
  tx_buffer[3] = 0x90;  // 0x10;  // 0x10u;
  rw_data_bb_spi(4u);
 
#else
  
  tx_buffer[0] = S2DCTRL1;
  tx_buffer[1] = 0x1C;  // 0x10;  // 0x10u;
  rw_data_bb_spi(2u);
  
#endif   
  
#if 0
  if(FAST_CLOCK == false)  
  {
    __delay_us(125);
  }
  else
  {
    __delay_ms(1);
  }
#endif    
  
  
  
  tx_buffer[0] = S2D_WHO_AM_I;
  tx_buffer[1] = RESET_VALUE;// 0xAA;  // 
  
  rw_data_bb_spi(2u);
  rx_buffer[2] = rx_buffer[1];
  
#if 0 

 

  

  // TODO:
  // check when releas that the TMR6 gets configured
  while(TMR6_IF == false);

  
  
  // this needs to be 68  .d for the IIS2
  // this needs to be 51  .d for the LISDE12
  

#else

  tx_buffer[0] = S2D_STATUS_READ; // S2D_WHO_AM_I;

  tx_buffer[1] = RESET_VALUE;

  TMR6_ON = true;
  
  while((val_pos == false) && (TMR6_IF == false))
  {
    // try reading the status reg zntil we have a valid Z position... 
    rw_data_bb_spi(2u);
    
    if(rx_buffer[1] & 0x04)
    {
      val_pos = true; 
    }
    
  
  }
  
  PR6 = temp_PR;
  T6_POSTSCALER = TMR6_02_POSTSCALER;
  TMR6_ON = false;
  
#endif

  tx_buffer[0] = S2D_OUT_READ_ZH;

  rw_data_bb_spi(2u);

  if(rx_buffer[1] == 0)
  {
    rw_data_bb_spi(2u);
  }

#if 0
  if(FAST_CLOCK == false)  
  {
    VALIM_TILT_ON = false;
  }
#else

  VALIM_TILT_ON = false;
  
#endif
  
  BB_SPI_CS   = false;
  BB_SPI_CLCK = false;
  BB_SPI_SDO  = false;
  
  
#if DEBUGGING_BB_IS_ON&&0
  DB_PRINT("\r\nI am: ");
  UART_int(rx_buffer[2]);
  
  DB_PRINT("\r\nSPI_read: ");
  UART_int(rx_buffer[1]);
#endif


#if 0
  if(FAST_CLOCK == false)  
  {
    __delay_ms(312);
  }
  else
  {
    __delay_ms(2500);
  }
#endif    


#if DEBUGGING_BB_IS_ON&&0
  // becaue that is actually a signed int
  if(rx_buffer[1] > 127)
  {
    DB_PRINT("\r\nDet OFF\r\n");
  }
  else
  {
    DB_PRINT("\r\nDet ON\r\n");
    ret_value = true;
  }
#else

  if(rx_buffer[1] < 128)
  {
    ret_value = true;
  }
  
#endif  

  return ret_value;
  
}


#elif 1
// that one is good...
// because we are switching off every time we need to configure the sensor anew every time...
static uint8_t get_tilt_data(void){
  
  
  
 uint8_t ret_value = 0u; 
 // int8_t hlooper = 0;
  
DB_PRINT("\r\nVALIM_ON\r\n");
  
  // configure here the timer 
  
  
  VALIM_TILT_ON = true;
    
  if(FAST_CLOCK == false)  
  {
    __delay_us(625);
  }
  else
  {
    __delay_ms(5);
  }
  
  
  BB_SPI_CS = true;

#if 0  
// that switches of the internal pull ups
  tx_buffer[0] = S2DCTRL0;
  tx_buffer[1] = 0x90;  // 0x10;  // 0x10u;
  rw_data_bb_spi(2u);
#endif  

  tx_buffer[0] = S2DCTRL1;
  tx_buffer[1] = 0x1C;  // 0x10;  // 0x10u;
  rw_data_bb_spi(2u);
  
  
#if 1
  if(FAST_CLOCK == false)  
  {
    __delay_us(125);
  }
  else
  {
    __delay_ms(1);
  }
#endif    
  
  
  tx_buffer[0] = S2D_WHO_AM_I;
  tx_buffer[1] = RESET_VALUE;// 0xAA;  // 
  
  rw_data_bb_spi(2u);
  
  
  
  // this needs to be 68  .d for the IIS2
  // this needs to be 51  .d for the LISDE12
  rx_buffer[2] = rx_buffer[1];

  tx_buffer[0] = S2D_OUT_READ_ZH;
  tx_buffer[1] = RESET_VALUE;// 0xAA;  // 

  rw_data_bb_spi(2u);

#if 0
  if(FAST_CLOCK == false)  
  {
    VALIM_TILT_ON = false;
  }
#else

  VALIM_TILT_ON = false;
  
#endif
  
  BB_SPI_CS   = false;
  BB_SPI_CLCK = false;
  BB_SPI_SDO  = false;
  
  
  
  DB_PRINT("\r\nI am: ");
  UART_int(rx_buffer[2]);
  
  DB_PRINT("\r\nSPI_read: ");
  UART_int(rx_buffer[1]);
  
  
#if 0
  if(FAST_CLOCK == false)  
  {
    __delay_ms(312);
  }
  else
  {
    __delay_ms(2500);
  }
#endif    


#if DEBUGGING_BB_IS_ON
  // becaue that is actually a signed int
  if(rx_buffer[1] > 127)
  {
    DB_PRINT("\r\nDet OFF\r\n");
  }
  else
  {
    DB_PRINT("\r\nDet ON\r\n");
    ret_value = true;
  }
#else

  if(rx_buffer[1] < 128)
  {
    ret_value = true;
  }
  
#endif  

  return ret_value;
  
}

#elif 1 // here are db lines included
// because we are switching off every time we need to configure the sensor anew every time...
static uint8_t get_tilt_data(void){
  
 uint8_t ret_value = 0u; 
 int8_t hlooper = 0;
  

  DB_PRINT("\r\nValim_on\r\n");
  
  

  
  
  VALIM_TILT_ON = true;
  
  
  BB_SPI_CLCK = true;
  
  __delay_ms(5);
  
  BB_SPI_CS   = true;
  
  
  tx_buffer[0] = S2DCTRL0;
  tx_buffer[1] = 0x90;  // 0x10;  // 0x10u;
  rw_data_bb_spi(2u);
  
  
  
  tx_buffer[0] = S2DCTRL1;
  tx_buffer[1] = 0x1C;  // 0x10;  // 0x10u;
  rw_data_bb_spi(2u);
  
  DB_PRINT("\r\nI am: ");
  
  tx_buffer[0] = S2D_WHO_AM_I;
  tx_buffer[1] = RESET_VALUE;// 0xAA;  // 
  
  rw_data_bb_spi(2u);
  
  // this needs to be 68  .d for the IIS2
  // this needs to be 51  .d for the LISDE12
  
  UART_int(rx_buffer[1]);

  UART_CRLF;
 

#if 1
 
  tx_buffer[0] = S2D_OUT_READ_ZH;
  tx_buffer[1] = RESET_VALUE;// 0xAA;  // 

  rw_data_bb_spi(2u);
  
  
  DB_PRINT("\r\nSPI_read: ");
  UART_int(rx_buffer[1]);
  
  
#else  
  
  
  tx_buffer[0] = S2D_OUT_READ_XL;
  tx_buffer[1] = RESET_VALUE;// 0xAA;  // 
  tx_buffer[2] = RESET_VALUE;// 0x55;  // 
  tx_buffer[3] = RESET_VALUE;// 0xAA;  // 
  tx_buffer[4] = RESET_VALUE;// 0x55;  // 
  tx_buffer[5] = RESET_VALUE;// 0xAA;  // 
  tx_buffer[6] = RESET_VALUE;// 0x55;  // 
  tx_buffer[7] = RESET_VALUE;// 0xAA;  // 
  
  rw_data_bb_spi(8u);
 

  DB_PRINT("\r\nSPI_read: ");
  
  for(hlooper = 0; hlooper < 6; hlooper = hlooper + 2)
  {
    
    UART_int(rx_buffer[hlooper + 2]);
    // DB_PRINT("-");
    
  }
  
#endif
    
  UART_CRLF;
  
#if 0  

  hvar = (int16_t)rx_buffer[5] + 256 * (int16_t)rx_buffer[6];

  force = (int16_t)hvar * 61 / 1000;  // iis2dlpc_from_fs2_to_mg(hvar);
  if(force > 0)
  {
    DB_PRINT("Detector is OFF\r\n");
  }
  else
  {
    DB_PRINT("Detector is ON\r\n");
  }
  
#elif 1
  // becaue that is actually a signed int
  if(rx_buffer[1] > 127)
  {
    
    DB_PRINT("\r\nDetector OFF\r\n");
    
  }
  else
  {
    DB_PRINT("\r\nDetector ON\r\n");
    ret_value = true;
  }
  
#else
  

  if(rx_buffer[6] > 127)

  {
    DB_PRINT("\r\nDetector OFF\r\n");
  }
  else
  {
    DB_PRINT("\r\nDetector ON\r\n");
  }
#endif        
  
  
  VALIM_TILT_ON = false;
  
  BB_SPI_CS   = false;
  BB_SPI_CLCK = false;
  BB_SPI_SDO  = false;
  
  return ret_value;
  
}

#else
  static uint8_t get_tilt_data(void){
  
 uint8_t ret_value = 0u; 
 int8_t hlooper = 0;
  
 int16_t hvar = 0; 
 int16_t force = 0;
  
  
  DB_PRINT("\r\nI am: ");
  
  tx_buffer[0] = S2D_WHO_AM_I;
  tx_buffer[1] = RESET_VALUE;// 0xAA;  // 
  
  rw_data_bb_spi(2u);
  
  // this needs to be 68  .d
  
  UART_int(rx_buffer[1]);

  UART_CRLF;
 

#if 1
 
  tx_buffer[0] = S2D_OUT_READ_ZH;
  tx_buffer[1] = RESET_VALUE;// 0xAA;  // 

  rw_data_bb_spi(2u);
  
  
  DB_PRINT("\r\nSPI_read: ");
  UART_int(rx_buffer[1]);
  
  
#else  
  
  
  tx_buffer[0] = S2D_OUT_READ_XL;
  tx_buffer[1] = RESET_VALUE;// 0xAA;  // 
  tx_buffer[2] = RESET_VALUE;// 0x55;  // 
  tx_buffer[3] = RESET_VALUE;// 0xAA;  // 
  tx_buffer[4] = RESET_VALUE;// 0x55;  // 
  tx_buffer[5] = RESET_VALUE;// 0xAA;  // 
  tx_buffer[6] = RESET_VALUE;// 0x55;  // 
  tx_buffer[7] = RESET_VALUE;// 0xAA;  // 
  
  rw_data_bb_spi(8u);
 

  DB_PRINT("\r\nSPI_read: ");
  
  for(hlooper = 0; hlooper < 6; hlooper = hlooper + 2)
  {
    
    UART_int(rx_buffer[hlooper + 2]);
    // DB_PRINT("-");
    
  }
  
#endif
    
  UART_CRLF;
  
#if 0  

  hvar = (int16_t)rx_buffer[5] + 256 * (int16_t)rx_buffer[6];

  force = (int16_t)hvar * 61 / 1000;  // iis2dlpc_from_fs2_to_mg(hvar);
  if(force > 0)
  {
    DB_PRINT("Detector is OFF\r\n");
  }
  else
  {
    DB_PRINT("Detector is ON\r\n");
  }
  
#elif 1
  // becaue that is actually a signed int
  if(rx_buffer[1] > 127)
  {
    
    DB_PRINT("\r\nDetector OFF\r\n");
    
  }
  else
  {
    DB_PRINT("\r\nDetector ON\r\n");
    ret_value = true;
  }
  
#else
  

  if(rx_buffer[6] > 127)

  {
    DB_PRINT("\r\nDetector OFF\r\n");
  }
  else
  {
    DB_PRINT("\r\nDetector ON\r\n");
  }
#endif        

  return ret_value;
  
}



#endif

// sending first the address and after that the data, 
// furthermore reading back the result in case it was a read instruction
static void rw_data_bb_spi(uint8_t len_to_send){
  
  
  uint8_t *tx_pnt = &tx_buffer[0];
  uint8_t *rx_pnt = &rx_buffer[0];
  int8_t hlooper = 0;
  
  
  BB_SPI_CLCK = true;

  // assert de chip select
  BB_SPI_CS = false;
  
  for(hlooper = 0; hlooper < len_to_send; hlooper++)
  {
    
    *rx_pnt = spi_transmit(*tx_pnt);
    rx_pnt++;
    tx_pnt++;
    
  }
 
  // de assert de chip select
  BB_SPI_CS = true;
 
}


// Function to send a single byte over SPI
static uint8_t spi_transmit(uint8_t data){
  
  uint8_t received = 0;
  int8_t hlooper = 0;
  uint8_t r_shifter = 0x80;
  

  // Loop through each bit of the data
  for (hlooper = 0; hlooper < 8; hlooper++)
  {
    
    // Set the MOSI pin according to the current bit
    if (data & r_shifter)
    {
      BB_SPI_SDO = true;
    }
    else
    {
      BB_SPI_SDO = false;
    }

    // Pulse the clock
    BB_SPI_CLCK = false;
    

    asm("nop");
    asm("nop");

    BB_SPI_CLCK = true;
   

    asm("nop");
    asm("nop");


    // Read the MISO pin if it's connected
    if(BB_SPI_SDI == true)
    { 
      received |= r_shifter; 
    }
    r_shifter = r_shifter >> 1;
  }


  return received;
  
}

#endif  // USE_SPI_TILT

// just a waitning function which takes care with the tmr 6 -->
// becasue of that implementation we will get the correctwaitting time 
// independent of the clockspeed
// PRE = 1:1
// PSA = 2:1 --> that comes from the bit bang uart
static void spi_tilt_timer(uint8_t us_time){
  
  // if clock is fast --> t_inc = 1us
  // else             --> t_inc = 8us 
  
  
  
  


}



// EOF