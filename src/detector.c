// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#line 7 "detector.c"

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

#include "gd_states.h"

#include "my_assert.h"

#include <string.h>

// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  
#ifndef FILE_DETECTOR_DB_ENABLED
#define FILE_DETECTOR_DB_ENABLED 0
#endif


  
#if FILE_DETECTOR_DB_ENABLED

#define DB_PRINT(str) G_DB_PRINT(str)
#define UART_int(var) G_UART_INT(var)

#else
  
#define DB_PRINT(str)
#define UART_int(var)

#endif


// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  



//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 




#define PWM_LUZ_PWM_VALUE_EEPROM_ADDRESS 0x44u

#define  BIT_SLOT_DOUBLE_PERIOD 0u
#define  BIT_SLOT_ALWAYS_TRANSMIT 1u
#define  BIT_SLOT_TX_150BPS 4u
#define  BIT_SLOT_LUZ_ENABLED 5u

#define DEBUG_ILUM_MEASUREMENT 0

#if DEBUG_ILUM_MEASUREMENT

  #define MEASURE_ILUMINATION_TIME_CNT_BASE (1u * TIME_BASE_200_CNT)    // the time between measurements of the ilum.sensor
  #define MEASURE_ILUMINATION_TIME_CNT_ON_STARTUP (5u * TIME_BASE_200_CNT)    // the time between measurements of the ilum.sensor

#else

  #define MEASURE_ILUMINATION_TIME_CNT_BASE (60u * TIME_BASE_200_CNT)    // the time between measurements of the ilum.sensor
  #define MEASURE_ILUMINATION_TIME_CNT_ON_STARTUP (2u * TIME_BASE_200_CNT)    // the time between measurements of the ilum.sensor

#endif

#define PWM_LUZ_STARTUP_CNT_SETTER (60u * TIME_BASE_200_CNT)

#define BAT_IS_TOO_LOW_THRESHOLD 80

#define CHARGE_FLAG_SET 0x01
#define CHARGE_FLAG_RESET 0x00
#define MAX_BATCNT_LIMIT (uint16_t)0x2F86 // 23x23x23


#define LED_TIME_SHOWING 15u  // ten seconds on for the LED
#define MAX_PERMITTED_DETECTORS 99u // there shall never be more than at max. 99 detectors in a series
#define MAX_PERMITTED_TX_DURATION 15u // there should never be such a long tx time duration...
#define DEFAULT_TIME_BETWEEN_TX 10u

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

static uint16_t measure_ilum_time_cnt = MEASURE_ILUMINATION_TIME_CNT_BASE;

struct udt_detector gd;

#if OV_PWM_LUZ
static uint8_t pwm_luz_startup_cnt = PWM_LUZ_STARTUP_CNT_SETTER;
#else
static uint16_t pwm_luz_startup_cnt = PWM_LUZ_STARTUP_CNT_SETTER;
#endif

//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

static void measure_ilumination(void);
static uint8_t calculate_voltage_from_input_value(uint16_t value);
static uint8_t read_bat_value(void);



//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

#if 0
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
#endif


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
  
  if((gd.max_detectores == 0u) ||
    (gd.transmission_duration == 0u) ||
    (gd.transmission_duration > MAX_PERMITTED_TX_DURATION) ||
    (gd.max_detectores > MAX_PERMITTED_DETECTORS) ||
    (gd.number == 0u) ||
    (gd.number > gd.max_detectores))
    {
      CFG_HAS_ERROR = true;
    }
    
  
  gd.time_between_tx = gd.transmission_duration * gd.max_detectores;
  
  if(gd.time_between_tx == 0u)
  {
    // TODO:    ERROR HERE!!!
    CFG_HAS_ERROR = true;
    gd.time_between_tx = DEFAULT_TIME_BETWEEN_TX;
    assert(false);
  }
  
  gd.vbat_low = calculate_voltage_from_input_value(LeerEeprom(0x48u) * 4u);
  
  // and now we would need  to calculate back first the mV value ,
  // add to it the dlta and then return the eeprom calculated value...
  
  gd.vbat_high = gd.vbat_low + VBAT_DELTA;
 
