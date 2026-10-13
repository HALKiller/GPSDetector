// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "int2string.h"
#include "stdbool.h"
#include "Global.h"


#include <stdint.h>
// #include <stdio.h>


#if 1


void uint_to_str(void* num_ptr, size_t size, char* str) {
  
  uint32_t num = 0;  // Store the final number in uint32_t to handle all cases

  // Handle based on the size of the input
  if (size == sizeof(uint8_t)) 
  {
    num = *(uint8_t*)num_ptr;  // Dereference as uint8_t
  } 
  else if (size == sizeof(uint16_t)) 
  {
    num = *(uint16_t*)num_ptr; // Dereference as uint16_t
  } 
  else if (size == sizeof(uint32_t)) 
  {
    num = *(uint32_t*)num_ptr; // Dereference as uint32_t   void uint_to_str(void* num_ptr, size_t size, char* str)  num = *(uint32_t*)num_ptr;   V2571. MISRA. Conversions between pointers to objects and integer types should not be performed.
  } 
  else 
  {
    // If size is unsupported, just return an empty string or handle error
    str[0] = NULL_TERMINATOR; // '\0';  // V2568. MISRA. Both operands of an operator should be of the same type category.
    return;
  }

  // Now convert the number to a string (same logic as before)
  char temp[11];  // Buffer for the digits (maximum 10 digits + null terminator)
  int i = 0;

  if (num == 0u) 
  {
    str[i++] = (char)'0'; // 48;
    str[i] = NULL_TERMINATOR;
    return;
  }

  // Convert each digit to character (reverse order)
  while (num != 0u) 
  {
    temp[i++] = (char)(num % 10u) + (char)'0'; // adding straight away the 0x30 necessary for the actual conversion
    num /= 10u;
  }

  // Reverse the digits and store them in the output buffer
  int j = 0;
  while (i > 0) 
  {
    str[j++] = temp[--i];
  }
  str[j] = NULL_TERMINATOR;  // Null terminate the string
}


#elif 1 

// 32 bit --> 4 294 967 295	therefore 10 chars --> for each cahr one nibble --> 5 bytes...or just staright out 10 bytes --> easiest....

