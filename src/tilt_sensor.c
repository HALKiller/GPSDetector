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

#if DEBUGGING_IS_ON
#include "UART.h"
#endif



//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 

#if USE_SPI_TILT

  // TODO: the _delay_us need replacing by tmr2 type of


// S2D Register adresses
#define S2D_OUT_T_L       0x0D
#define S2D_OUT_T_H       0x0E
#define S2D_WHO_AM_I      0x8F  // only read therefore set the bit 7 always 1

#define S2DCTRL1          0x20
#define S2DCTRL2          0x21
#define S2DCTRL3          0x22
#define S2DCTRL4          0x23
#define S2DCTRL5          0x24
#define S2DCTRL6          0x25

#define S2D_OUT_T         0x26
#define S2D_STATUS        0x27

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
#if USE_SPI_TILT

 const uint16_t shifter[8] = {
	
	(1U << 0), (1U << 1), (1U << 2), (1U << 3), 
	(1U << 4), (1U << 5), (1U << 6), (1U << 7), 

}; 

#endif


//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //
#if USE_SPI_TILT

uint8_t tx_buffer[2] = { 

  S2D_OUT_READ_ZH, 
  RESET_VALUE,

};

uint8_t rx_buffer[2];

#endif

//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //


#if USE_SPI_TILT
static void spi_init(void) ;
static uint8_t get_tilt_data(void);
static void rw_data_bb_spi(uint8_t len_to_send);
static uint8_t spi_transmit(uint8_t data);
#endif

//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //



void tilt_sensor_init(void){
  
  tilt_sensor.detector_is_on = TS_STARTUP_STATE;
  
  tilt_sensor.on_cnt = 0u;
  
  tilt_sensor.off_cnt = 0u;
 
#if USE_SPI_TILT
  
  spi_init();
  
#endif
 

}




//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //


#if TILT_IS_ALWAYS_ON

// a count algorithm in function of the last read state --> therefroe we are changoing the state only on 
// count > than threshold. and there can be two different thresholds for up and downcount.
void update_tilt_sensor_state(void){


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
void update_tilt_sensor_state(void){

#if USE_SPI_TILT
  uint8_t read_state = get_tilt_data();
#else
  uint8_t read_state = TILT_SENSOR;
#endif



	if(read_state == SENSOR_IS_TOP_MOUNTED)
	{

    if(tilt_sensor.detector_is_on != 
    )
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
	
	
}






#endif

#if USE_SPI_TILT

  // Function to initialize the SPI pins
static void spi_init(void) {
  
  tx_buffer[0] = S2DCTRL1;
  tx_buffer[1] = 0x10;  // 0x10u;
  rw_data_bb_spi(2u);
  
  
}


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


// sending first the address and after that the data, 
// furthermore reading back the result in case it was a read instruction
static void rw_data_bb_spi(uint8_t len_to_send){
  
  
  uint8_t *tx_pnt = &tx_buffer[0];
  uint8_t *rx_pnt = &rx_buffer[0];
  int8_t hlooper = 0;
  
  
  BB_SPI_CLCK = true;

  __delay_us(5);
  
  // assert de chip select
  BB_SPI_CS = false;
  
  __delay_us(5);
  
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
  
  BB_SPI_CLCK = true;

  __delay_us(50);

  // Loop through each bit of the data
  for (hlooper = 0; hlooper < 8; hlooper++)
  {
    
    // Set the MOSI pin according to the current bit
    if (data & (shifter[7 - hlooper]))
    {
      BB_SPI_SDO = true;
    }
    else
    {
      BB_SPI_SDO = false;
    }

    // Pulse the clock
    BB_SPI_CLCK = false;
    
    __delay_us(10); // Adjust delay as needed
    
    BB_SPI_CLCK = true;
   
    __delay_us(10); // Adjust delay as needed

    // Read the MISO pin if it's connected
    if(BB_SPI_SDI == true)
    { 
      received |= (shifter[7 - hlooper]); 
    }
  }


  return received;
  
}

#endif  // USE_SPI_TILT





// EOF