// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com



#include "eeprom.h"

#include "Global.h"


#include <xc.h>            /* XC8 General Include File */

// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  
#ifndef FILE_EEPROM_DB_ENABLED
#define FILE_EEPROM_DB_ENABLED 0
#endif
#if FILE_EEPROM_DB_ENABLED
#define DB_PRINT(str) G_DB_PRINT(str)
#define UART_int(var) G_UART_INT(var)
#else
#define DB_PRINT(str)
#define UART_int(var)
#endif
// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  


#define UNUSED 0xFF

// BATCNT
#define BATC_L 0x00 // 0x83 // BATCNT_LOW
#define BATC_H 0x00 // 0x2F // BATCNT_HIGH
#define BATC_F 0x00 // BATCNT_FLG


#if 1

#define FTW0_1 0x15
#define FTW0_2 0x70
#define FTW0_3 0xE0
#define FTW0_4 0x22

#define FTW1_1 0x15
#define FTW1_2 0x70
#define FTW1_3 0xFA
#define FTW1_4 0x59

#define FTW2_1 0x15
#define FTW2_2 0x70
#define FTW2_3 0xE0
#define FTW2_4 0x22

#define FTW3_1 0x15
#define FTW3_2 0x70
#define FTW3_3 0xFA
#define FTW3_4 0x58

#else


#define FTW0_1 0x15
#define FTW0_2 0x39
#define FTW0_3 0x2B
#define FTW0_4 0x7F

#define FTW1_1 0x15
#define FTW1_2 0x39
#define FTW1_3 0x45
#define FTW1_4 0xB6

#define FTW2_1 0x15
#define FTW2_2 0x39
#define FTW2_3 0x2B
#define FTW2_4 0x7F

#define FTW3_1 0x15
#define FTW3_2 0x39
#define FTW3_3 0x45
#define FTW3_4 0xB6

#endif
// HEADER
#define HEAD_0 'D'
#define HEAD_1 'C'
#define HEAD_2 'T'
#define HEAD_3 'C'

// CLIENT
#define CLID_0 '0'
#define CLID_1 '0'
#define CLID_2 '1'
#define CLID_3 '1'

// BALIZA
#define BLID_0 '0'
#define BLID_1 '1'
#define BLID_2 '9'

// DETECTOR CONFIG
#define TX_DURATION         0x06
#define TIEMPO_SINCRONISMO  0x01
#define PWM_PORCENTAGE      0x01
#define PWM_LUZ_ON_OFF      0x28  // 0x1A


#if 1

// Define bit settings with meaningful names
#define NOT_DOBLEPERIOD      0  // when set --> does not double the period even on low bat
#define TX_CONTINUO_ENABLED  0  // when set --> transmits in circles all the time
#define ALWAYS_1_B2          1  // unused
#define ALWAYS_0_B3          0  // unused
#define BPS_150_300_MODE     1  // when set --> tx = 150BPS
#define LUCES_NOT_ACTIVE     0  // when set --> we do not iluminate with LED
#define ALWAYS_0_B6          0  // unused
#define ALWAYS_1_B7          1  // unused

// Construct BPS_CONFIG dynamically
#define BPS_CONFIG  ((NOT_DOBLEPERIOD << 0) | (TX_CONTINUO_ENABLED << 1) | \
                     (ALWAYS_1_B2 << 2) | (ALWAYS_0_B3 << 3) | \
                     (BPS_150_300_MODE << 4) | (LUCES_NOT_ACTIVE << 5) | \
                     (ALWAYS_0_B6 << 6) | (ALWAYS_1_B7 << 7))

#else

#define BPS_CONFIG          0x90  // 0x92 = "pitar in circle" mode" //  0x90  // 0x90 = "normal" mode

#endif

#define MAX_DETECTORES      0x14 // 0x3C  // 0x14  // 0x0C
#define BATERIE_LEVEL       0xCF
#define BATNIV              0xCF  // CF = 10.8

#if 1

__EEPROM_DATA(BATC_L, BATC_H, BATC_F, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);

