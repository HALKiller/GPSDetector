// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com



//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //



//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //
#include "detector.h"

#include "Global.h"

#include "eeprom.h"

#include "generic_union_flgs.h"

#include "UART.h"

#include "io_port_sfr_names.h"

#include "timers.h"

#include "ADC.h"

#include "handlers.h"

#include <string.h>




//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 




#define PWM_LUZ_PWM_VALUE_EEPROM_ADDRESS 0x44u

#define  BIT_SLOT_DOUBLE_PERIOD 0u
#define  BIT_SLOT_ALWAYS_TRANSMIT 1u
#define  BIT_SLOT_TX_150BPS 4u
#define  BIT_SLOT_LUZ_ENABLED 5u


#if DEBUGGING_IS_ON||0
#define MEASURE_ILUMINATION_TIME_CNT_BASE (5u * TIME_BASE_200_CNT)    // the time between measurements of the ilum.sensor
#else
#define MEASURE_ILUMINATION_TIME_CNT_BASE (45u * TIME_BASE_200_CNT)    // the time between measurements of the ilum.sensor
#endif

#define BAT_IS_TOO_LOW_THRESHOLD 80


//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //



struct udt_m{
	
  uint8_t on_time;
  uint8_t off_time;
  uint8_t pwm_onoff_time_cnt;
  uint8_t pwm_value;
  
};


//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 

#if 0
const uint8_t shifts_8bit[8] = {
	
	(1U << 0), (1U << 1), (1U << 2), (1U << 3), 
	(1U << 4), (1U << 5), (1U << 6), (1U << 7), 

	
}; 

#endif






//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //

union8_t gd_flags;

static struct udt_m pwm_luz;

static uint8_t measure_ilum_time_cnt = MEASURE_ILUMINATION_TIME_CNT_BASE;

struct udt_detector gd;


uint8_t baterie_mV;

// static uint8_t gVoltajeBateriaTrasTransmision = 0;

#if USE_OLD_CFG_SETTER
uint8_t FTW0[4];
uint8_t FTW1[4];
uint8_t FTW2[4];
uint8_t FTW3[4];
#endif
//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

static void measure_ilumination(void);
static uint16_t calculate_voltage_from_input_value(uint16_t value);




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
 
 
  gd_flags.reg = 0u;
  
  
  for ( i = 0; i < 3u; i++ )
  {
    gd_number = gd_number * 10u + ( LeerEeprom ( 0x3Eu + (uint8_t)i ) & 0xFu );
  }
  
  
  gd.number = gd_number;  
  gd.transmission_duration  = LeerEeprom ( 0x42u );
  gd.syncro_time    = LeerEeprom ( 0x43u ); 
  gd.max_detectores = LeerEeprom ( 0x47u );
  
  gd.time_between_tx = gd.transmission_duration * gd.max_detectores;
  
  gd.vbat_low = calculate_voltage_from_input_value(LeerEeprom(0x48u) * 4u);
  
  // and now we would need  to calculate back first the mV value ,
  // add to it the dlta and then return the eeprom calculated value...
  
  gd.vbat_high = gd.vbat_low + VBAT_DELTA;
  // gd.vbat_high = (((uint32_t)gd.vbat_low + VBAT_DELTA) * 1008u - 435u) / 131u; 
#if DEBUGGING_BB_IS_ON

  DB_PRINT("Vbat_L: ");
  UART_int(gd.vbat_low);
  UART_CRLF;
  DB_PRINT("Vbat_H: ");
  UART_int(gd.vbat_high);
  UART_CRLF;
  
#endif

#if USE_OLD_CFG_SETTER  
  for ( i = 0; i < 4; i++ )
  {
    FTW0[i] = LeerEeprom ( 0x22u + (uint8_t)i );
    FTW1[i] = LeerEeprom ( 0x27u + (uint8_t)i );
    FTW2[i] = LeerEeprom ( 0x2Cu + (uint8_t)i );
    FTW3[i] = LeerEeprom ( 0x31u + (uint8_t)i );
  }
