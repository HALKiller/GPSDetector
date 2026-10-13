#ifndef INT_TO_STR_H
#define INT_TO_STR_H


#include <stdint.h>
#include <stddef.h>

// with this conversion implementation i can simulate an function overloading in c 
// for converting any uint variable to string without having to worry about its size


#define UINT_TO_STR(num, str) uint_to_str(&(num), sizeof(num), str)

void uint_to_str(void* num_ptr, size_t size, char* str);








// void uint8_to_str(uint8_t num, char *str);
// void uint16_to_str(uint16_t num, char *str);
// void uint32_to_str(uint32_t num, char *str);  // The original function

// void int_to_str_converter(uint32_t the_value, char *str_pnt);

// this is basically a function overloading workaround for c

// #define uint_to_str(num, str) _Generic((num),  \
    // uint8_t: uint8_to_str,                    \
    // uint16_t: uint16_to_str,                  \
    // uint32_t: uint32_to_str                   \
// )(num, str)
















































#endif