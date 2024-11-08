#ifndef GENERIC_UNION_FLGS_H
#define GENERIC_UNION_FLGS_H

#include <stdint.h>

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

// TODO: --> reset on startup the correct ones
#define SWITCH_CLOCK    gFLAGS.b0
#define FAST_CLOCK      gFLAGS.b1
#define LUZ_ENABLED     gFLAGS.b2 
// #define LUZ_HANDLER_ON  gFLAGS.b3 // that is getting set when the sensor measures it is dark
// #define DOUBLE_PERIOD   gFLAGS.b4
#define TX_150BPS       gFLAGS.b5
#define DEBUG_FLG_PRINT_TIME gFLAGS.b7





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