__EEPROM_DATA(UNUSED, UNUSED, FTW0_1, FTW0_2, FTW0_3, FTW0_4, UNUSED, FTW1_1);
__EEPROM_DATA(FTW1_2, FTW1_3, FTW1_4, UNUSED, FTW2_1, FTW2_2, FTW2_3, FTW2_4);
__EEPROM_DATA(UNUSED, FTW3_1, FTW3_2, FTW3_3, FTW3_4, UNUSED, HEAD_0, HEAD_1);
__EEPROM_DATA(HEAD_2, HEAD_3, CLID_0, CLID_1, CLID_2, CLID_3, BLID_0, BLID_1);

__EEPROM_DATA(BLID_2, UNUSED ,TX_DURATION,  TIEMPO_SINCRONISMO, PWM_PORCENTAGE, PWM_LUZ_ON_OFF, BPS_CONFIG, MAX_DETECTORES);
__EEPROM_DATA(BATNIV, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);

__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);

__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);

__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);

__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);

__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
__EEPROM_DATA(UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);



#else

__eeprom unsigned char gContenidoEeprom[] = {

  BATC_L, BATC_H, BATC_F, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  
  UNUSED, UNUSED, FTW0_1, FTW0_2, FTW0_3, FTW0_4, UNUSED, FTW1_0,
  FTW1_2, FTW1_3, FTW1_4, UNUSED, FTW2_0, FTW2_2, FTW2_3, FTW2_4,
  UNUSED, FTW3_0, FTW3_2, FTW3_3, FTW3_4, UNUSED, HEAD_0, HEAD_1,
  HEAD_2, HEAD_3, CLID_0, CLID_1, CLID_2, CLID_3, BLID_0, BLID_1,
  BLID_2, UNUSED ,TX_DURATION,  TIEMPO_SINCRONISMO, PWM_PORCENTAGE, PWM_LUZ_ON_OFF, BPS_CONFIG, MAX_DETECTORES,
  BATNIV, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,

};

#endif







#if 1
void write_eeprom(uint8_t the_address, uint8_t data){
  uint8_t intconold;

	EEADR = the_address;
	
	EEDATA = data;
	EECON1bits.EEPGD = 0; //data memory
	EECON1bits.WREN = 1; //enables write
	intconold = INTCONbits.GIE; 
	INTCONbits.GIE = 0; //Deshabilita interrupciones para el trozo de codigo siguiente según datasheet.
#if 0

asm("movlw 0x55");
asm("movwf EECON2");
asm("movlw 0xAA");
asm("movwf EECON2");
asm("bsf EECON1, 1");

#else
  
	EECON2 = 0X55;
	EECON2 = 0X0AA;
	EECON1bits.WR = 1; // start write
  
#endif		

	INTCONbits.GIE = intconold;

	while (EECON1bits.WR != 0);
	EECON1bits.WREN = 0;
	
}
#endif


#if 1

uint8_t LeerEeprom(uint8_t direccion)
{
  EEADR = direccion;
  EEPGD = 0;
  RD = 1;
  return (uint8_t) EEDAT;
}

#endif




// If the new one works this is obsolete
#if 0