#endif  
  
  pwm_luz.pwm_value = LeerEeprom ( 0x44u );
  
  ee_retval = LeerEeprom ( 0x45u );
  
  pwm_luz.off_time = TIME_BASE_200_CNT * (uint8_t)((ee_retval & 0x0Fu));
  
  pwm_luz.on_time = TIME_BASE_200_CNT * (uint8_t)(( ee_retval >> 4 ) & 0x0Fu);
  
  pwm_luz.pwm_onoff_time_cnt = pwm_luz.off_time;  // becaseu we start with the pwm in off state...
  
  ee_retval = LeerEeprom ( 0x46u );
 

  if((ee_retval & 0x01) == false)
  {
    DOUBLE_PERIOD = true;
  }

  if((ee_retval & 0x02) != false)
  {
    ALWAYS_TRANSMIT = true;
  }
  
  if((ee_retval & 0x10) != false)
  {
    TX_150BPS = true;
  }
  
  if((ee_retval & 0x20) == false)
  {
    LUZ_ENABLED = true;
  }

  if(ALWAYS_TRANSMIT == true)
  {
    handlers_generic_set_handler_FLG(e_always_transmit_handler);
  }
  
 
  

#if DEBUGGING_IS_ON||0

  DB_PRINT("CFG: ");
  
  UART_int(ee_retval);
  
  DB_PRINT("\r\n");
  
  DB_PRINT("DOUBLE_PERIOD: ");
  
  db_printing_bits(DOUBLE_PERIOD);

  DB_PRINT("LUZ_ENABLED: ");
  
  db_printing_bits(LUZ_ENABLED);
  
  DB_PRINT("TX_150BPS: ");
  
  db_printing_bits(TX_150BPS);
  
  DB_PRINT("ALWAYS_TRANSMIT: "); 

  db_printing_bits(ALWAYS_TRANSMIT);


  
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


#if 1


void LeerValorBateria(void){

  uint16_t adcvalue;
  uint16_t adcconv;
  
  ConversionAdc(RIGHT_JUSTIFIED, BATERIA_ADC_CHANNEL);

  adcconv = ( (ADRESH * 256) + ADRESL );

#if 0

  // this is the original good one!
  
  adcvalue = ( ( ( adcconv * 10 + 36 ) / 18 ) + ( ( adcconv * 17 + 49 ) / 48 ) ) / 7;
  
#elif 1
  
  adcvalue = calculate_voltage_from_input_value(adcconv);
  
#else  
  
  DB_PRINT("\r\nOld: ");
  
  UART_int(adcvalue);
  

  
  adcvalue = (adcconv * 131 + 759) / 1008;// that does not fit into a uint16_t 
  DB_PRINT("\r\nNEW: ");
  UART_int(adcvalue);
  DB_PRINT("\r\n");
#endif



  baterie_mV = (adcvalue & 0x00ff);
  // TODO:
  // implement this line
  // gTransmiteADoblePeriodo = ( ( adcconv / 4 ) <= LeerEeprom( 0x48 ) );
  if((BAT_IS_LOW_FLG == true) && (adcvalue >= gd.vbat_high))
  {
    BAT_IS_LOW_FLG = FALSE;
    BAT_IS_TOO_LOW = false;
  }
  else if((BAT_IS_LOW_FLG == false) && (adcvalue  <= gd.vbat_low))
  {
    BAT_IS_LOW_FLG = true;
    if(adcvalue <= BAT_IS_TOO_LOW_THRESHOLD)
    {
      BAT_IS_TOO_LOW = true; 
    }
  }
  
}

#if 0

