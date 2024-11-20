// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

//  **********************  COMMENT BLOCK  ************************  //
//writing a small ringbuffer for the rx reception 


#include "ring_buffer.h"

#include "Global.h"

#include "handlers.h"

#include "UART.h"

// #include <stdint.h>

#include "io_port_sfr_names.h"

#if DEBUGGING_IS_ON
#define RING_BUFFER_SIZE (uint8_t)10u
#else
#define RING_BUFFER_SIZE (uint8_t)40u  
#endif

#define NOT_SEARCH_THE_BUG 1

#if 1

#if MISRA_COMPLIANT == 1

static struct r_buffer{
	unsigned char data[RING_BUFFER_SIZE];
	int8_t read_index;
	int8_t write_index;
	uint8_t buffer_length;
}ring_buff;

#else
  
struct r_buffer{
	unsigned char data[RING_BUFFER_SIZE];
	uint8_t read_index;
	uint8_t write_index;
	uint8_t buffer_length;
}ring_buff;

#endif

const uint8_t const_buffer_size = RING_BUFFER_SIZE;




//  **********************  PUBLIC FUNCTIONS BODY  ************************  //
//  **********************  PUBLIC FUNCTIONS BODY  ************************  //

void set_data_value_into_buffer(uint8_t next_char){

#if NOT_SEARCH_THE_BUG	
	// but first test if buffer is full
	if(const_buffer_size != ring_buff.buffer_length)
	{
		ring_buff.data[ring_buff.write_index] = next_char;
		ring_buff.buffer_length++;
		ring_buff.write_index++;
		if(ring_buff.write_index == const_buffer_size)
		{
			ring_buff.write_index = 0u;
		}		
	}

	handlers_generic_set_handler_FLG(e_ring_buffer_handler);

#endif

}


#if 0

uint8_t get_data_value_from_buffer(void){
	
	uint8_t ret_value = 0;
	uint8_t temp_val = 1;
	bool temp_GIE = GLOBAL_IE;

	// but first test if buffer is empty
	
	GLOBAL_IE = false;
	
	if(ring_buff.buffer_length != false)
	{
		ret_value = ring_buff.data[ring_buff.read_index];
		ring_buff.buffer_length--;
		ring_buff.read_index++;
		if(const_buffer_size == ring_buff.read_index)
		{
			ring_buff.read_index = 0;
		}
		if(ring_buff.buffer_length == 0)
		{
			reset_ring_buffer_handler_FLG();
			// TODO --> reset the buffer handler
		}
			temp_val = ret_value;
	}		
	
	GLOBAL_IE = temp_GIE;	

	
	if(ret_value > 128)
	{
		return false;
	}
	else
	{
		return temp_val;
	}
	// return ret_value;
	
}


#else
	

uint8_t get_data_value_from_buffer(void){

#if NOT_SEARCH_THE_BUG	
	
	uint8_t ret_value = 0;
	bool temp_GIE = GLOBAL_IE;
  uint8_t db_var = 0;
	// but first test if buffer is empty
	
	GLOBAL_IE = false;
	
	if(ring_buff.buffer_length != false)
	{
		ret_value = ring_buff.data[ring_buff.read_index];
		ring_buff.buffer_length--;
		ring_buff.read_index++;
		if(const_buffer_size == ring_buff.read_index)
		{
			ring_buff.read_index = 0;
		}
		if(ring_buff.buffer_length == 0)
		{
			reset_ring_buffer_handler_FLG();
			// TODO --> reset the buffer handler
		}
			
	}		
	
	GLOBAL_IE = temp_GIE;	
  
#if 0

  db_var = ret_value;
  
	UART_int(db_var);
  
#endif

	return db_var;//ret_value;

#endif
	
}


#endif


#if 1
void get_data_from_buffer_with_pnt(uint8_t *rx_data){

#if NOT_SEARCH_THE_BUG	
	uint8_t ret_value = 0;
	bool temp_GIE = GLOBAL_IE;
  uint8_t db_var = 0;
	// but first test if buffer is empty
	
	GLOBAL_IE = false;

  
	if(ring_buff.buffer_length != false)
	{
		ret_value = ring_buff.data[ring_buff.read_index];
		ring_buff.buffer_length--;
		ring_buff.read_index++;
		if(const_buffer_size == ring_buff.read_index)
		{
			ring_buff.read_index = 0;
		}
		if(ring_buff.buffer_length == 0)
		{
			reset_ring_buffer_handler_FLG();
			// TODO --> reset the buffer handler
		}
			
	}		
	
	GLOBAL_IE = temp_GIE;	
  
#if 1

  *rx_data = ret_value;

  
	// UART_int(db_var);
  
#endif



#endif
	
}


#endif


void flush_ring_buffer(void){

#if NOT_SEARCH_THE_BUG	
	
	RCSTAbits.CREN = FALSE;
	ring_buff.write_index = 0u;
	ring_buff.read_index = 0u;
	ring_buff.buffer_length = 0u;
	reset_ring_buffer_handler_FLG();
	RCSTAbits.CREN = TRUE;

#endif	
}










#endif





// EOF