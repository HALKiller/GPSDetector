#ifndef DDS_H
#define	DDS_H


#include "Global.h"


#if COMPILE_FOR_INTERNAL_TEST  
 
 uint16_t get_txcnt(void);
 
#endif  



void Transmite(bool TransmiteRadiogonio);

void AD9954Configura(void);

void AD9954TransmiteMensaje(void);

void DDS_flush_buffer(void);

extern uint8_t FTW0[4];
extern uint8_t FTW1[4];
extern uint8_t FTW2[4];
extern uint8_t FTW3[4];

#ifndef BUFFER_TRAMA_CAPTACION_GPS
#define BUFFER_TRAMA_CAPTACION_GPS
/* El define de BUFFER_TRAMA_CAPTACION_GPS hace que estas variables puedan ser
 * definidas si no está el archivo GPSParser.h, en un proyecto que sí tenga
 * el sintetizador AD9954 para la transmisión de datos. Variable reciclada
 */

typedef union
{
    uint8_t EquivalentByte;
    struct
    {
        unsigned b0:1,b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
    };
} t_byte;


#define ELMS_TRAMA 79

typedef struct
{
  uint8_t *BuferDeEntrada;  //    [ELMS_TRAMA];  /**< El mensaje que se va a enviar
                                           // * se guarda aquí antes de su envío */
  uint8_t PosicionDelBufer;  /**< Para guardar el valor de
                                           * strlen antes de su envío */
  bool ProcesaLaTrama;  /**< No se utiliza esta variable
                                           * cuando se recicla */
} sEntradaDeTrama;

sEntradaDeTrama gEntradaDeTrama; 

#endif //* BUFFER_TRAMA_CAPTACION_GPS */

#if 1

const uint8_t gTablaAsciiABaudot[] = {
0b00000011, // Null
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b01010011, // Bell
0b00000000, // 
0b00000000, // 
0b00100011, // Line feed
0b00000000, // 
0b00000000, // 
0b00001011, // Carriage return
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00000000, // 
0b00010011, // Space
0b01011011, // !
0b01000111, // "
0b00010111, // #
0b01001011, // $
0b00000000, // 
0b00101111, // &
0b01101011, // '
0b01111011, // (
0b00100111, // )
0b00000000, // 
0b00000000, // 
0b00011011, // ,  // this one here is strangely enough actually working...
0b01100011, // -
0b00011111, // .
0b01011111, // /
0b00110111, // 0
0b01110111, // 1
0b01100111, // 2
0b01000011, // 3
0b00101011, // 4
0b00000111, // 5
0b01010111, // 6
0b01110011, // 7
0b00110011, // 8
0b00001111, // 9
0b00111011, // :
0b00111111, // ;
0b01111111, // < /**< Cuando se hace un 'Shift to letters' */
0b00000000, // 
0b01101111, // > /**< Cuando se hace un 'Shift to figures' */
0b01001111, // ?
0b00000000, // 
0b01100011, // A
0b01001111, // B
0b00111011, // C
0b01001011, // D
0b01000011, // E
0b01011011, // F
0b00101111, // G
0b00010111, // H
0b00110011, // I
0b01101011, // J
0b01111011, // K
0b00100111, // L
0b00011111, // M
0b00011011, // N
0b00001111, // O
0b00110111, // P
0b01110111, // Q
0b00101011, // R
0b01010011, // S
0b00000111, // T
0b01110011, // U
0b00111111, // V
0b01100111, // W
0b01011111, // X
0b01010111, // Y
0b01000111, // Z
0b00000000  // 92 líneas en total. :TODO: esta última se puede eliminar
};


#else
	
const uint8_t gTablaAsciiABaudot[] = {
0b11111100, // Null
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b10101100, // Bell
0b11111111, // 
0b11111111, // 
0b11011100, // Line feed
0b11111111, // 
0b11111111, // 
0b11110100, // Carriage return
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11111111, // 
0b11101100, // Space
0b10100100, // !
0b10111000, // "
0b11101000, // #
0b10110100, // $
0b11111111, // 
0b11010000, // &
0b10010100, // '
0b10000100, // (
0b11011000, // )
0b11111111, // 
0b11111111, // 
0b11100100, // ,
0b10011100, // ?
0b11100000, // .
0b10100000, // /
0b11001000, // 
0b10001000, // 
0b10011000, // 
0b10111100, // 
0b11010100, // 
0b11111000, // 
0b10101000, // 
0b10001100, // 
0b11001100, // 
0b11110000, // 
0b11000100, // :
0b11000000, // ;
0b10000000, // < /**< Cuando se hace un 'Shift to letters' */
0b11111111, // 
0b10010000, // > /**< Cuando se hace un 'Shift to figures' */
0b10110000, // ?
0b11111111, // 
0b10011100, // A
0b10110000, // B
0b11000100, // C
0b10110100, // D
0b10111100, // E
0b10100100, // F
0b11010000, // G
0b11101000, // H
0b11001100, // I
0b10010100, // J
0b10000100, // K
0b11011000, // L
0b11100000, // M
0b11100100, // N
0b11110000, // O
0b11001000, // P
0b10001000, // Q
0b11010100, // R
0b10101100, // S
0b11111000, // T
0b10001100, // U
0b11000000, // V
0b10011000, // W
0b10100000, // X
0b10101000, // Y
0b10111000, // Z
0b11111111  // 92 líneas en total. :TODO: esta última se puede eliminar
};

#endif



#endif	// DDS_H

