// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "Aux_functions.h"
#include "UART.h"	// only to have the NULL_TERMINATOR...
#include "Global.h"

#include "stddef.h"




// does what it says: clears a single bit in an 16 bit integer --> two parameters:
// a value of a register and the bit positon which shall get cleared
unsigned int clear_single_bit_in_int(unsigned int b_field, uint8_t n_bit){
  unsigned int workint = 1;

	b_field = b_field & ~(workint << n_bit);	// clears the bit....
	
  return b_field;
  
}




#if 0

unsigned int set_single_bit_in_int(unsigned int b_field, uint8_t n_bit) {
    // Check if n_bit is within the range of valid bits for an unsigned int
    // if (n_bit >= sizeof(unsigned int) * CHAR_BIT) {
        // return b_field; // Return the original value if the bit position is invalid
    // }

    // Set the specific bit
    return b_field | (1U << n_bit);
}

#else

// does what it says: sets a single bit in an 16 bit integer --> two parameters:
// a value of a register and the bit positon which shall get set
unsigned int set_single_bit_in_int(unsigned int b_field, uint8_t n_bit){
  unsigned int workint = 1;

	b_field = b_field | (workint << n_bit);	// sets the bit....
	return b_field;
}

#endif

// does what it says: test a single bit in an 16 bit integer --> two parameters:
// a value of a register and the bit positon which shall get tested
uint8_t test_single_bit_in_int(unsigned int b_field, uint8_t n_bit){
 
 unsigned int workint = 1;
// unsigned int result = 0;

	if((workint << n_bit) == (b_field & (workint << n_bit)))
  {	
		return true;
	}
	return false;

}


#if 0
// does what it says: test a single bit in an 16 bit integer --> two parameters:
// a value of a register and the bit positon which shall get tested
uint8_t test_bit_in_int(unsigned int b_field, uint8_t n_bit){
  unsigned int workint = 1;	

  // tests the bit
	if((workint << n_bit) == (b_field & (workint << n_bit)))
  {	
		return true;
	}
	return false;
	
}

#else
uint8_t test_bit_in_int(unsigned int b_field, uint8_t n_bit) {
  
  // Check if n_bit is within the range of valid bits
  if (n_bit >= sizeof(unsigned int) * 8) {
      return 0; // or return false; if you prefer
  }

  // Create a mask by shifting 1 to the left by n_bit positions
  unsigned int mask = 1U << n_bit;

  // Return whether the specific bit is set
  return (b_field & mask) != 0; // Returns 1 if the bit is set, 0 otherwise
  
}
#endif

#if NOT_UNUSED_FUNCTIONS






// does what it says: sets a single bit in an 16 bit integer --> two parameters:
// a value of a register and the bit positon which shall get set
void set_bit(unsigned int *b_field, uint8_t n_bit){
unsigned int workint = 1;

	*b_field = *b_field | (workint << n_bit);	// sets the bit....
	
}

// does what it says: clears a single bit in an 16 bit integer --> two parameters:
// a value of a register and the bit positon which shall get cleared
void clear_bit(unsigned int *b_field, uint8_t n_bit){
unsigned int workint = 1;

	*b_field = *b_field & ~(workint << n_bit);	// clears the bit....
	
}


// does what it says: clears or sets a single bit in an 16 bit integer --> three parameters:
// a pointer to a register, the bit positon which shall get manipulated and if it gets set or reset
void clear_or_set_bit(unsigned int *b_field, uint8_t n_bit, uint8_t clear_or_set){
unsigned int workint = 1;
	
	if(clear_or_set){
		*b_field = *b_field | (workint << n_bit);	// sets the bit....
	}else{
		*b_field = *b_field & ~(workint << n_bit);	// clears the bit....
	}
	
}

// does what it says: sets a single bit in an 16 bit integer --> two parameters:
// a value of a register and the bit positon which shall get set
unsigned int set_bit_in_int(unsigned int b_field, uint8_t n_bit){
unsigned int workint = 1;

	b_field = b_field | (workint << n_bit);	// sets the bit....
	return b_field;
}





// does what it says: clears a single bit in an 16 bit integer --> two parameters:
// a value of a register and the bit positon which shall get cleared
unsigned int clear_bit_in_int(unsigned int b_field, uint8_t n_bit){
unsigned int workint = 1;

	b_field = b_field & ~(workint << n_bit);	// clears the bit....
	return b_field;
}

#endif









#if NOT_UNUSED_FUNCTIONS