// this is functional but only to 16bits --> 
// therefore i cant use it for the long int--> but otherwise it is fine
void int_to_str_converter(uint32_t the_value, char *str_pnt){
	
union {
	uint32_t bcd;	// uint24_t bcd;
	struct
	{
		uint8_t byte_0;
		uint8_t byte_1;
		uint8_t byte_2;
		uint8_t byte_3;
    uint8_t byte_4;
	};

	struct 
	{
		unsigned ones					: 4;
		unsigned tens					: 4;
		unsigned hundreds			: 4;
		unsigned thousands		: 4;
		unsigned t_thousands	: 4;
		unsigned h_thousands	: 4;
    unsigned million			: 4;
		unsigned t_million		: 4;
		unsigned h_million  	: 4;
		unsigned billion    	: 4;
	};
}result;

unsigned char *bcd_pnt = &result.byte_0;

unsigned char hlooper = 0;
unsigned char hlooper_2 = 0;
unsigned char not_zero_flg = 0;

char temp_string[11];
char *temp_strpnt = &temp_string[10];

	*temp_strpnt = NULL_TERMINATOR;
	temp_strpnt--;
		
		
		
	result.bcd = 0;

	for(hlooper = 0; hlooper < 3; hlooper++)
	{
		result.bcd = result.bcd << 1;
		if(the_value >=32768)
		{
			result.bcd = result.bcd + 1;
		}
    
		the_value = (the_value << 1) & 0xFFFF;
	}

	for(hlooper = 0; hlooper < 29; hlooper++)
	{
	
		bcd_pnt = &result.byte_0;
		for(hlooper_2 = 0; hlooper_2 < 5; hlooper_2++)
		{

			if((0x0F & *bcd_pnt) >= 5)
			{
				*bcd_pnt = *bcd_pnt + 3;		
			}
			if((0xF0 & *bcd_pnt) >= 80)
			{
				*bcd_pnt = *bcd_pnt + 48;
			}
			bcd_pnt++;
		}
		
		result.bcd = result.bcd << 1;
		if(the_value >=32768)
		{
			result.bcd = result.bcd + 1;
		}
		the_value = (the_value << 1) & 0xFFFF;;
				
	}
	
	#if 1	// because in the end was this the fastes...
	*str_pnt = result.h_thousands + 0x30;
	if(*str_pnt != 0x30){
		not_zero_flg = true;
		str_pnt++;
	}
	
	*str_pnt = result.t_thousands+ 0x30;
	if((*str_pnt != 0x30) || not_zero_flg){
		not_zero_flg = true;
		str_pnt++;;
	}
	
	*str_pnt = result.thousands+ 0x30;
	if((*str_pnt != 0x30) || not_zero_flg){
		not_zero_flg = true;
		str_pnt++;;
	}

	*str_pnt = result.hundreds+ 0x30;	
	if((*str_pnt != 0x30) || not_zero_flg){
		not_zero_flg = true;
		str_pnt++;;
	}

	*str_pnt = result.tens+ 0x30;	
	if((*str_pnt != 0x30) || not_zero_flg){
		not_zero_flg = true;
		str_pnt++;;
	}
	*str_pnt = result.ones+ 0x30;	
	str_pnt++;
	*str_pnt = NULL_TERMINATOR;	
	
#elif 1
	

	for(hlooper = 0; hlooper < 6; hlooper++){
		bcd_pnt = (&result.byte_0 + (hlooper/2));
		
		if((hlooper % 2) == 0){	// we are with the low nibble...
			*temp_strpnt = (*bcd_pnt & 0X0F) + 0x30;;
		}else{	// we are with the high nibble
			*temp_strpnt = ((*bcd_pnt & 0XF0) >> 4) + 0x30;;
		}
		temp_strpnt--;

	
	}
	temp_strpnt++;
	hlooper = 0;
	while(hlooper < 6 && temp_strpnt != '\0'){
		hlooper++;
		if(not_zero_flg){
			*str_pnt = *temp_strpnt;
			str_pnt++;
			// temp_strpnt++;
		}else{
			if(*temp_strpnt != '0'){
				not_zero_flg = true;
				*str_pnt = *temp_strpnt;
				str_pnt++;
			}
			// temp_strpnt++;
		}
		temp_strpnt++;
		
	}
	if(hlooper == 0){
		*str_pnt == '0';
		str_pnt++;
		
	}
	*str_pnt = NULL_TERMINATOR;
	
	#endif
}

#elif 1

void int_to_str_converter(uint32_t the_value, char *str_pnt){

  uint8_t res[6];

  unsigned char hlooper = 0;
  
  uint8_t blooper = 0;

  uint32_t val = the_value;

		
  res[0] = 0;    
  res[1] = 0;    
  res[2] = 0;    
  res[3] = 0;    
  res[4] = 0;    
  res[5] = (uint8_t) ((val & 0xFF000000) >> 24);
  
  for(hlooper = 0; hlooper < 32; hlooper++)
  {
    for(blooper = 0; blooper < 5; blooper++)  // only till the last -1 becaseu we need to test against the value received the bit there
    {
      if((0x0F & res[blooper]) >= 0x05)
      {
        res[blooper] = res[blooper] + 0x03;		
      }
      if((0xF0 & res[blooper]) >= 0x50)
      {
        res[blooper] = res[blooper] + 0x30;
      }
      

      if((res[blooper + 1] & 0x80) != 0)  // testing for the next shifting left from the next lower byte...which is the higher byte in memory
      {
        res[blooper]++;
      }
      res[blooper] = res[blooper] << 1; // we left shifted everything..now the addinbg takes place
    }
    val = val << 1;
    res[5] = (uint8_t) ((val & 0xFF000000) >> 24);
  }
  
  for(hlooper = 0; hlooper < 5; hlooper++)
  {
    *str_pnt = (res[hlooper] >> 4) + 0x30;
    str_pnt++;
    *str_pnt = (res[hlooper] & 0x0F) + 0x30;
    str_pnt++;
  }
  
  *str_pnt =  = NULL_TERMINATOR;

}



#else
  

// 32 bit --> 4 294 967 295	therefore 10 chars --> for each cahr one nibble --> 5 bytes...or just staright out 10 bytes --> easiest....