#if DEBUGGING_BB_IS_ON&&0

  DB_PRINT("Vbat_L: ");
  UART_int(gd.vbat_low);
  UART_CRLF;
  DB_PRINT("Vbat_H: ");
  UART_int(gd.vbat_high);
  UART_CRLF;
  
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
  
  if(get_batcnt() > MAX_BATCNT_LIMIT)
  {
    write_eeprom(BATCNT_H_ADDRESS, 0u);
    write_eeprom(BATCNT_L_ADDRESS, 0u);
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


void increment_detector(void){
  
  
  gd.number++;
  if(gd.number > gd.max_detectores)
  {
    gd.number = 1;
  }
  
}



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

uint16_t get_batcnt(void){
  
  uint16_t ret_val = 0;
  
  uint8_t t_val = LeerEeprom(BATCNT_H_ADDRESS);
  
  ret_val = (uint16_t) t_val * 256;
  
  t_val = LeerEeprom(BATCNT_L_ADDRESS);
  
  ret_val = ret_val + t_val;
 
  return ret_val;
  
}


uint8_t LeerValorBateria(void){

  uint16_t adcconv;
  
  
#if DB_V69_PCB  
  
  
  gd.bat_decivolt = 120;
  
  
#elif 1


   
  gd.bat_decivolt = read_bat_value();
    


#else
  
  adcconv = read_bat_value();
  
  gd.bat_decivolt = calculate_voltage_from_input_value(adcconv);
  
#endif

  if((BAT_IS_LOW_FLG == true) && (gd.bat_decivolt >= gd.vbat_high))
  {
    BAT_IS_LOW_FLG = FALSE;
    BAT_IS_TOO_LOW = false;
  }
  else if((BAT_IS_LOW_FLG == false) && (gd.bat_decivolt  <= gd.vbat_low))
  {
    BAT_IS_LOW_FLG = true;

  }
  if(gd.bat_decivolt <= BAT_IS_TOO_LOW_THRESHOLD)
  {
    BAT_IS_TOO_LOW = true; 
  }
  
  return gd.bat_decivolt;
  
}






void measure_bat_for_batcnt(e_gpsd_states_t state){
  // becaseu we are hanlding pretty much the same things on the two instnaces we can use a
  // single funciton and jsut depending on from where we are coming handling than the diferent things
  
  // first retrieve the flag and the cnt...
  // and that comes from the EEPROM!
  // easiest way to retrieve the information is to chuck them into an array and loop
  // uint8_t temp_val;
  uint8_t t_val;
  uint8_t bat_val;
  uint16_t c_cnt = 0;
  
  
  bat_val = read_bat_value();
  
  t_val = LeerEeprom(BATCNT_FLG_ADDRESS);
  
  c_cnt = get_batcnt(); 
  
  // we have the last state...
  if(state == E_TRANSMISSION_STATE)
  {
    if((bat_val < BATCNT_LOW_VOLTAGE) && (t_val == CHARGE_FLAG_SET))
    {
      write_eeprom(BATCNT_FLG_ADDRESS, CHARGE_FLAG_RESET);
      
    }
  }
  else
  {
    if((bat_val > BATCNT_HIGH_VOLTAGE) && (t_val == CHARGE_FLAG_RESET))
    {
      
      write_eeprom(BATCNT_FLG_ADDRESS, CHARGE_FLAG_SET);
  
      if(c_cnt >= MAX_BATCNT_LIMIT)
      {
        write_eeprom(BATCNT_H_ADDRESS, 0u);
        write_eeprom(BATCNT_L_ADDRESS, 0u);
      }
      else
      {
        c_cnt++;
        t_val = c_cnt/256;
        write_eeprom(BATCNT_H_ADDRESS, t_val);
        t_val = c_cnt%256;
        write_eeprom(BATCNT_L_ADDRESS, t_val);
      }
    }
  } 
}






uint8_t read_ilum_sensor(void){

	uint8_t ret_value;

  ConversionAdc(LEFT_JUSTIFIED, LDR_ADC_CHANNEL);
	
  ret_value = ADRESH;
	
	return ret_value;
	
}


// setting values for the startup of the ilumination part...
void detector_init_ilumination_handling(void){
  
  // To avoid that the Detector iluminates on short wake ups...
  LUZ_HANDLER_ON = false;
  PWM_IS_ON = false;
  PWM_LUZ_START = true;
  measure_ilum_time_cnt = MEASURE_ILUMINATION_TIME_CNT_ON_STARTUP; // the time between measurements...
  pwm_luz.pwm_onoff_time_cnt = pwm_luz.off_time;
  pwm_luz_startup_cnt = PWM_LUZ_STARTUP_CNT_SETTER; // that cnts 1 minute...the higher measuring circle...
  
}


void detector_status_led_handler(void){
  
  
  gd.led_time_out_cnt--;
  if(gd.led_time_out_cnt == 0)
  {
    STATUS_LED_ON = false;
    STATUS_LED_RED_OFF();
    STATUS_LED_GREEN_OFF();
    gd.led_state = ALL_LED_OFF;
    DB_PRINT("L_OFF\r\n");
  }
  else
  {
    if(gd.led_state == LED_RED_BLINKS)
    {
      STATUS_LED_RED_SWAP();
    }
    else if(gd.led_state == LED_GREEN_BLINKS)
    {
      STATUS_LED_GREEN_SWAP();
    }
  }
  
  
  
  
  
  
}


// this function starts the timeout counter of the status led
// and what type of Status is getting set actually...
void detector_status_led_cnt_on(leds_state_t led_status){
  
  // set the status
  gd.led_state = led_status;
  // set the timer
  gd.led_time_out_cnt = LED_TIME_SHOWING;
  // Set the flag
  STATUS_LED_ON = true;

  switch (gd.led_state)
  {
    
    case LED_RED_ON: 
    case LED_RED_BLINKS: 
      STATUS_LED_RED_ON();
      DB_PRINT("LR_ON\r\n");
    break;
    
    case LED_GREEN_ON:
    case LED_GREEN_BLINKS:
      STATUS_LED_GREEN_ON();
      DB_PRINT("LG_ON\r\n");
    break;
    case ALL_LED_OFF:
      STATUS_LED_RED_OFF();
      STATUS_LED_GREEN_OFF();
      DB_PRINT("L_OFF\r\n");
    break;

  }
  
}





#if COMPILE_WITH_PWM_LUZ
// if the pwm_luz is on from the config we enter here every time_base time(200ms)
// and have to update than depending on the different counters the states..
// to make the whole thing second based i use a cnt to five first and therefroe i can stay with most things inside a normal uint8_t cnt...

#if OV_PWM_LUZ

// in this version we enter only once per second...

void pwm_luz_time_update(void){
  
#if DEBUGGING_BB_IS_ON
static uint8_t db_cnt = 10;
#endif         
// this is the 1 minute downcnt... 
  if(PWM_LUZ_START == true)
  {
#if DEBUGGING_BB_IS_ON  
    if(--db_cnt == 0)
    {
      DB_PRINT("\r\nL: ");
      UART_int(pwm_luz_startup_cnt);
      db_cnt = 10;
    }
#endif           
    if(--pwm_luz_startup_cnt == 0)
    {
      PWM_LUZ_START = false;
      DB_PRINT("\r\nL_START_DONE\r\n");
    }
  }
  

  if(--pwm_luz.pwm_onoff_time_cnt == 0u)
  {
    // we swap on_and_off
    if(PWM_IS_ON == TRUE)
    {
      pwm_luz.pwm_onoff_time_cnt = pwm_luz.off_time;
      PWM_IS_ON = FALSE;
      if(LUZ_HANDLER_ON == TRUE)
      {
        TMR0_IE = FALSE;
        LED = FALSE;
#if DEBUGGING_BB_IS_ON          
      DB_PRINT("\r\nL_OFF ");
      UART_int(pwm_luz_startup_cnt);
      DB_LED_1_ON;
#endif             
      }
    }
    else
    {
      pwm_luz.pwm_onoff_time_cnt = pwm_luz.on_time;
      PWM_IS_ON = TRUE;
#if 1 
      if(MEASURE_ILUM_FLG == true)
      {
#if DEBUGGING_BB_IS_ON          
            DB_LED_1_OFF;
            DB_PRINT("\r\nM ");
            UART_int(pwm_luz_startup_cnt);
#endif            
        measure_ilumination();
        MEASURE_ILUM_FLG = false;
        
      }
#endif    
      if(LUZ_HANDLER_ON == TRUE)
      {
        TMR0_IE = TRUE;
        DB_PRINT("\r\nL_ON");
      }
    }
  }

  // we would need to check on these things: 
  // * do we need to measure again the sensor?
  // * swap over the cnt?
  if(--measure_ilum_time_cnt == 0u)
  {
    
    if(PWM_LUZ_START == true)
    {
      measure_ilum_time_cnt = MEASURE_ILUMINATION_TIME_CNT_ON_STARTUP;
    }
    else
    {
      measure_ilum_time_cnt = MEASURE_ILUMINATION_TIME_CNT_BASE;
    }
    
    MEASURE_ILUM_FLG = true;
    
  }

  
}

#elif 0
void pwm_luz_time_update(void){
#if DEBUGGING_BB_IS_ON
static uint8_t db_cnt = 10;
#endif          
  if(PWM_LUZ_START == true)
  {
#if DEBUGGING_BB_IS_ON  
    if(--db_cnt == 0)
    {
      DB_PRINT("\r\nL: ");
      UART_int(pwm_luz_startup_cnt);
      db_cnt = 10;
    }
#endif           
    if(--pwm_luz_startup_cnt == 0)
    {
      PWM_LUZ_START = false;
      DB_PRINT("\r\nL_START_DONE\r\n");
    }
  }
  
#if 1 
  if ((MEASURE_ILUM_FLG == true) && (PWM_IS_ON == false))
  {
#if DEBUGGING_BB_IS_ON          
        DB_LED_1_OFF;
        DB_PRINT("\r\nM ");
        UART_int(pwm_luz_startup_cnt);
#endif            
    measure_ilumination();
    MEASURE_ILUM_FLG = false;
    
  }
#endif    

  
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
#if DEBUGGING_BB_IS_ON          
        DB_PRINT("\r\nL_OFF ");
        UART_int(pwm_luz_startup_cnt);
        DB_LED_1_ON;
#endif        
#if PWM_LUZ_DEBUG
        LED_SIMUL_OFF;
#endif        
      }
      else
      {
        
        pwm_luz.pwm_onoff_time_cnt = pwm_luz.on_time;
        TMR0_IE = TRUE;
        PWM_IS_ON = TRUE;
        DB_PRINT("\r\nL_ON");
      }
      
#if DEBUGGING_BB_IS_ON
// we have a new setting here...
      DB_PRINT("\r\Nl_c: ");
      UART_int(pwm_luz.pwm_onoff_time_cnt);
#endif         
      
    }
 
  }
  else
  {
    pwm_luz.pwm_onoff_time_cnt = pwm_luz.off_time;
    TMR0_IE = FALSE;
    PWM_IS_ON = FALSE;
    LED = FALSE;
  }
  
  // we would need to check on these things: 
  // * do we need to measure again the sensor?
  // * swap over the cnt?
  if(--measure_ilum_time_cnt == 0u)
  {
    
    if(PWM_LUZ_START == true)
    {
      measure_ilum_time_cnt = MEASURE_ILUMINATION_TIME_CNT_ON_STARTUP;
    }
    else
    {
      measure_ilum_time_cnt = MEASURE_ILUMINATION_TIME_CNT_BASE;
    }
    
    MEASURE_ILUM_FLG = true;
    // measure_ilumination();
    
  }
  
