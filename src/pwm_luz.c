// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com



//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //



//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //
#include "pwm_luz.h"

#include "Global.h"

#include "eeprom.h"

#include "generic_union_flgs.h"

#include "UART.h"

#include "io_port_sfr_names.h"

#include "timers.h"

#include "ADC.h"

#include <stdint.h>


//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //


struct udt_detector{
  
  uint8_t number;
  uint8_t max_detectores;
  uint8_t transmit_time;
  uint8_t syncro_time;
  
  
};



struct udt_m{
	
  uint8_t on_time;
  uint8_t off_time;
  uint8_t pwm_onoff_time_cnt;
  
  uint8_t pwm_value;
};


//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 
 
static const uint16_t shifter[16] = {
	
	(1U << 0), (1U << 1), (1U << 2), (1U << 3), 
	(1U << 4), (1U << 5), (1U << 6), (1U << 7), 
  (1U << 8), (1U << 9), (1U << 10), (1U << 11), 
	(1U << 12), (1U << 13), (1U << 14), (1U << 14), 
	
}; 
 


//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 

#define PWM_LUZ_PWM_VALUE_EEPROM_ADDRESS 0x44u

#define  BIT_SLOT_DOUBLE_PERIOD 0u
#define  BIT_SLOT_ALWAYS_TRANSMIT 1u
#define  BIT_SLOT_TX_150BPS 4u
#define  BIT_SLOT_LUZ_ENABLED 5u


#if DEBUGGING_IS_ON
#define MEASURE_ILUMINATION_TIME_CNT_BASE 5u*TIME_BASE_200_CNT    // the time between measurements of the ilum.sensor
#else
#define MEASURE_ILUMINATION_TIME_CNT_BASE 45u*TIME_BASE_200_CNT    // the time between measurements of the ilum.sensor
#endif







// FLAGS   
// #define LUZ_ENABLED     pwm_flgs.b0 // from the eeprom cfg
#define LUZ_HANDLER_ON  pwm_flgs.b1 // that is getting set when the sensor measures it is dark
#define DOUBLE_PERIOD   pwm_flgs.b2
#define TX_150BPS       pwm_flgs.b3
#define ALWAYS_TRANSMIT pwm_flgs.b4
#define PWM_IS_ON       pwm_flgs.b5 // when the TMR0_IE gers set 



//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //

static union8_t pwm_flgs;
static struct udt_m pwm_luz;
static uint8_t measure_ilum_time_cnt = MEASURE_ILUMINATION_TIME_CNT_BASE;
static struct udt_detector gd;
static uint8_t FTW0[4];
static uint8_t FTW1[4];
static uint8_t FTW2[4];
static uint8_t FTW3[4];

//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

static void measure_ilumination(void);




//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

#if 1

static void db_printing_bits(uint8_t onoff_bit){
  
  if(onoff_bit == 0u)
  {
    DB_PRINT("Off\r\n");
  }
  else
  {
    DB_PRINT("On\r\n");
  }
  
  
  
}

