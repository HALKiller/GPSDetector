#ifndef GENERIC_UNION_FLGS_H
#define GENERIC_UNION_FLGS_H

#include <stdint.h>


#if 1

// TODO: --> reset on startup the correct ones
#define SWITCH_CLOCK          gFLAGS.b0
#define FAST_CLOCK            gFLAGS.b1
#define LUZ_ENABLED           gFLAGS.b2 
#define TIMEOUT_FLG           gFLAGS.b3 
#define RTC_TIME_IS_GOOD      gFLAGS.b4
#define TX_150BPS             gFLAGS.b5
#define RTC_ALARM_ON          gFLAGS.b6
#define DEBUG_FLG_PRINT_TIME  gFLAGS.b7


// FLAGS   
// #define LUZ_ENABLED     gd_flags.b0 // from the eeprom cfg
#define LUZ_HANDLER_ON      gd_flags.b1 // that is getting set when the sensor measures it is dark
#define DOUBLE_PERIOD       gd_flags.b2
#define COPY_POS_IS_VALID   gd_flags.b3
#define ALWAYS_TRANSMIT     gd_flags.b4
#define PWM_IS_ON           gd_flags.b5 // when the TMR0_IE gers set 
#define BAT_IS_LOW_FLG      gd_flags.b6 // when the bat is under the threshhold
#define BAT_IS_TOO_LOW      gd_flags.b7 // once we reach that we swoff off for one hour



#define DDS_CFG_ERR         dFLAGS.b7
#define STATUS_LED_ON       dFLAGS.b6
#define TILT_SENSOR_ERR     dFLAGS.b5
#define PWM_LUZ_START       dFLAGS.b4
#define MEASURE_ILUM_FLG    dFLAGS.b3
#define RMC_TIME_IS_VALID   dFLAGS.b2
#define GPS_HAS_SYNCED      dFLAGS.b1
#define FREE_ERRFLG0        dFLAGS.b0



#else
 
// that does not compile...
#define SWITCH_CLOCK          gFLAGS.b0
#define FAST_CLOCK            gFLAGS.b1
#define TIMEOUT_FLG           gFLAGS.b3 
#define RTC_TIME_IS_GOOD      gFLAGS.b4
#define PWM_IS_ON             gflags.b5 // when the TMR0_IE gers set 
#define RTC_ALARM_ON          gFLAGS.b6
#define DEBUG_FLG_PRINT_TIME  gFLAGS.b7



#define DOUBLE_PERIOD       gd_flags.b0
#define ALWAYS_TRANSMIT     gd_flags.b1
#define LUZ_HANDLER_ON      gd_flags.b2 // that is getting set when the sensor measures it is dark
#define COPY_POS_IS_VALID   gd_flags.b3
#define TX_150BPS           gd_FLAGS.b4
#define LUZ_ENABLED         gd_FLAGS.b5
#define BAT_IS_LOW_FLG      gd_flags.b6 // when the bat is under the threshhold

#endif



typedef union udt_generic_8bit_union{	
	
  uint8_t reg;
	
	struct{
    
		unsigned b0: 1;
		unsigned b1: 1;
		unsigned b2: 1;
		unsigned b3: 1;
		unsigned b4: 1;
		unsigned b5: 1;
		unsigned b6: 1;
		unsigned b7: 1;

	};
}union8_t;


extern union8_t gFLAGS;
extern union8_t gd_flags;
extern union8_t dFLAGS;






typedef union udt_generic_16bit_union{	
	
  uint16_t reg;
	
	struct{
    
		unsigned b0: 1;
		unsigned b1: 1;
		unsigned b2: 1;
		unsigned b3: 1;
		unsigned b4: 1;
		unsigned b5: 1;
		unsigned b6: 1;
		unsigned b7: 1;
		unsigned b8: 1;
		unsigned b9: 1;
		unsigned b10: 1;
		unsigned b11: 1;
		unsigned b12: 1;
		unsigned b13: 1;
		unsigned b14: 1;
		unsigned b15: 1;
    
	};
}union16_t;


typedef union udt_generic_32bit_union{	
	
  uint32_t reg;
	
	struct{
    
		unsigned b0: 1;
		unsigned b1: 1;
		unsigned b2: 1;
		unsigned b3: 1;
		unsigned b4: 1;
		unsigned b5: 1;
		unsigned b6: 1;
		unsigned b7: 1;
		unsigned b8: 1;
		unsigned b9: 1;
		unsigned b10: 1;
		unsigned b11: 1;
		unsigned b12: 1;
		unsigned b13: 1;
		unsigned b14: 1;
		unsigned b15: 1;
    unsigned b16: 1;
		unsigned b17: 1;
		unsigned b18: 1;
		unsigned b19: 1;
    unsigned b20: 1;
		unsigned b21: 1;
		unsigned b22: 1;
		unsigned b23: 1;
		unsigned b24: 1;
		unsigned b25: 1;
		unsigned b26: 1;
		unsigned b27: 1;
		unsigned b28: 1;
		unsigned b29: 1;
		unsigned b30: 1;
		unsigned b31: 1;

	};
}union32_t;
















#endif // GENERIC_UNION_FLGS_H