// this is functional but only to 16bits --> 
// therefore i cant use it for the long int--> but otherwise it is fine
void int_to_str_converter(uint32_t the_value, char *str_pnt){
	
union {
	uint32_t bcd;	// uint24_t bcd;
	struct
	{
		uint8_t byte_0;
		uint8_t byte_1;
		uint8_t byte_2;
		uint8_t byte_3;
    uint8_t byte_4;
	};

	struct 
	{
		unsigned ones					: 4;
		unsigned tens					: 4;
		unsigned hundreds			: 4;
		unsigned thousands		: 4;
		unsigned t_thousands	: 4;
		unsigned h_thousands	: 4;
	};
}result;

unsigned char *bcd_pnt = &result.byte_0;

unsigned char hlooper = 0;
unsigned char hlooper_2 = 0;
unsigned char not_zero_flg = 0;

char temp_string[6];
char *temp_strpnt = &temp_string[5];

	*temp_strpnt = NULL_TERMINATOR;
	temp_strpnt--;
		
		
		
	result.bcd = 0;

	for(hlooper = 0; hlooper < 3; hlooper++)
	{
		result.bcd = result.bcd << 1;
		if(the_value >=32768)
		{
			result.bcd = result.bcd + 1;
		}
		the_value = (the_value << 1) & 0xFFFF;
	}

	for(hlooper = 0; hlooper < 13; hlooper++)
	{
	
		bcd_pnt = &result.byte_0;
		for(hlooper_2 = 0; hlooper_2 < 3; hlooper_2++)
		{

			if((0x0F & *bcd_pnt) >= 5)
			{
				*bcd_pnt = *bcd_pnt + 3;		
			}
			if((0xF0 & *bcd_pnt) >= 80)
			{
				*bcd_pnt = *bcd_pnt + 48;
			}
			bcd_pnt++;
		}
		
		result.bcd = result.bcd << 1;
		if(the_value >=32768)
		{
			result.bcd = result.bcd + 1;
		}
		the_value = (the_value << 1) & 0xFFFF;;
				
	}
	
	#if 1	// because in the end was this the fastes...
	*str_pnt = result.h_thousands + 0x30;
	if(*str_pnt != 0x30){
		not_zero_flg = true;
		str_pnt++;
	}
	
	*str_pnt = result.t_thousands+ 0x30;
	if((*str_pnt != 0x30) || not_zero_flg){
		not_zero_flg = true;
		str_pnt++;;
	}
	
	*str_pnt = result.thousands+ 0x30;
	if((*str_pnt != 0x30) || not_zero_flg){
		not_zero_flg = true;
		str_pnt++;;
	}

	*str_pnt = result.hundreds+ 0x30;	
	if((*str_pnt != 0x30) || not_zero_flg){
		not_zero_flg = true;
		str_pnt++;;
	}

	*str_pnt = result.tens+ 0x30;	
	if((*str_pnt != 0x30) || not_zero_flg){
		not_zero_flg = true;
		str_pnt++;;
	}
	*str_pnt = result.ones+ 0x30;	
	str_pnt++;
	*str_pnt = NULL_TERMINATOR;	
	
#elif 1
	

	for(hlooper = 0; hlooper < 6; hlooper++){
		bcd_pnt = (&result.byte_0 + (hlooper/2));
		
		if((hlooper % 2) == 0){	// we are with the low nibble...
			*temp_strpnt = (*bcd_pnt & 0X0F) + 0x30;;
		}else{	// we are with the high nibble
			*temp_strpnt = ((*bcd_pnt & 0XF0) >> 4) + 0x30;;
		}
		temp_strpnt--;

	
	}
	temp_strpnt++;
	hlooper = 0;
	while(hlooper < 6 && temp_strpnt != '\0'){
		hlooper++;
		if(not_zero_flg){
			*str_pnt = *temp_strpnt;
			str_pnt++;
			// temp_strpnt++;
		}else{
			if(*temp_strpnt != '0'){
				not_zero_flg = true;
				*str_pnt = *temp_strpnt;
				str_pnt++;
			}
			// temp_strpnt++;
		}
		temp_strpnt++;
		
	}
	if(hlooper == 0){
		*str_pnt == '0';
		str_pnt++;
		
	}
	*str_pnt = NULL_TERMINATOR;
	
	#endif
}





#endif