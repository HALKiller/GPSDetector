#ifndef EEPROM_H
#define EEPROM_H

#include "stdint.h"


void write_eeprom(uint8_t the_address, uint8_t data);


uint8_t LeerEeprom ( uint8_t direccion );
















































#endif // EEPROM_H