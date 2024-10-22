// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //

// this template is made for the 16Fxxxx Chip --> because there are Ansel Channels instead of 
// Independent register For analog select for each port 
// TRIS and PORT PINS are fine for all 8bit MCU
// but in the meantime it got extended to various PIC16F 

//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //



#include "xc.h"

#include <stdint.h>
#include "device_driver_config.h"
#include "Global.h"
#include "UART.h"
#include "my_assert.h"

#include "io_port_sfr_names.h"

//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 

#define INPUT 1
#define OUTPUT 0
#define NO_CH (uint8_t)0xFF
#define ANALOG (uint8_t)1
#define DIGITAL (uint8_t)0
#define GPIO 1


#define SFR_DNE (uint8_t)0xFF

#define NUM_PINS_PER_PORT 8
#define EACH_PORT_HAS_AN_ANSEL_REGISTER 1
#define LATCH_REGISTER_EXISTS 1


#if USE_DEVICE_DRIVER

//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

static volatile uint8_t sPortA = 0;
static volatile uint8_t sPortB = 0;
static volatile uint8_t sPortC = 0;

static struct udt_num_pos{
  
  uint8_t number;
  uint8_t position;
  
}channel_num_pos;

enum{
	
	NUM_PORTS_E = 3,
	
};

#if 1
// this gets a forward declaration in the header
struct IO_ConfigType {
	
	uint8_t Channel;
	uint8_t PinType;
	uint8_t AN_Channel;
	uint8_t Direction;
	uint8_t Data;
	uint8_t Function;
	
};

#else

// this gets a forward declaration in the header
typedef struct IO_ConfigType {
	
	uint8_t Channel;
	uint8_t PinType;
	uint8_t AN_Channel;
	uint8_t Direction;
	uint8_t Data;
	uint8_t Function;
	
}IO_ConfigType;

#endif

//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 



#if HW_GPS_DETECTOR

static const IO_ConfigType IO_Config_Port[] = {
	
  //	PORT_PIN			       AN_DIGI		AN_CH		  TRISA		Init  PERIFERIC			
	{ IO_SYNC_AD9954 	  ,DIGITAL		,0			,OUTPUT		,LOW,		GPIO },	 //	RA_0  DONE
	{ IO_SDIO_AD9954    ,DIGITAL		,1			,OUTPUT		,LOW,		GPIO },  //	RA_1  DONE
	{ IO_SCLK_AD9954 		,DIGITAL		,2			,OUTPUT		,LOW,	  GPIO },  //	RA_2  DONE
	{ IO_BATERIA 	      ,ANALOG		  ,3			,INPUT		,LOW,		GPIO },  //	RA_3  DONE
	{ IO_VDD_AD9954 	  ,DIGITAL		,NO_CH	,OUTPUT		,LOW,		GPIO },	 //	RA_4  DONE
	{ IO_LDR 				    ,ANALOG		  ,4			,INPUT		,LOW,		GPIO },  //	RA_5  DONE
	{ IO_FREE_RA6 	    ,DIGITAL		,NO_CH	,OUTPUT		,LOW,	  GPIO },  //	RA_6  DONE
	{ IO_DB_LED_1 		  ,DIGITAL		,NO_CH	,OUTPUT		,LOW,		GPIO },	 //	RA_7  DONE
	
	{ IO_TILT_SENSOR    ,DIGITAL		,12			,INPUT		,LOW,		GPIO },  //	RB_0  DONE
	{ IO_FREE_RB1 		  ,DIGITAL		,10			,OUTPUT		,LOW,		GPIO },  //	RB_1
	{ IO_PS0_AD9954 		,DIGITAL		,8			,OUTPUT		,LOW,		GPIO },  //	RB_2  DONE
	{ IO_PS1_AD9954 		,DIGITAL		,9			,OUTPUT		,LOW,		GPIO },  //	RB_3  DONE
	{ IO_UPDATE_AD9954 	,DIGITAL		,11			,OUTPUT		,LOW,		GPIO },  //	RB_4  DONE
	{ IO_FREE_RB5 		  ,DIGITAL		,13			,OUTPUT		,LOW,		GPIO },  //	RB_5
	{ IO_ICSPCLCK 			,DIGITAL		,NO_CH	,OUTPUT		,LOW,		GPIO },  //	RB_6
	{ IO_ICSPDAT 	      ,DIGITAL		,NO_CH	,OUTPUT		,LOW,		GPIO },  //	RB_7

	{ IO_FREE_RC0 			,SFR_DNE		,NO_CH	,OUTPUT		,LOW,		GPIO },  //	RC_0
	{ IO_FREE_RC1 			,SFR_DNE		,NO_CH	,OUTPUT		,LOW,		GPIO },  //	RC_1
	{ IO_RESET_AD9954 	,SFR_DNE		,NO_CH	,OUTPUT		,HIGH,	GPIO },  //	RC_2  DONE
	{ IO_FREE_RC3 			,SFR_DNE		,NO_CH	,OUTPUT		,LOW,		GPIO },  //	RC_3
	{ IO_FREE_RC4 			,SFR_DNE		,NO_CH	,OUTPUT		,LOW,		GPIO },	 //	RC_4
	{ IO_DB_LED_2	      ,SFR_DNE		,NO_CH	,OUTPUT		,LOW,		GPIO },  //	RC_5  DONE
	{ IO_UART_TX_PC 		,SFR_DNE		,NO_CH	,OUTPUT		,LOW,		GPIO },  //	RC_6
	{ IO_UART_RX_PC 		,SFR_DNE		,NO_CH	,INPUT		,LOW,		GPIO },  //	RC_7
	// { MCLR					,DIGITAL		,NO_CH	,INPUT		,LOW,		GPIO },	 //	RE_1
	
};