void init_detector_config(void){
  
  int8_t i;
  
  uint8_t ee_retval = 0u;
  
  uint8_t gd_number = 0u;
 
  for ( i = 0; i < 3u; i++ )
  {
    gd_number = gd_number * 10u + ( LeerEeprom ( 0x3Eu + (uint8_t)i ) & 0xFu );
  }
  
  
  gd.number = gd_number;  
  gd.transmit_time  = LeerEeprom ( 0x42u );
  gd.syncro_time    = LeerEeprom ( 0x43u ); 
  gd.max_detectores = LeerEeprom ( 0x47u );
  
  
  for ( i = 0; i < 4; i++ )
  {
    FTW0[i] = LeerEeprom ( 0x22u + (uint8_t)i );
    FTW1[i] = LeerEeprom ( 0x27u + (uint8_t)i );
    FTW2[i] = LeerEeprom ( 0x2Cu + (uint8_t)i );
    FTW3[i] = LeerEeprom ( 0x31u + (uint8_t)i );
  }
  
  
  pwm_luz.pwm_value = LeerEeprom ( 0x44u );
  
  ee_retval = LeerEeprom ( 0x45u );
  pwm_luz.off_time = TIME_BASE_200_CNT * (uint8_t)((ee_retval & 0x0Fu));
  pwm_luz.on_time = TIME_BASE_200_CNT * (uint8_t)(( ee_retval >> 4 ) & 0x0Fu);
  
  pwm_luz.pwm_onoff_time_cnt = pwm_luz.off_time;  // becaseu we start with the pwm in off state...
  
  ee_retval = LeerEeprom ( 0x46u );
  
  DB_PRINT("CFG: ");
  
  UART_int(ee_retval);
  
  DB_PRINT("\r\n");
  
#if 1

  DOUBLE_PERIOD  = !(ee_retval & shifter[BIT_SLOT_DOUBLE_PERIOD]);

  LUZ_ENABLED = !(ee_retval & shifter[BIT_SLOT_LUZ_ENABLED]);
  
  TX_150BPS = (ee_retval & shifter[BIT_SLOT_TX_150BPS]);
  
  ALWAYS_TRANSMIT = (ee_retval & shifter[BIT_SLOT_ALWAYS_TRANSMIT]);

  
#elif 1


  DOUBLE_PERIOD  = !(ee_retval & shifter[BIT_SLOT_DOUBLE_PERIOD]);
  
  db_printing_bits(DOUBLE_PERIOD);

  LUZ_ENABLED = !(ee_retval & shifter[BIT_SLOT_LUZ_ENABLED]);
  
  db_printing_bits(LUZ_ENABLED);
  
  TX_150BPS = (ee_retval & shifter[BIT_SLOT_TX_150BPS]);
  
  db_printing_bits(TX_150BPS);
  
  ALWAYS_TRANSMIT = (ee_retval & shifter[BIT_SLOT_ALWAYS_TRANSMIT]);

  db_printing_bits(ALWAYS_TRANSMIT);


#else  
  
  DOUBLE_PERIOD  = !((t_byte *)&i)->b0;
  
  // gActivarDoblePeriodo       = !((t_byte *)&i)->b0;

  LUZ_ENABLED = !((t_byte *)&i)->b5;
  TX_150BPS    = ((t_byte *)&i)->b4;  
  // gTrueSi150FalseSi300       = ((t_byte *)&i)->b4;
  ALWAYS_TRANSMIT       = ((t_byte *)&i)->b1;
  
#endif
  
  
}

#else
  
void init_detector_config(void){
  
  int8_t i;
  
  uint8_t ee_retval = 0u;
  uint8_t gd_number = 0u;
 
  for ( i = 0; i < 3u; i++ )
  {
    gd_number = gd_number * 10u + ( LeerEeprom ( 0x3Eu + i ) & 0xFu );
  }
  
  
  gd.number = gd_number;  
  gd.transmit_time  = LeerEeprom ( 0x42u );
  gd.syncro_time    = LeerEeprom ( 0x43u ); 
  gd.max_detectores = LeerEeprom ( 0x47u );
  
  
  for ( i = 0; i < 4; i++ )
  {
    FTW0[i] = LeerEeprom ( 0x22u + i );
    FTW1[i] = LeerEeprom ( 0x27u + i );
    FTW2[i] = LeerEeprom ( 0x2Cu + i );
    FTW3[i] = LeerEeprom ( 0x31u + i );
  }
  
  
  pwm_luz.pwm_value = LeerEeprom ( 0x44u );
  
  i = LeerEeprom ( 0x45u );
  pwm_luz.off_time          = i & 0x0F;
  pwm_luz.on_time           = ( i >> 4 ) & 0x0F;

  i = LeerEeprom ( 0x46u );
  
#if 0

  DOUBLE_PERIOD  = !(i & shifter[BIT_SLOT_DOUBLE_PERIOD]);
  
  // gActivarDoblePeriodo       = !((t_byte *)&i)->b0;

  LUZ_ENABLED = !((t_byte *)&i)->b5;
  TX_150BPS    = ((t_byte *)&i)->b4;  
  // gTrueSi150FalseSi300       = ((t_byte *)&i)->b4;
  ALWAYS_TRANSMIT       = ((t_byte *)&i)->b1;

#else  
  DOUBLE_PERIOD  = !((t_byte *)&i)->b0;
  
  // gActivarDoblePeriodo       = !((t_byte *)&i)->b0;

  LUZ_ENABLED = !((t_byte *)&i)->b5;
  TX_150BPS    = ((t_byte *)&i)->b4;  
  // gTrueSi150FalseSi300       = ((t_byte *)&i)->b4;
  ALWAYS_TRANSMIT       = ((t_byte *)&i)->b1;
  
  #endif
  
  
}

