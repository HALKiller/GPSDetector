#ifndef EXTENSION_STRINGS_H
#define	EXTENSION_STRINGS_H



#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


uint8_t AToUint8_t( uint8_t  * s, uint8_t ndigits );

uint8_t * Uint8_tStrchr( uint8_t  * ptr, uint8_t chr );



void DecimalUint16ToA( uint8_t * buf, uint16_t val, uint8_t npos, bool null_to_end );



void DecimalUint8ToA( uint8_t * buf, uint8_t val, uint8_t npos, bool null_to_end );
                      
#if 0
float Uint8_tToF( uint8_t  * str );

void DecimalInt8ToA( uint8_t * buf, int8_t val, uint8_t npos,
                     bool null_to_end );

#endif





























#endif	/* EXTENSION_STRINGS_H */