#endif

// extern volatile uint8_t PORTA;
// extern volatile uint8_t PORTB;
// extern volatile uint8_t PORTC;

static uint8_t volatile *const ports_address[NUM_PORTS_E] = {
	
	(volatile uint8_t*)&PORTA,
	(volatile uint8_t*)&PORTB,
	(volatile uint8_t*)&PORTC,
	
};

static uint8_t volatile *const shadow_ports[NUM_PORTS_E] = {
	
	(volatile uint8_t*) &sPortA, 
	(volatile uint8_t*) &sPortB, 
	(volatile uint8_t*) &sPortC, 
	
};

static uint8_t volatile *const ports_direction[NUM_PORTS_E] = {
	
	(volatile uint8_t*)&TRISA,
	(volatile uint8_t*)&TRISB,
	(volatile uint8_t*)&TRISC,
	
};


#if LATCH_REGISTER_EXISTS

static uint8_t volatile *const port_latch[NUM_PORTS_E] = {
	
	(volatile uint8_t*)&LATA,
	(volatile uint8_t*)&LATB,
	(volatile uint8_t*)&LATC,
	
};

#else
  
static uint8_t volatile *const port_latch[NUM_PORTS_E] = {
	
	(volatile uint8_t*)&PORTA,
	(volatile uint8_t*)&PORTB,
	(volatile uint8_t*)&PORTC,
	
};

#endif


#if EACH_PORT_HAS_AN_ANSEL_REGISTER

static uint8_t volatile *const analog_channel[NUM_PORTS_E] = {
	
	(volatile uint8_t*)&ANSELA,
	(volatile uint8_t*)&ANSELB,
	(volatile uint8_t*)&ANSELB,	// this is only a workaround!
	
};

#else

static uint8_t volatile *const analog_channel[2] = {
	
	(volatile uint8_t*)&ANSEL,
	(volatile uint8_t*)&ANSELH,
	
};

#endif

static const uint8_t pins[NUM_PINS_PER_PORT] = {
	
	(1U << 0), (1U << 1), (1U << 2), (1U << 3), 
	(1U << 4), (1U << 5), (1U << 6), (1U << 7), 
	
};


//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //




//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

static void set_channel_number_position(uint8_t channel);
static void clear_bit(uint8_t volatile *reg_pnt, uint8_t pos);
static void set_bit(uint8_t volatile *reg_pnt, uint8_t pos);



//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

#if 0
const IO_ConfigType *set_pnt_to_struct(void){
	
	// const IO_ConfigType *t_pnt = IO_Config_Port;
	
	// return t_pnt;
	return IO_Config_Port;
	
}

#endif


uint8_t d_driver_get_AN_from_channel(IO_ChannelType channel){
	
	return IO_Config_Port[channel].AN_Channel;
	
}