#if 0
  if ((MEASURE_ILUM_FLG == true) && (PWM_IS_ON == false))
  {
    measure_ilumination();
    MEASURE_ILUM_FLG = false;
  }
#endif    
  
}



#elif 1

void pwm_luz_time_update(void){
#if DEBUGGING_BB_IS_ON
static uint8_t db_cnt = 10;
#endif          
  if(PWM_LUZ_START == true)
  {
#if DEBUGGING_BB_IS_ON  
    if(--db_cnt == 0)
    {
      DB_PRINT("\r\nL: ");
      UART_int(pwm_luz_startup_cnt);
      db_cnt = 10;
    }
#endif           
    if(--pwm_luz_startup_cnt == 0)
    {
      PWM_LUZ_START = false;
      DB_PRINT("\r\nL_START_DONE\r\n");
    }
  }
  
#if 1 
  if ((MEASURE_ILUM_FLG == true) && (PWM_IS_ON == false))
  {
#if DEBUGGING_BB_IS_ON          
        DB_LED_1_OFF;
        DB_PRINT("\r\nM ");
        UART_int(pwm_luz_startup_cnt);
#endif            
    measure_ilumination();
    MEASURE_ILUM_FLG = false;
    
  }
#endif    

  
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
#if DEBUGGING_BB_IS_ON          
        DB_PRINT("\r\nL_OFF ");
        UART_int(pwm_luz_startup_cnt);
        DB_LED_1_ON;
#endif        
#if PWM_LUZ_DEBUG
        LED_SIMUL_OFF;
#endif        
      }
      else
      {
        
        pwm_luz.pwm_onoff_time_cnt = pwm_luz.on_time;
        TMR0_IE = TRUE;
        PWM_IS_ON = TRUE;
        DB_PRINT("\r\nL_ON");
      }
      