// 16 words longer!! incredible but yes
static uint16_t calculate_voltage_from_input_value(uint16_t value){
  
  uint16_t val_1 = 0;
  uint16_t val_2 = 0;
  uint16_t ret_value = 0u;
  
  val_1 = (5 * value / 9) + 2;
  
  val_2 = (17 * value + 49) >> 4;
  
  val_2 = val_2 / 3u;
  
  // val_2 = (17 * value + 49 ) / 48;
  
  ret_value = (val_1 + val_2) / 7;
  
  // ret_value = ( ( ( value * 5 + 9 ) / 2 ) + ( ( value * 17 + 49 ) / 48 ) ) / 7;
  
  return ret_value;

}  
#elif 1


// 4 words shorter
static uint16_t calculate_voltage_from_input_value(uint16_t value){
  
  uint16_t val_1 = 0;
  uint16_t val_2 = 0;
  uint16_t ret_value = 0u;
  
  val_1 = (5 * value / 9) + 2;
  val_2 = (17 * value + 49 ) / 48;
  ret_value = (val_1 + val_2) / 7;
  
  // ret_value = ( ( ( value * 5 + 9 ) / 2 ) + ( ( value * 17 + 49 ) / 48 ) ) / 7;
  
  return ret_value;
  
}

#else
  
static uint16_t calculate_voltage_from_input_value(uint16_t value){
  
  
  uint16_t ret_value = 0u;
  
  ret_value = ( ( ( value * 10 + 36 ) / 18 ) + ( ( value * 17 + 49 ) / 48 ) ) / 7;
  
  return ret_value;
  
}

#endif

#else
  

void LeerValorBateria(bool AntesDeTransmitir){

  uint16_t adcvalue;
  uint16_t adcconv;


	// swoff_global_interrupt();
  
  ConversionAdc(RIGHT_JUSTIFIED, BATERIA_ADC_CHANNEL);

  adcconv = ( (ADRESH * 256) + ADRESL );
  
  

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
    // BAT_IS_LOW_FLG = ( ( adcconv / 4 ) <= LeerEeprom( 0x48 ) );
    
  }
  else
  {
    // 
    gVoltajeBateriaTrasTransmision = ( adcvalue & 0x00ff );
  }
  
  adcvalue = 0;
  
}


#endif






uint8_t read_ilum_sensor(void){

	uint8_t ret_value;

  ConversionAdc(LEFT_JUSTIFIED, LDR_ADC_CHANNEL);
	
  ret_value = ADRESH;
	
	return ret_value;
	
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
#if DEBUGGING_BB_IS_ON&&0
    DB_PRINT("\r\nl_c: ");
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






//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //



static void measure_ilumination(void){
  
  uint8_t t_val = 0;
  bool temp_flg = false;
  
  bool temp_IE = TMR0_IE;
  
  // TODO:
  // measure the adc from the sensor and compare to thresholde
  // if there is a change --> run the change setter for on or for off
#if DEBUGGING_IS_ON&&PWM_LUZ_DEBUG&&0 //   DEBUGGING_BB_IS_ON  // 
  DB_PRINT("\r\nIlu: ");
#endif  
  
  TMR0_IE = FALSE;
  LED = false;  // so that we are not measuring the LED
  
  
  
  // and now we need a delay to assure that 
  // we are not measring the LED actually
  // easiest solution is to give a 30ms delay
  __delay_ms(30);
  
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

#if DEBUGGING_IS_ON&&PWM_LUZ_DEBUG&&0// DEBUGGING_BB_IS_ON  // 
  UART_int(t_val);
  DB_PRINT("\r\n");
  
  
#endif  



  if(LUZ_HANDLER_ON != temp_flg)
  {
    // DB_PRINT("LUZ_ON\r\n");
    LUZ_HANDLER_ON = !LUZ_HANDLER_ON;
    
    if(LUZ_HANDLER_ON == FALSE)
    {

      TMR0_IE = false;
      LED = false;
#if PWM_LUZ_DEBUG
      LED_SIMUL_OFF;
#endif        
      DB_PRINT("\r\nLUZ_h_OFF\r\n");
    }
    else
    {
      DB_PRINT("\r\nLUZ_h_ON\r\n");
    }
  }
}


#endif // COMPILE_WITH_PWM_LUZ



// EOF