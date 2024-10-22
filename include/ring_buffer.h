#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>


void set_data_value_into_buffer(uint8_t next_char);

uint8_t get_data_value_from_buffer(void);

void get_data_from_buffer_with_pnt(uint8_t *rx_data);

void flush_ring_buffer(void);







































#endif	// RING_BUFFER_H