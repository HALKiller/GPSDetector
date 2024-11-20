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

#include "handlers.h"

#include <string.h>



//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

#if 0
struct udt_detector{
  
  uint8_t number;
  uint8_t max_detectores;
  uint8_t transmission_duration;
  uint8_t syncro_time;
  uint16_t seconds_until_next_tx;
  
};
#endif


struct udt_m{
	
  uint8_t on_time;
  uint8_t off_time;
  uint8_t pwm_onoff_time_cnt;
  uint8_t pwm_value;
  
};


//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 
 #if 0
 
static const uint16_t shifter[16] = {
	
	(1U << 0), (1U << 1), (1U << 2), (1U << 3), 
	(1U << 4), (1U << 5), (1U << 6), (1U << 7), 
  (1U << 8), (1U << 9), (1U << 10), (1U << 11), 
	(1U << 12), (1U << 13), (1U << 14), (1U << 14), 
	
}; 
 
#endif

//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 

#define PWM_LUZ_PWM_VALUE_EEPROM_ADDRESS 0x44u

#define  BIT_SLOT_DOUBLE_PERIOD 0
#define  BIT_SLOT_ALWAYS_TRANSMIT 1
#define  BIT_SLOT_TX_150BPS 4
#define  BIT_SLOT_LUZ_ENABLED 5


#if DEBUGGING_IS_ON
#define MEASURE_ILUMINATION_TIME_CNT_BASE 15u*TIME_BASE_200_CNT    // the time between measurements of the ilum.sensor
#else
#define MEASURE_ILUMINATION_TIME_CNT_BASE 45u*TIME_BASE_200_CNT    // the time between measurements of the ilum.sensor
#endif


// #define PWM_LUZ_DEBUG 0

// #if PWM_LUZ_DEBUG
// #define LED_SIMUL DB_LED_2
// #else
// #define LED_SIMUL  
// #endif



// FLAGS   
// #define LUZ_ENABLED     pwm_flgs.b0 // from the eeprom cfg
#define LUZ_HANDLER_ON  pwm_flgs.b1 // that is getting set when the sensor measures it is dark
#define DOUBLE_PERIOD   pwm_flgs.b2
// #define TX_150BPS       pwm_flgs.b3
#define ALWAYS_TRANSMIT pwm_flgs.b4
#define PWM_IS_ON       pwm_flgs.b5 // when the TMR0_IE gers set 



//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //

static union8_t pwm_flgs;

static struct udt_m pwm_luz;

static uint8_t measure_ilum_time_cnt = MEASURE_ILUMINATION_TIME_CNT_BASE;

struct udt_detector gd;


uint8_t baterie_mV;

static uint8_t gVoltajeBateriaTrasTransmision = 0;

uint8_t FTW0[4];
uint8_t FTW1[4];
uint8_t FTW2[4];
uint8_t FTW3[4];

//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

static void measure_ilumination(void);




//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

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

#if 0

void init_detector_config(void){
  
  uint8_t t_var = 0x15;
  UART_int(t_var);

}

#else
  

void init_detector_config(void){
  
  int8_t i;
  
  uint8_t ee_retval = 0u;
  
  uint8_t gd_number = 0u;
 
  for ( i = 0; i < 3u; i++ )
  {
    gd_number = gd_number * 10u + ( LeerEeprom ( 0x3Eu + (uint8_t)i ) & 0xFu );
  }
  
  
  gd.number = gd_number;  
  gd.transmission_duration  = LeerEeprom ( 0x42u );
  gd.syncro_time    = LeerEeprom ( 0x43u ); 
  gd.max_detectores = LeerEeprom ( 0x47u );
  
  gd.time_between_tx = gd.transmission_duration * gd.max_detectores;
  
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
 
#if DEBUGGING_IS_ON 

  DB_PRINT("CFG: ");
  
  UART_int(ee_retval);
  
  DB_PRINT("\r\n");
  
#endif
  
#if 1

  DOUBLE_PERIOD  = !(ee_retval & shifts[BIT_SLOT_DOUBLE_PERIOD]);

  LUZ_ENABLED = !(ee_retval & shifts[BIT_SLOT_LUZ_ENABLED]);
  
  TX_150BPS = (ee_retval & shifts[BIT_SLOT_TX_150BPS]);
  
  ALWAYS_TRANSMIT = (ee_retval & shifts[BIT_SLOT_ALWAYS_TRANSMIT]);

  if(ALWAYS_TRANSMIT == true)
  {
    handlers_generic_set_handler_FLG(e_always_transmit_handler);
  }
  

#elif 1


  DOUBLE_PERIOD  = !(ee_retval & shifts[BIT_SLOT_DOUBLE_PERIOD]);
  
  db_printing_bits(DOUBLE_PERIOD);

  LUZ_ENABLED = !(ee_retval & shifts[BIT_SLOT_LUZ_ENABLED]);
  
  db_printing_bits(LUZ_ENABLED);
  
  TX_150BPS = (ee_retval & shifts[BIT_SLOT_TX_150BPS]);
  
  db_printing_bits(TX_150BPS);
  
  ALWAYS_TRANSMIT = (ee_retval & shifts[BIT_SLOT_ALWAYS_TRANSMIT]);

  db_printing_bits(ALWAYS_TRANSMIT);


#else  
  

  
#endif
  
  
}


