#ifndef DDS_H
#define	DDS_H


#include "Global.h"




#if 1

void Transmite(bool TransmiteRadiogonio);

void AD9954Configura(void);

void AD9954TransmiteMensaje(void);

void AD9954LimpiaBufferTransmision(void);

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
0b00011011, // ,
0b01100011, // ?
0b00011111, // .
0b01011111, // /
0b00110111, // 
0b01110111, // 
0b01100111, // 
0b01000011, // 
0b00101011, // 
0b00000111, // 
0b01010111, // 
0b01110011, // 
0b00110011, // 
0b00001111, // 
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


#else

/**
 * @brief Función que ajusta las velocidades de transmisión
 */
void AD9954AjustaVelocidadDeTransmision( int Bps );

/**
 * @brief Función que escribe las palabras de configuración al AD9954
 */


#if (defined AD9954_TRANSMITE_NO_BAUDOT) && (AD9954_TRANSMITE_NO_BAUDOT == 1)
void AD9954TransmiteStringAscii(uint16_t NumDatos, uint8_t *DatosAEnviar);
#endif

/**
 * @brief Función que transmite un único dato. Se espera un dato BAUDOT
 */
// void AD9954TransmiteByte( uint8_t ByteBaudot );

/**
 * @brief Función que transmite una cadena de caracteres ASCII en BAUDOT
 */
void inline AD9954TransmiteString( uint8_t NumDatos, uint8_t *CadenaAscii );



/**
 * @brief Función que enciende el sintetizador para comenzar la transmisión
 */
void AD9954Enciende ( void );

/**
 * @brief Función que apaga el sintetizador
 */
void AD9954Apaga ( void );

/**
 * @brief Genera un pulso en la línea I/O Update
 */
void AD9954PulsoUpdate ( void );

/**
 * @brief Genera un pulso en la línea I/O Sync
 */
void AD9954PulsoIoSync ( void );

/**
 * @brief Selecciona el banco 3 del AD9954
 */
void AD9954SelectorBanco3 ( void );

/**
 * @brief Selecciona el banco 2 del AD9954
 */
void AD9954SelectorBanco2 ( void );

/**
 * @brief Selecciona el banco 1 del AD9954
 */
void AD9954SelectorBanco1 ( void );

/**
 * @brief Selecciona el banco 0 del AD9954
 */
void AD9954SelectorBanco0 ( void );

/**
 * @brief Función que transmite por SPI al AD9954
 */
void SpiTransmite( uint8_t Dato );

/**
 * @brief Definiciones de las conexiones entre el PIC y el AD9954
 */
 
 
 
 // 
#if PIC_16F1936

#define RESET_AD9954 LATCbits.LATC2
#define VDD_AD9954 LATAbits.LATA4
#define IOSYNC_AD9954 LATAbits.LATA0
#define SDIO_AD9954 LATAbits.LATA1
#define SCLK_AD9954 LATAbits.LATA2
#define IO_UPDATE_AD9954 LATBbits.LATB4
#define PS0_AD9954 LATBbits.LATB2
#define PS1_AD9954 LATBbits.LATB3


#define T2_POSTSCALER	T2CONbits.T2OUTPS
#define T2_PRESCALER T2CONbits.T2CKPS

#else
 
#define RESET_AD9954 RC2
#define VDD_AD9954 RA4
#define IOSYNC_AD9954 RA0
#define SDIO_AD9954 RA1
#define SCLK_AD9954 RA2
#define IO_UPDATE_AD9954 RB4
#define PS0_AD9954 RB2
#define PS1_AD9954 RB3


#define T2_POSTSCALER	T2CONbits.TOUTPS
#define T2_PRESCALER T2CONbits.T2CKPS

#endif


#define AD9954_TRANSMITE_CARACTER_ASCII(X) \
AD9954TransmiteByte((uint8_t)(gTablaAsciiABaudot[ (int)X ]))

/**
 * @brief Unión de estructuras que permite un acceso a los bits por separado
 * 
 * Se ha utilizado esta unión de estructuras para conseguir acceder a los bits
 * independientes de un uint8_t. Se ha realizado esta acción porque el
 * compilador tarda una cantidad de tiempo progresiva en seleccionar un bit
 * según esté éste de lejos respecto del origen.
 */
typedef union
{
    uint8_t EquivalentByte;
    struct
    {
        unsigned b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
    };
} t_byte;

/**
 * @brief Indica si está transmitiendo a 300 bps o a 150
 */
extern bool gTrueSi150FalseSi300;

/**
 * @brief Tabla de conversión de ASCII a su equivalente BAUDOT
 * 
 * @warning Se ha utilizado dos caracteres no usados en BAUDOT para la
 * separación de la tabla de figuras de la tabla de letras. No interfiere en el
 * mensaje recibido, únicamente en el mensaje enviado a nivel interno
 * 
 */
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
0b00011011, // ,
0b01100011, // ?
0b00011111, // .
0b01011111, // /
0b00110111, // 
0b01110111, // 
0b01100111, // 
0b01000011, // 
0b00101011, // 
0b00000111, // 
0b01010111, // 
0b01110011, // 
0b00110011, // 
0b00001111, // 
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



#ifdef TABLA_BAUDOT
/**
 * @brief Tabla de caracteres BAUDOT válidos para la transmisión
 */
const uint8_t Baudot[] = {
0b00000011, 
0b00001011, 
0b00100011, 
0b00010011, 
0b01110111, 
0b01100111, 
0b01000011, 
0b00101011, 
0b00000111, 
0b01010111, 
0b01110011, 
0b00110011, 
0b00001111, 
0b00110111, 
0b01100011, 
0b01010011, 
0b01001011, 
0b01011011, 
0b00101111, 
0b00010111, 
0b01101011, 
0b01111011, 
0b00100111, 
0b01000111, 
0b01011111, 
0b00111011, 
0b00111111, 
0b01001111, 
0b00011011, 
0b00011111, 
0b01101111, 
0b01111111
};
#endif

#endif

#endif	// DDS_H