#if 0
__eeprom unsigned char gContenidoEeprom[] = {
  /* Palabras de configuración del sintetizador que ya no se utilizan */
  0x00, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x24, 0x07, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x08, 0x00, 0x00, 0x01, 0x04, 0x00, 0x09, 0x00, 0x00, 0x02, 0x08,
  0x00, 0x0A, 0x00, 0x00, 0x03, 0x0C, 0x00,

  /* Frecuencia.  Normal = 26.666 USB; Radiogonio = 26.620 LSB */
  /* Frecuencias del sintetizador. */
  0x0B, /* No se usa */ 0x15, 0x5D, 0x7D, 0xBF, /* FTW0 - Normal 0     */
  0x0B, /* No se usa */ 0x15, 0x5D, 0x63, 0x88, /* FTW1 - Normal 1     */
  0x0B, /* No se usa */ 0x15, 0x5D, 0x7D, 0xBF, /* FTW2 - Radiogonio 0 */
  0x0B, /* No se usa */ 0x15, 0x5D, 0x63, 0x88, /* FTW3 - Radiogonio 1 */
  
  0xFF, /* No se usa */
  
  'D', 'C', 'T', 'C', /* Cabecera: DCTC  ¡                                    */
  '9', '9', '9', '8', /* Número de cliente, gIdentificadorCliente             */
  '0', '0', '3',      /* Número de baliza,  gNumeroDeBaliza                   */
  
  0x00, /* T_ERROR_GPS. No se usa                                             */
  EEPROM_TX_DURATION, /* gTiempoDuracionTransmision: tiempo de transmisión  0A=10sg         */
  TIEMPO_SINCRONISMO, /* gSegundosSincronismo:                                              */
        /* Número de segundos que dura la transmisión de sincronismo          */
  PWM_PORCENTAGE, /* gPorcentajePwm: Porcentaje de PWM de la ráfaga de luz / 10         */
  PWM_LUZ_ON_OFF, /* Nibble superior: gSegundosLuzEnOn.                                 */
        /* número de segundos con luz nocturna encendida.                     */
        /* Nibble inferior: gSegundosLuzEnOff.                                */
        /* número de segundos con luz nocturna apagada.                       */
  EEPROM_BPS_CONFIG, /* Tipo de configuración de la baliza   90=150bps   80=300bps         */
  EEPROM_MAX_DETECTORES, /* gTotalBalizas: Número de balizas en total 3C=60 2A=42  32=50  24=36  1E=30   18=24*/
  0xCF, /* Valor de batería para transmisión cada 2 ciclos  CF=10.8  D7=11.2  */
  0xFC, /* Segunda palabra de configuración de la baliza                      */
  0x02, /* Número de satélites a la vista antes de apagar el GPS (no usado)   */
  
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, /* No se usa */
  
  /* Esta sección entera ya no se utiliza */
  0x24, 0x50, 0x4D, 0x54, 0x4B, 0x33, 0x31, 0x34, 0x2C, 0x30, 0x2C, 0x31, 0x2C,
  0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30,
  0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C,
  0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2A, 0x32, 0x39, 0x0D, 0x0A, 0x24,
  0x50, 0x4D, 0x54, 0x4B, 0x32, 0x35, 0x31, 0x2C, 0x34, 0x38, 0x30, 0x30, 0x2A,
  0x31, 0x34, 0x0D, 0x0A, 0x24, 0x50, 0x4D, 0x54, 0x4B, 0x33, 0x32, 0x34, 0x2C,
  0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x36, 0x39, 0x2A, 0x31, 0x34,
  0x0D, 0x0A, 0x7F, 0x13, 0x53, 0x3B, 0x6F, 0x37, 0x37, 0x67, 0x7F, 0x13, 0x13,
  0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x7F, 0x13, 0x1B, 0x1F, 0x13,
  0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x7F, 0x13, 0x3B, 0x1F, 0x6F, 0x37, 0x37,
  0x77, 0x7F, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13,
  
/*'<'   ' '   'A'   'C'   'T'   'I'   'V'   'A'   'N'   'D'   'O'   'S'   'E'   ' '  */
  0x7F, 0x13, 0x63, 0x3B, 0x07, 0x33, 0x3F, 0x63, 0x1B, 0x4B, 0x0F, 0x53, 0x43, 0x13,
/*'V'   '>'   '8'   '2'   '4'   '4'   '<'  */
  0x3F, 0x6F, 0x33, 0x67, 0x2B, 0x2B, 0x7F,

/*'<'   ' '   'G'   'O'   '>'   '0'   '0'   '3'   '<' */
  0x7F, 0x13, 0x2F, 0x0F, 0x6F, 0x37, 0x37, 0x43, 0x7F  /* No se usa */
  
};


#elif 1