// does what it says: sets a single bit in an 32 bit integer --> two parameters:
// a value of a register and the bit positon which shall get set
void set_bit_long_int(unsigned long int *b_field, uint8_t n_bit){
unsigned int workint = 1;

	*b_field = *b_field | (workint << n_bit);	// sets the bit....
	
}

// does what it says: clears a single bit in an 32 bit integer --> two parameters:
// a value of a register and the bit positon which shall get cleared
void clear_bit_long_int(unsigned long int *b_field, uint8_t n_bit){
unsigned int workint = 1;

	*b_field = *b_field & ~(workint << n_bit);	// clears the bit....
	
}


// does what it says: clears or sets a single bit in an 32 bit integer --> three parameters:
// a pointer to a register, the bit positon which shall get manipulated and if it gets set or reset
void clear_or_set_bit_long_int(unsigned long int *b_field, uint8_t n_bit, uint8_t clear_or_set){
unsigned int workint = 1;
	
	if(clear_or_set){
		*b_field = *b_field | (workint << n_bit);	// sets the bit....
	}else{
		*b_field = *b_field & ~(workint << n_bit);	// clears the bit....
	}
	
}


// does what it says: test a single bit in an 32 bit integer --> two parameters:
// a value of a register and the bit positon which shall get tested
uint8_t test_bit_in_long_int(unsigned long int b_field, uint8_t n_bit){
unsigned long int workint = 1;


	if((workint << n_bit) == (b_field & (workint << n_bit))){	// tests the bit
		return true;
	}
	return false;

}


#endif










#if NOT_UNUSED_FUNCTIONS



// just a testing function on the divider --> returns true or false
uint8_t datavalidacion_evitar_zero_division(unsigned int divisor){
	if(divisor == 0){
		return false;
	}else{
		return true;
	}
}





//   * * * * * * * * * * * * * * *     R O M   - -   R A M     S T R I N G   H A N D L E R S     * * * * * * * * * * //
//   * * * * * * * * * * * * * * *     R O M   - -   R A M     S T R I N G   H A N D L E R S     * * * * * * * * * * //
//   * * * * * * * * * * * * * * *     R O M   - -   R A M     S T R I N G   H A N D L E R S     * * * * * * * * * * //



void copy_ROM_string_to_RAM (const char *rom_pnt, char *ram_pnt){
#define LOCAL_UART 0
uint8_t m_length = 16;

#if LOCAL_UART
char *t_pnt;
t_pnt = ram_pnt;
#endif

	while(*rom_pnt != NULL_TERMINATOR && m_length > 0){
		*ram_pnt = *rom_pnt;
		ram_pnt++;
		rom_pnt++;
		m_length--;
	}
	*ram_pnt = NULL_TERMINATOR;
	
	
#if LOCAL_UART
	UWT("The copied string: ");
	UWT(t_pnt);
	UART_CRLF;
#endif

#undef LOCAL_UART
}

// returns 0 if the string was too long
uint8_t copy_ROM_to_RAM (const char *rom_pnt, char *ram_pnt){
// char *t_pnt;
uint8_t m_length = 31;

	// t_pnt = ram_pnt;
	
	if(rom_pnt == NULL)
	{		
		return 1;		
	}
	
	
	while(*rom_pnt != NULL_TERMINATOR && m_length > 0)
	{
		*ram_pnt = *rom_pnt;
		ram_pnt++;
		rom_pnt++;
		m_length--;
	}
	
	*ram_pnt = NULL_TERMINATOR;
	
	if(*rom_pnt == NULL_TERMINATOR)
	{
		m_length = 1;
	}
	
	return m_length;
	

}


void copy_ROM_to_RAM_and_send_to_UART(const char *in_rom){
char the_text[32];
char *message_pnt;		
uint8_t rnd_cnt = 28;	// these are then 1kByte of data ---quite a biyte...
uint8_t t_flg = 0;
const char *m_pnt = in_rom;

	message_pnt = &the_text[0];

	while (rnd_cnt > 0 && t_flg == false){
		t_flg = copy_ROM_to_RAM(m_pnt, message_pnt);
		rnd_cnt--;
		UWT(message_pnt);
		
		m_pnt = m_pnt + 31;
	}

	
}


#endif


//   * * * * * * * * * * * * * * *     R O M   - -   R A M     S T R I N G   H A N D L E R S     * * * * * * * * * * //
//   * * * * * * * * * * * * * * *     R O M   - -   R A M     S T R I N G   H A N D L E R S     * * * * * * * * * * //
//   * * * * * * * * * * * * * * *     R O M   - -   R A M     S T R I N G   H A N D L E R S     * * * * * * * * * * //









//	EOF