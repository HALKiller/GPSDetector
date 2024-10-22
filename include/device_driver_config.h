#ifndef DEVICE_DRIVER_CONFIG_H
#define DEVICE_DRIVER_CONFIG_H

#include <stdint.h>

#include <Global.h>

#if 1

#define ALL_INTERNAL_DECLARED 0

#if !ALL_INTERNAL_DECLARED

typedef struct IO_ConfigType IO_ConfigType;


#endif

#if 1

#define HIGH 1
#define LOW 0

#else
  
enum{
  
  LOW,
  HIGH,
  
};
#endif

#if HW_GPS_DETECTOR

typedef enum{
	
  IO_SYNC_AD9954 			    ,
  IO_SDIO_AD9954 				  ,
  IO_SCLK_AD9954 		      ,
  IO_BATERIA 	            ,
  IO_VDD_AD9954 	        ,
  IO_LDR 				          ,
  IO_FREE_RA6 	          ,
  IO_DB_LED_1             ,           // IO_LUZ_ON_OFF-->LED outputs

  IO_TILT_SENSOR  ,     // IO_INTERRUPTOR_POSICION 	,
  IO_FREE_RB1 		    ,
  IO_PS0_AD9954 		        ,
  IO_PS1_AD9954 		        ,
  IO_UPDATE_AD9954 		  ,
  IO_FREE_RB5 		    ,
  IO_ICSPCLCK 			    ,
  IO_ICSPDAT 	    ,

  IO_FREE_RC0 			    ,
  IO_FREE_RC1 			    ,
  IO_RESET_AD9954 			    ,
  IO_FREE_RC3 			    ,
  IO_FREE_RC4 				    ,
  IO_DB_LED_2	    , // That gives Valim to the GPS --> in debug we do not need that...
  IO_UART_TX_PC 		    ,	
  IO_UART_RX_PC 		    ,	
  NUM_DIGITAL_PINS
  
}IO_ChannelType;



#endif

// const IO_ConfigType *set_pnt_to_struct(void);

void IO_Init(void);

uint8_t device_driver_get_AN_from_channel(IO_ChannelType channel);

void IO_Write_channel(IO_ChannelType channel, uint8_t setter);

void IO_Set_channel(IO_ChannelType channel);

void IO_Clear_channel(IO_ChannelType channel);

void IO_Toggle_channel(IO_ChannelType channel);

uint8_t IO_Read_channel(IO_ChannelType channel);















#endif












#endif // DEVICE_DRIVER_CONFIG_H