#endif



// if the pwm_luz is on from the config we enter here every time_base time(200ms)
// and have to update than depending on the different counters the states..
// to make the whole thing second based i use a cnt to five first and therefroe i can stay with most things inside a normal uint8_t cnt...
void pwm_luz_time_update(void){
  
  
  if(LUZ_HANDLER_ON == TRUE)
  {
    if(--pwm_luz.pwm_onoff_time_cnt == 0u)
    {
      
      
      DB_LED3_SWAP;
      
      // we swap on_and_off
      if(PWM_IS_ON == TRUE)
      {
        pwm_luz.pwm_onoff_time_cnt = pwm_luz.off_time;
        TMR0_IE = FALSE;
        PWM_IS_ON = FALSE;
        LED = FALSE;
        DB_LED_2 = FALSE;
      }
      else
      {
        pwm_luz.pwm_onoff_time_cnt = pwm_luz.on_time;
        TMR0_IE = TRUE;
        PWM_IS_ON = TRUE;
      }
    }
    UWT("\r\nONOFFcnt: ");
    UART_int(pwm_luz.pwm_onoff_time_cnt);
  }
  
  
  // we would need to check on these things: 
  // * do we need to measure again the sensor?
  // * swap over the cnt?
  if(--measure_ilum_time_cnt == 0u)
  {
    
    measure_ilum_time_cnt = MEASURE_ILUMINATION_TIME_CNT_BASE;
    
    measure_ilumination();
    
    
    
  }
  
  

  
  
  
  
  
  
  
  
}


uint8_t get_pwm_luz_pwm_value(void){
  
  
  return pwm_luz.pwm_value;
  
}



#if DEBUGGING_IS_ON
void swap_luz_on_off(void){
  
  LUZ_HANDLER_ON = !LUZ_HANDLER_ON;
  db_printing_bits(LUZ_HANDLER_ON);
  
}

#endif


//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //


uint8_t read_ilum_sensor(void)
{
	uint8_t ret_value;
	
	// gADC_is_active = true;
	
	// swoff_global_interrupt();
  ConversionAdc(LEFT_JUSTIFIED, LDR_ADC_CHANNEL);
	ret_value = ADRESH;
	// restore_global_interrupt();
	
	return ret_value;
	
}












static void measure_ilumination(void){
  
  uint8_t t_val = 0;
  bool temp_flg = false;
  
  bool temp_IE = TMR0_IE;
  
  // TODO:
  // measure the adc from the sensor and compare to thresholde
  // if there is a change --> run the change setter for on or for off
  DB_PRINT("\r\nIlum: ");
  
  
  TMR0_IE = FALSE;
  LED = false;  // so that we are not measuring the LED
  DB_LED_2 = FALSE;
  t_val = read_ilum_sensor();  
  TMR0_IE = temp_IE;
  
#if INVERTED_LDR_SENSOR
  temp_flg = ( t_val < LUZ_ADC_DARK_THRESHOLD_INVERTED );
#else    
  temp_flg = ( t_val > LUZ_ADC_DARK_THRESHOLD );
#endif

  UART_int(t_val);
  DB_PRINT("\r\n");
  

  // if(LUZ_HANDLER_ON != get_ilum_value_adc())
  if(LUZ_HANDLER_ON != temp_flg)
  {
    
    LUZ_HANDLER_ON = !LUZ_HANDLER_ON;
    
    if(LUZ_HANDLER_ON == FALSE)
    {
 
      
      TMR0_IE = false;
      LED = false;
      DB_LED_2 = FALSE;
      
      
    }
  }


}




// EOF