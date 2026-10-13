#ifndef AUXFUNCTIONS_H
#define AUXFUNCTIONS_H

#include "Global.h"

#define CPRRU copy_ROM_to_RAM_and_send_to_UART


unsigned int set_single_bit_in_int(unsigned int b_field, uint8_t n_bit);

unsigned int clear_single_bit_in_int(unsigned int b_field, uint8_t n_bit);

uint8_t test_single_bit_in_int(unsigned int b_field, uint8_t n_bit);

uint8_t test_bit_in_int(unsigned int b_field, uint8_t n_bit);




#if 0
unsigned int set_bit_in_int(unsigned int b_field, uint8_t n_bit);

unsigned int clear_bit_in_int(unsigned int b_field, uint8_t n_bit);




void set_bit(unsigned int *b_field, uint8_t n_bit);

void clear_bit(unsigned int *b_field, uint8_t n_bit);

void clear_or_set_bit(unsigned int *b_field, uint8_t n_bit, uint8_t clear_or_set);



	
	
	
void set_bit_long_int(unsigned long int *b_field, uint8_t n_bit);

void clear_bit_long_int(unsigned long int *b_field, uint8_t n_bit);

void clear_or_set_bit_long_int(unsigned long int *b_field, uint8_t n_bit, uint8_t clear_or_set);

uint8_t test_bit_in_long_int(unsigned long int b_field, uint8_t n_bit);


uint8_t datavalidacion_evitar_zero_division(unsigned int divisor);



void copy_ROM_string_to_RAM (const char *rom_pnt, char *ram_pnt);

uint8_t copy_ROM_to_RAM (const char *rom_pnt, char *ram_pnt);

void copy_ROM_to_RAM_and_send_to_UART(const char *in_rom);

#endif

#endif //	AUXFUNCTIONS_H