#if DEBUGGING_BB_IS_ON
// we have a new setting here...
      DB_PRINT("\r\Nl_c: ");
      UART_int(pwm_luz.pwm_onoff_time_cnt);
#endif         
      
    }
 
  }
  else
  {
    pwm_luz.pwm_onoff_time_cnt = pwm_luz.off_time;
    TMR0_IE = FALSE;
    PWM_IS_ON = FALSE;
    LED = FALSE;
  }
  
  // we would need to check on these things: 
  // * do we need to measure again the sensor?
  // * swap over the cnt?
  if(--measure_ilum_time_cnt == 0u)
  {
    
    if(PWM_LUZ_START == true)
    {
      measure_ilum_time_cnt = MEASURE_ILUMINATION_TIME_CNT_ON_STARTUP;
    }
    else
    {
      measure_ilum_time_cnt = MEASURE_ILUMINATION_TIME_CNT_BASE;
    }
    
    MEASURE_ILUM_FLG = true;
    // measure_ilumination();
    
  }
  
#if 0
  if ((MEASURE_ILUM_FLG == true) && (PWM_IS_ON == false))
  {
    measure_ilumination();
    MEASURE_ILUM_FLG = false;
  }
#endif    
  
}


#else
  
void pwm_luz_time_update(void){
  
  if(PWM_LUZ_START == true)
  {
    DB_PRINT("\r\nL: ");
    UART_int(pwm_luz_startup_cnt);
    if(--pwm_luz_startup_cnt == 0)
    {
      PWM_LUZ_START = false;
    }
  }

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
      
#if DEBUGGING_BB_IS_ON
// we have a new setting here...
      DB_PRINT("\r\Nl_c: ");
      UART_int(pwm_luz.pwm_onoff_time_cnt);
#endif         
      
    }
 
  }

  
  // we would need to check on these things: 
  // * do we need to measure again the sensor?
  // * swap over the cnt?
  if(--measure_ilum_time_cnt == 0u)
  {
    
    if(PWM_LUZ_START == true)
    {
      measure_ilum_time_cnt = MEASURE_ILUMINATION_TIME_CNT_ON_STARTUP;
    }
    else
    {
      measure_ilum_time_cnt = MEASURE_ILUMINATION_TIME_CNT_BASE;
    }
    
 
    measure_ilumination();
    
  }
  
  
}