__eeprom unsigned char gContenidoEeprom[] = {
  /* Palabras de configuración del sintetizador que ya no se utilizan */
  0x00, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x24, 0x07, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x08, 0x00, 0x00, 0x01, 0x04, 0x00, 0x09, 0x00, 0x00, 0x02, 0x08,
  0x00, 0x0A, 0x00, 0x00, 0x03, 0x0C, 0x00,

    /* Frecuencia.  Normal = 26.666 USB; Radiogonio = 26.620 LSB */
  /* Frecuencias del sintetizador. */
  0x0B, /* No se usa */ 0x15, 0x39, 0x2B, 0x7F, /* FTW0 - Normal 0     */
  0x0B, /* No se usa */ 0x15, 0x39, 0x45, 0xB6, /* FTW1 - Normal 1     */
  0x0B, /* No se usa */ 0x15, 0x39, 0x2B, 0x7F, /* FTW2 - Radiogonio 0 */
  0x0B, /* No se usa */ 0x15, 0x39, 0x45, 0xB6, /* FTW3 - Radiogonio 1 */
  // 0x0B, /* No se usa */ 0x15, 0x39, 0x45, 0xB6, /* FTW3 - Radiogonio 1 */
  
  0xFF, /* No se usa */
  
  'D', 'C', 'T', 'C', /* Cabecera: DCTC  ¡                                    */
  '9', '9', '9', '8', /* Número de cliente, gIdentificadorCliente             */
  '0', '0', '2',      /* Número de baliza,  gNumeroDeBaliza                   */
  
  0x00, /* T_ERROR_GPS. No se usa                                             */
  0x0A, /* gTiempoDuracionTransmision: tiempo de transmisión  0A=10sg         */
  0x01, /* gSegundosSincronismo:                                              */
        /* Número de segundos que dura la transmisión de sincronismo          */
  0x05, /* gPorcentajePwm: Porcentaje de PWM de la ráfaga de luz / 10         */
  0x28, /* Nibble superior: gSegundosLuzEnOn.                                 */
        /* número de segundos con luz nocturna encendida.                     */
        /* Nibble inferior: gSegundosLuzEnOff.                                */
        /* número de segundos con luz nocturna apagada.                       */
  0x92, // 0x90, if b0==false --> Double Period on low bat, if b1==true-->always_transmit, if b4==true TX_150 BPS, if b5==false-->LUZ_ENABLED 
  0x0C, /* gTotalBalizas: Número de balizas en total 3C=60 2A=42  32=50  24=36  1E=30   18=24*/
  0xCF, /* Valor de batería para transmisión cada 2 ciclos  CF=10.8  D7=11.2  */
  0xFC, /* Segunda palabra de configuración de la baliza                      */
  0x02, /* Número de satélites a la vista antes de apagar el GPS (no usado)   */
  
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, /* No se usa */
  
  /* Esta sección entera ya no se utiliza */
  0x24, 0x50, 0x4D, 0x54, 0x4B, 0x33, 0x31, 0x34, 0x2C, 0x30, 0x2C, 0x31, 0x2C,
  0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30,
  0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C,
  0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2A, 0x32, 0x39, 0x0D, 0x0A, 0x24,
  0x50, 0x4D, 0x54, 0x4B, 0x32, 0x35, 0x31, 0x2C, 0x34, 0x38, 0x30, 0x30, 0x2A,
  0x31, 0x34, 0x0D, 0x0A, 0x24, 0x50, 0x4D, 0x54, 0x4B, 0x33, 0x32, 0x34, 0x2C,
  0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x36, 0x39, 0x2A, 0x31, 0x34,
  0x0D, 0x0A, 0x7F, 0x13, 0x53, 0x3B, 0x6F, 0x37, 0x37, 0x67, 0x7F, 0x13, 0x13,
  0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x7F, 0x13, 0x1B, 0x1F, 0x13,
  0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x7F, 0x13, 0x3B, 0x1F, 0x6F, 0x37, 0x37,
  0x77, 0x7F, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13,
  
/*'<'   ' '   'A'   'C'   'T'   'I'   'V'   'A'   'N'   'D'   'O'   'S'   'E'   ' '  */
  0x7F, 0x13, 0x63, 0x3B, 0x07, 0x33, 0x3F, 0x63, 0x1B, 0x4B, 0x0F, 0x53, 0x43, 0x13,
/*'V'   '>'   '8'   '2'   '4'   '4'   '<'  */
  0x3F, 0x6F, 0x33, 0x67, 0x2B, 0x2B, 0x7F,

/*'<'   ' '   'G'   'O'   '>'   '0'   '0'   '3'   '<' */
  0x7F, 0x13, 0x2F, 0x0F, 0x6F, 0x37, 0x37, 0x43, 0x7F  /* No se usa */
  
};


