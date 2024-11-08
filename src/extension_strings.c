// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com



#include "extension_strings.h"

#define NOT_IN_USE 0


uint8_t AToUint8_t(uint8_t  * s, uint8_t ndigits)
{
  uint8_t a, b, c;

  // 0x0022 bytes
  for ( a = 0, b = 0, c = 0; ( c < 0xA ) && ( b < ndigits ); b++)
  {
    c = s[b] - 0x30;
    a = (uint8_t) (a * 10 + c);
  }

  return a;
}

#if 1

uint8_t * Uint8_tStrchr(uint8_t  *ptr, uint8_t chr){

  while (*ptr != 0) 
  {
      if (*ptr == chr) 
      {
        return ptr;
      }
      ptr++;
  }
  
  return NULL;
  
}

#else
  
//we are searching for a certain char here it seems...
uint8_t * Uint8_tStrchr(uint8_t  * ptr, uint8_t chr)
{
  do
  {
    if ( ptr[0] == chr )
    {
      return ptr;
    }
  } while ( ptr++[0] );
  return 0;
}

#endif



void DecimalUint8ToA(uint8_t * buf, uint8_t val, uint8_t npos, bool null_to_end)
{
  uint8_t c;

  if (
      ( ( val >= 100 ) && ( npos >= 3 ) ) ||
      ( ( val < 100 ) && ( val >= 10 ) && ( npos >= 2 ) ) ||
      ( ( val < 10 ) && ( npos >= 1 ) )
     )
  {
    if( null_to_end == true ) {buf[npos] = 0;}
    while ( npos > 1 )
    {
      npos--;
      c = val % 10;
      val /= 10;
      c += '0';
      buf[npos] = c;
    }
    c = val % 10;
    c += '0';
    buf[0] = c;
  }
}



void DecimalUint16ToA(uint8_t * buf, uint16_t val, uint8_t npos, bool null_to_end)
{
  if (
      ( ( val >= 0x2710 ) && ( npos >= 5 ) ) ||
      ( ( val < 0x2710 ) && ( npos >= 4 ) )
     )
  {
    DecimalUint8ToA( buf, (uint8_t) ( val / 0x0064 ), npos - 2, null_to_end );
    DecimalUint8ToA( buf + npos - 2, (uint8_t) ( val % 0x0064 ), 2, null_to_end );
  }
}



#if NOT_IN_USE
// seems unused
float Uint8_tToF(uint8_t  * str)
{
  if ( (str == 0) || (str[0] == 0) )
  {
    return 0;
  }
  float integerPart = 0;
  float fractionPart = 0;
  uint16_t divisorForFraction = 1;
  float sign = 1.0;
  bool inFraction = false;
  uint8_t c;
  /*Take care of +/- sign*/
  if ( str[0] == '-' )
  {
    str++;
    sign = -1.0;
  }
  else if ( str[0] == '+' )
  {
    str++;
  }
  while ( str[0] != '\0')
  {
    c = str[0];
    c -= 0x30;
    if ( c < 10 )
    {
      if ( inFraction == true )
      {
        // Se visualiza cómo se convierte de un caracter a un entero
        fractionPart = fractionPart * 10.0 + ( float ) c;
        divisorForFraction *= 10;
      }
      else
      {
        integerPart = integerPart * 10.0 + ( float ) c;
      }
    }
    else if ( str[0] == '.' )
    {
      if ( inFraction == true )
      {
        return sign * (integerPart + fractionPart / divisorForFraction);
      }
      else
      {
        inFraction = true;
      }
    }
    else
    {
      return sign * (integerPart + fractionPart / divisorForFraction);
    }
    str++;
  }
  return sign * (integerPart + fractionPart/divisorForFraction);
}


// seems unused
void DecimalInt8ToA(uint8_t * buf, int8_t val, uint8_t npos, bool null_to_end)
{
  if ( ( val < 0 ) && ( npos >= 1 ) )
  {
    buf[0] = '-';
    buf++;
    val = -val;
  }
  DecimalUint8ToA(buf, (uint8_t) val, npos--, null_to_end);
}

#endif