#ifndef CHECKSUMMING_H
#define CHECKSUMMING_H

#include <stdint.h>



uint8_t checksumming_chcksum_creator(uint8_t *the_data, uint8_t data_len);

uint8_t checksumming_chcksum_checker(uint8_t *the_data, uint8_t data_len);









































#endif //  CHECKSUMMING_H