#endif

uint8_t get_detector_number(void){
  
  return gd.number;
  
}

uint8_t get_sync_time(void){
  
  
  return gd.syncro_time;
  
}


uint8_t get_transmission_duration(void){
  
  return gd.transmission_duration;
  
}

uint8_t get_max_detectores(void){
  
  return gd.max_detectores;
  
}



void LeerValorBateria(bool AntesDeTransmitir){

  uint16_t adcvalue;
  uint16_t adcconv;


	// swoff_global_interrupt();
  
  ConversionAdc(RIGHT_JUSTIFIED, BATERIA_ADC_CHANNEL);

  adcconv = ( (ADRESH * 256) + ADRESL );
  
  // adcconv = ADRESH;
  // adcconv = adcconv<<8 & ADRESL;
  
  // adcconv = ( (ADRESH << 8) & ADRESL );
  
	// restore_global_interrupt();

#if 1

  // this is the original good one!
  adcvalue = ( ( ( adcconv * 10 + 36 ) / 18 ) + ( ( adcconv * 17 + 49 ) / 48 ) ) / 7;
  
#else  
  
  DB_PRINT("\r\nOld: ");
  
  UART_int(adcvalue);
  

  
  adcvalue = (adcconv * 131 + 759) / 1008;// that does not fit into a uint16_t 
  DB_PRINT("\r\nNEW: ");
  UART_int(adcvalue);
  DB_PRINT("\r\n");
#endif


  if ( AntesDeTransmitir == true )
  {
    baterie_mV = ( adcvalue & 0x00ff );
    // TODO:
    // implement this line
    // gTransmiteADoblePeriodo = ( ( adcconv / 4 ) <= LeerEeprom( 0x48 ) );
  }
  else
  {
    // 
    gVoltajeBateriaTrasTransmision = ( adcvalue & 0x00ff );
  }
  
  adcvalue = 0;
  
}


















#if COMPILE_WITH_PWM_LUZ
// if the pwm_luz is on from the config we enter here every time_base time(200ms)
// and have to update than depending on the different counters the states..
// to make the whole thing second based i use a cnt to five first and therefroe i can stay with most things inside a normal uint8_t cnt...
void pwm_luz_time_update(void){
  
  
  if(LUZ_HANDLER_ON == TRUE)
  {
    if(--pwm_luz.pwm_onoff_time_cnt == 0u)
    {
      // we swap on_and_off
      if(PWM_IS_ON == TRUE)
      {
        pwm_luz.pwm_onoff_time_cnt = pwm_luz.off_time;
        TMR0_IE = FALSE;
        PWM_IS_ON = FALSE;
        LED = FALSE;
#if PWM_LUZ_DEBUG
        LED_SIMUL_OFF;
#endif        
      }
      else
      {
        pwm_luz.pwm_onoff_time_cnt = pwm_luz.on_time;
        TMR0_IE = TRUE;
        PWM_IS_ON = TRUE;
      }
    }
#if DEBUGGING_IS_ON&&0    
    UWT("\r\nONOFFcnt: ");
    UART_int(pwm_luz.pwm_onoff_time_cnt);
#endif    
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



#if 0
void swap_luz_on_off(void){
  
  LUZ_HANDLER_ON = !LUZ_HANDLER_ON;
  db_printing_bits(LUZ_HANDLER_ON);
  
}

#endif


//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //


uint8_t read_ilum_sensor(void){

	uint8_t ret_value;


  ConversionAdc(LEFT_JUSTIFIED, LDR_ADC_CHANNEL);
	
  ret_value = ADRESH;
	
	return ret_value;
	
}



static void measure_ilumination(void){
  
  uint8_t t_val = 0;
  bool temp_flg = false;
  
  bool temp_IE = TMR0_IE;
  
  // TODO:
  // measure the adc from the sensor and compare to thresholde
  // if there is a change --> run the change setter for on or for off
#if DEBUGGING_IS_ON&&PWM_LUZ_DEBUG   
  DB_PRINT("\r\nIlum: ");
#endif  
  
  TMR0_IE = FALSE;
  LED = false;  // so that we are not measuring the LED
#if PWM_LUZ_DEBUG
  LED_SIMUL_OFF;
#endif        
  
  t_val = read_ilum_sensor(); 
  
  TMR0_IE = temp_IE;
  
#if INVERTED_LDR_SENSOR
  temp_flg = ( t_val < LUZ_ADC_DARK_THRESHOLD_INVERTED );
#else    
  temp_flg = ( t_val > LUZ_ADC_DARK_THRESHOLD );
#endif

#if DEBUGGING_IS_ON&&PWM_LUZ_DEBUG
  UART_int(t_val);
  DB_PRINT("\r\n");
#endif  

  if(LUZ_HANDLER_ON != temp_flg)
  {
    
    LUZ_HANDLER_ON = !LUZ_HANDLER_ON;
    
    if(LUZ_HANDLER_ON == FALSE)
    {

      TMR0_IE = false;
      LED = false;
#if PWM_LUZ_DEBUG
      LED_SIMUL_OFF;
#endif        
  
    }
  }
}


#endif // COMPILE_WITH_PWM_LUZ



// EOF