#else

__EEPROM_DATA(0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08);
__EEPROM_DATA(0x00, 0x00, 0x01, 0x04, 0x00, 0x09, 0x00, 0x00);
__EEPROM_DATA(0x00, 0x0B, 0x15, 0x3F, 0x48, 0x80, 0x0B, 0x15);
__EEPROM_DATA(0x0B, 0x15, 0x4D, 0x4F, 0xFD, 0xFF, 0x44, 0x45);

__EEPROM_DATA(0x31, 0x00, 0x0A, 0x01, 0x02, 0x18, 0x84, 0x1E);
__EEPROM_DATA(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x24, 0x50, 0x4D);
__EEPROM_DATA(0x31, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C);
__EEPROM_DATA(0x30, 0x2C, 0x30, 0x2C, 'D', 0x2C, 0x30, 0x2C);  // 61 = 'D'

__EEPROM_DATA(0x30, 0x2C, 0x30, 0x2A, 0x32, 0x39, 0x0D, 0x0A);
__EEPROM_DATA(0x2C, 0x34, 0x38, 0x30, 0x30, 0x2A, 0x31, 0x34);
__EEPROM_DATA(0x32, 0x34, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30);
__EEPROM_DATA(0x0D, 0x0A, 0x7F, 0x13, 0x53, 0x3B, 0x6F, 0x37);

__EEPROM_DATA(0x13, 0x13, 0x13, 0x13, 0x13, 0x7F, 0x13, 0x1B);
__EEPROM_DATA(0x7F, 0x13, 0x3B, 0x1F, 0x6F, 0x37, 0x37, 0x77);
__EEPROM_DATA(0x13, 0x13, 0x7F, 0x13, 0x63, 0x3B, 0x07, 0x33);
__EEPROM_DATA(0x3F, 0x6F, 0x33, 0x67, 0x2B, 0x2B, 0x7F, 0x7F);


__EEPROM_DATA(0x24, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08);
__EEPROM_DATA(0x02, 0x08, 0x00, 0x0A, 0x00, 0x00, 0x03, 0x0C);
__EEPROM_DATA(0x3F, 0x62, 0xB7, 0x0B, 0x15, 0x4D, 0x6A, 0x16);
__EEPROM_DATA(0x54, 0x43, 0x30, 0x30, 0x31, 0x32, 0x30, 0x30);

__EEPROM_DATA(0xFF, 0xFC, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
__EEPROM_DATA(0x54, 0x4B, 0x33, 0x31, 0x34, 0x2C, 0x30, 0x2C);
__EEPROM_DATA(0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C);
__EEPROM_DATA(0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C);

__EEPROM_DATA(0x24, 0x50, 0x4D, 0x54, 0x4B, 0x32, 0x35, 0x31);
__EEPROM_DATA(0x0D, 0x0A, 0x24, 0x50, 0x4D, 0x54, 0x4B, 0x33);
__EEPROM_DATA(0x2C, 0x30, 0x2C, 0x36, 0x39, 0x2A, 0x31, 0x34);
__EEPROM_DATA(0x37, 0x67, 0x7F, 0x13, 0x13, 0x13, 0x13, 0x13);

__EEPROM_DATA(0x1F, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13);
__EEPROM_DATA(0x7F, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13);
__EEPROM_DATA(0x3F, 0x63, 0x1B, 0x4B, 0x0F, 0x53, 0x43, 0x13);
__EEPROM_DATA(0x13, 0x2F, 0x0F, 0x6F, 0x37, 0x37, 0x43, 0x7F);



#endif


#endif