#endif

uint8_t get_pwm_luz_pwm_value(void){
  
  
  return pwm_luz.pwm_value;
  
}


//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //


static void measure_ilumination(void){
  
  uint8_t t_val = 0;
  bool temp_flg = false;
  
  bool temp_IE = TMR0_IE;
  
  // measure the adc from the sensor and compare to thresholde
  // if there is a change --> run the change setter for on or for off
#if DEBUGGING_BB_IS_ON&&0  // DEBUGGING_IS_ON&&PWM_LUZ_DEBUG&&0 //
  DB_PRINT("\r\nIlu: ");
#endif  
  
  TMR0_IE = FALSE;
  LED = false;  // so that we are not measuring the LED
  
  
  
  // and now we need a delay to assure that 
  // we are not measring the LED actually
  // easiest solution is to give a 30ms delay
  
  // __delay_ms(30);
  
  if(FAST_CLOCK == false)  
  {
    __delay_ms(4);

  }
  else
  {
    __delay_ms(32);
  
  }
  
#if PWM_LUZ_DEBUG
  LED_SIMUL_OFF;
#endif        
  
  t_val = read_ilum_sensor(); 
  
  TMR0_IE = temp_IE;
  
#if INVERTED_LDR_SENSOR
  temp_flg = ( t_val < LUZ_ADC_DARK_THRESHOLD_INVERTED ); // 70
#else    
  temp_flg = ( t_val > LUZ_ADC_DARK_THRESHOLD );  // 230
#endif

#if FILE_DETECTOR_DB_ENABLED  // DEBUGGING_BB_IS_ON&&0  // DEBUGGING_IS_ON&&PWM_LUZ_DEBUG&&0// 
  
  DB_PRINT("\r\nIlum: ");
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

 #if DEBUGGING_BB_IS_ON  // DEBUGGING_IS_ON&&PWM_LUZ_DEBUG&&0//     
      DB_PRINT("\r\nLUZ_h_OFF\r\n");
#endif      
    }
    else
    {
#if DEBUGGING_BB_IS_ON  // DEBUGGING_IS_ON&&PWM_LUZ_DEBUG&&0//       
      DB_PRINT("\r\nLUZ_h_ON\r\n");
#endif      
    }
  }
}