void IO_Write_channel(IO_ChannelType channel, uint8_t setter){
	
  uint8_t volatile *reg_pnt;

// DB_LED = true;  
// 64(!)us the next instruction

// i reduced it to 4(!!)us

  set_channel_number_position((uint8_t)channel);
// DB_LED = false;
  reg_pnt = port_latch[(int8_t)channel_num_pos.number];
  
  
	if(setter == False)
	{

// 5us the next instruction
    clear_bit(reg_pnt, channel_num_pos.position); 

		// IO_Clear_channel(channel);
    
	}
	else
	{
// 5us the next instruction
    set_bit(reg_pnt, channel_num_pos.position);  
		// IO_Set_channel(channel);

	}
	
}


void IO_Set_channel(IO_ChannelType channel){

#if 0	

	*port_latch[channel / NUM_PINS_PER_PORT] = 
	*port_latch[channel / NUM_PINS_PER_PORT] | (1U<<(channel%NUM_PINS_PER_PORT));

#elif 1
  
  uint8_t volatile *reg_pnt;
  
  set_channel_number_position((uint8_t)channel);

  reg_pnt = port_latch[(int8_t)channel_num_pos.number];
  set_bit(reg_pnt, channel_num_pos.position);
  

  
#else	
  
	*port_latch[channel / NUM_PINS_PER_PORT] = 
	*port_latch[channel / NUM_PINS_PER_PORT] | pins[channel%NUM_PINS_PER_PORT];
  
#endif	
	
}



void IO_Clear_channel(IO_ChannelType channel){

#if 0	

	*port_latch[channel / NUM_PINS_PER_PORT] = 
	*port_latch[channel / NUM_PINS_PER_PORT] & ~(1U<<(channel%NUM_PINS_PER_PORT));

#elif 1

  uint8_t volatile *reg_pnt;
  
  set_channel_number_position((uint8_t)channel);

  reg_pnt = port_latch[(int8_t)channel_num_pos.number];
  clear_bit(reg_pnt, channel_num_pos.position);
  
  
#else
	
  uint8_t volatile *reg_pnt;

	*port_latch[channel / NUM_PINS_PER_PORT] = 
	*port_latch[channel / NUM_PINS_PER_PORT] & ~pins[channel%NUM_PINS_PER_PORT];
	
#endif
	
}



// inline void IO_Toggle_channel(uint8_t porter, uint8_t piner){
void IO_Toggle_channel(IO_ChannelType channel){
	
#if 1
	
  uint8_t volatile *reg_pnt;


  set_channel_number_position((uint8_t)channel);

  reg_pnt = port_latch[(int8_t)channel_num_pos.number];
  
	*reg_pnt = *reg_pnt ^ pins[channel_num_pos.position];

#elif 1

// 72us exe time with 8MHz clock!!
	*port_latch[porter] = 
	*port_latch[porter] ^ pins[piner];

#else
	// 92 us exe time with 8MHz clock!!
	*port_latch[channel / NUM_PINS_PER_PORT] = 
	*port_latch[channel / NUM_PINS_PER_PORT] ^ pins[channel%NUM_PINS_PER_PORT];
	
#endif	
	
	
}


#if 0
void IO_update_Port_for_sPort(IO_ChannelType channel){
	
	
	*port_latch[channel / NUM_PINS_PER_PORT] = *shadow_ports[channel / NUM_PINS_PER_PORT];

	
}
#endif



uint8_t IO_Read_channel(IO_ChannelType channel){
	
  
#if 0

  
  uint8_t volatile *reg_pnt;
 
  set_channel_number_position((uint8_t)channel);

  reg_pnt = ports_address[channel_num_pos.number];

	return (!(*reg_pnt & pins[channel_num_pos.position]) == 0);


#elif 1

  uint8_t ret_value = 1;
  
  uint8_t volatile *reg_pnt;
 

 
  set_channel_number_position((uint8_t)channel);

  reg_pnt = ports_address[(int8_t)channel_num_pos.number];

	if((*reg_pnt & pins[(int8_t)channel_num_pos.position]) == 0u)
		
	{
		ret_value = (uint8_t)0u;
	}
  
  return ret_value;
  
#else

	if((*ports_address[channel / NUM_PINS_PER_PORT] & pins[channel%NUM_PINS_PER_PORT]) == 0)
		
	{
		ret_value = 0;
	}

  return ret_value;
  
#endif


	
	
	
}

//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //




#if ALL_INTERNAL_DECLARED||1

void IO_Init(void)

	
	
#else
	
void IO_Init(const IO_ConfigType *ConfigT)
	
#endif	

{
  const IO_ConfigType *ConfigT = &IO_Config_Port[0];
  
	int8_t hlooper = 0;


  uint8_t volatile *sfr_pnt;
	
	// assuring that the const declarations are coherent...
	assert(sizeof(IO_Config_Port) / sizeof(IO_Config_Port[0]) == NUM_DIGITAL_PINS);
	
	for(hlooper = 0; hlooper < NUM_DIGITAL_PINS; hlooper++)
	{
		
		// get the correct Port...
		// number = ConfigT[hlooper].Channel / NUM_PINS_PER_PORT;
		// ... and then the correct pin
		// position = ConfigT[hlooper].Channel % NUM_PINS_PER_PORT;
		
    
    
    assert((int8_t)ConfigT[hlooper].Channel == hlooper);
    
    set_channel_number_position(ConfigT[hlooper].Channel);
    
#if EACH_PORT_HAS_AN_ANSEL_REGISTER
		
		// does that sfr exist?
		if(ConfigT[hlooper].PinType != SFR_DNE)		
		{
      
      sfr_pnt = analog_channel[(int8_t)channel_num_pos.number];
      
			if((ConfigT[hlooper].PinType == ANALOG)	&& (ConfigT[hlooper].AN_Channel != NO_CH))
			{
				set_bit(sfr_pnt, channel_num_pos.position);
			}
			else
			{
				clear_bit(sfr_pnt, channel_num_pos.position);
			}
		}

#elif 1
		if(ConfigT[hlooper].AN_Channel != NO_CH)
		{
			if(ConfigT[hlooper].PinType == ANALOG)
			{
				*analog_channel[number] = *analog_channel[number] | pins[position];
			}
			else
			{
				*analog_channel[number] = *analog_channel[number] & ~pins[position];
			}
		}
#else

		// the Analog channel if used...--> NO_CH means it is not implemented on that pin
		if(ConfigT[hlooper].AN_Channel != NO_CH)
		{
			if(ConfigT[hlooper].PinType == ANALOG)
			{
				*analog_channel[ConfigT[hlooper].AN_Channel / NUM_PINS_PER_PORT] = 
				*analog_channel[ConfigT[hlooper].AN_Channel / NUM_PINS_PER_PORT] | pins[ConfigT[hlooper].AN_Channel % NUM_PINS_PER_PORT];
			}
			else
			{
				*analog_channel[ConfigT[hlooper].AN_Channel / NUM_PINS_PER_PORT] = 
				*analog_channel[ConfigT[hlooper].AN_Channel / NUM_PINS_PER_PORT] & ~pins[ConfigT[hlooper].AN_Channel % NUM_PINS_PER_PORT];
			}
		}
		
#endif


		// first set the data H/L
    
    
    sfr_pnt = port_latch[channel_num_pos.number];
    
		if(ConfigT[hlooper].Data == HIGH)
		{
			set_bit(sfr_pnt, channel_num_pos.position);
		}
		else
		{
			clear_bit(sfr_pnt, channel_num_pos.position);
		}
		

		// then set the direction
    
    
    sfr_pnt = ports_direction[channel_num_pos.number];
    
		if(ConfigT[hlooper].Direction == INPUT)
		{
			set_bit(sfr_pnt, channel_num_pos.position);
		}
		else
		{
			clear_bit(sfr_pnt, channel_num_pos.position);
		}
	}


}


static void set_channel_number_position(uint8_t channel){
  
  // i am doing that because i know that there are 8 Pins per Port and therefore i 
  // can do it like that...

  channel_num_pos.number = channel >> 3u;

  // ... and then the correct pin
  channel_num_pos.position = channel & 0x07;
  

}



  
static void set_bit(uint8_t volatile *reg_pnt, uint8_t pos){
  
  
  *reg_pnt = *reg_pnt | pins[pos];
  
  
}

static void clear_bit(uint8_t volatile *reg_pnt, uint8_t pos){
  
  
  *reg_pnt = *reg_pnt & ~pins[pos];
  
  
}



//   * * * * * *      I S R  - -  H A N D E L E R     * * * * * * * * * * * * * *   //

#endif  // USE_DEVICE_DRIVER

// EOF