#endif // COMPILE_WITH_PWM_LUZ



#if VREF_METHOD

static uint8_t read_bat_value(void){

  uint16_t adcvalue;
  uint8_t adcconv;
  uint16_t bat_val;
  
#if 1  
  
  const uint32_t R_div_Vref_const = 8204;
  uint16_t ADC_Vref = 0;
  
#if 1

  FVRCON = 0x82;  // 10000010
  while(FVRCONbits.FVRRDY == false)
  {
    // wait to stabilize...
  }
  
  ADC_Vref = ConversionAdc(RIGHT_JUSTIFIED, VREF_ADC_CHANNEL);
  FVRCONbits.FVREN = false;
 
#if 0 
  DB_PRINT("\r\nVref: ");
  UART_int(ADC_Vref);
#endif
  
#else  

  adc_set_vref_adc_value();
  
#endif  

  adcvalue = ConversionAdc(RIGHT_JUSTIFIED, BATERIA_ADC_CHANNEL);
#if 0  
  DB_PRINT("  Vbadc: ");
  UART_int(adcvalue);
#endif  
  if(ADC_Vref != 0)
	{
		bat_val = (adcvalue * R_div_Vref_const) / ADC_Vref;
	}
  
  
  
  // bat_val = calculate_mV_from_ADC(adcvalue);
 #if 0  
  DB_PRINT("  BAT: ");
  UART_int(bat_val);
#endif
#if DEBUGGING_BB_IS_ON  
  gd.db_adc_value = adcvalue;
#endif  

#if 0
  adcconv = calculate_voltage_from_input_value(adcvalue);
#else  
  adcconv = bat_val / 100;  // calculate_voltage_from_input_value(adcvalue);
#endif  

#if 0
  DB_PRINT("  BATv: ");
  UART_int(adcconv);
  DB_PRINT("\r\n");
#endif   
  
#endif

  return adcconv;
  
}



#elif USE_ADC_OVERSAMPLING

static uint8_t read_bat_value(void){

  uint16_t adcvalue;
  uint8_t adcconv;

  adcvalue = ConversionAdc(RIGHT_JUSTIFIED, BATERIA_ADC_CHANNEL);
  
#if DEBUGGING_BB_IS_ON  
  gd.db_adc_value = adcvalue;
#endif  

  adcconv = calculate_voltage_from_input_value(adcvalue);

  return adcconv;
  
}


#elif 1

static uint8_t read_bat_value(void){

  uint8_t adcvalue;
  uint16_t adcconv;

  ConversionAdc(RIGHT_JUSTIFIED, BATERIA_ADC_CHANNEL);


  adcconv = ( ((uint16_t)ADRESH * 256) + ADRESL );
  
#if DEBUGGING_BB_IS_ON  
  gd.db_adc_value = adcconv;
#endif  

  adcvalue = calculate_voltage_from_input_value(adcconv);

  return adcvalue;
  
}

#else

static uint16_t read_bat_value(void){

  // uint16_t adcvalue;
  uint16_t adcconv;
  
  ConversionAdc(RIGHT_JUSTIFIED, BATERIA_ADC_CHANNEL);

  adcconv = ( ((uint16_t)ADRESH * 256) + ADRESL );

  // adcvalue = calculate_voltage_from_input_value(adcconv);

  // baterie_mV = (adcvalue & 0x00ff);

  return adcconv;
  
}

#endif






#if 0

// 16 words longer!! incredible but yes
static uint16_t calculate_voltage_from_input_value(uint16_t value){
  
  uint16_t val_1 = 0;
  uint16_t val_2 = 0;
  uint16_t ret_value = 0u;
  
  val_1 = (5 * value / 9) + 2;
  
  val_2 = (17 * value + 49) >> 4;
  
  val_2 = val_2 / 3u;
  
  ret_value = (val_1 + val_2) / 7;
  
  return ret_value;

}  
#else


// 4 words shorter
static uint8_t calculate_voltage_from_input_value(uint16_t value){
  
  uint16_t val_1 = 0;
  uint16_t val_2 = 0;
  uint16_t ret_value = 0u;
  
  val_1 = (5 * value / 9) + 2;
  
  val_2 = (17 * value + 49 ) / 48;
  
  ret_value = (val_1 + val_2) / 7;
  
  return (uint8_t)(ret_value & 0x00FF);
  
  // return ret_value;
  
}



#endif




// EOF