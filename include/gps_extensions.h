#ifndef GPS_EXTENSIONS_H
#define GPS_EXTENSIONS_H

#include <stdint.h>
#include "stddef.h"
#include "stdbool.h"
#include "Global.h"



#if USE_REDUCED_RAM
#define MAX_SENTENCE_LENGTH	15
#else
#define MAX_SENTENCE_LENGTH	82
#endif

// #define MAX_SENTENCE_LENGTH	82

enum eEstadoAntena
{
  NO_TRAMA = 99, /**< No hay trama */
  OPEN = 0,      /**< No hay antena */
  NORMAL = 1,    /**< La antena está conectada correctamente */
  SHORT = 2      /**< La antena está en cortocircuito */
};

/**
 * @brief Clasificador interno del campo 3 de la trama GGA: Dirección latitud
 * 
 */
enum eLatitude
{
  N = 0, /**< Norte */
  S      /**< Sur */
};

/**
 * @brief Clasificador interno del campo 5 de la trama GGA: Dirección longitud
 * 
 */
enum eLongitude
{
  E = 0, /**< Este */
  W      /**< Oeste */
};

/**
 * @brief Clasificador interno del campo 6 de la trama GGA: Calidad GPS
 * 
 * @warning Se han incluido otros términos del standard que corresponden a la
 * versión 3.01 del estándar. Aún así, como es retrocompatible, el estándar
 * antiguo queda cubierto también.
 * 
 */
enum eGpsQuality
{
  NO_GPS = 0, /**< @param NO_GPS: Fix not available or invalid */
  GPS = 1,    /**< @param GPS: GPS SPS Mode, fix valid */
  DGPS = 2,   /**< @param DGPS: Differential GPS, SPS Mode, fix valid */
  PPS = 3,    /**< @param PPS: Differential GPS, PPS Mode, fix valid */
  RTK = 4,    /**< @param RTK: Real Time Kinematic. System used in RTK mode*  with fixed integers */
  FRTK = 5,   /**< @param FRTK: Float RTK. Satellite system used in RTK mode,* floating integers */
  RECKON = 6, /**< @param RECKON: Estimated (dead reckoning) Mode */
  MANUAL = 7, /**< @param MANUAL: Manual Input Mode */
  SIMUL = 8   /**< @param SIMUL: Simulator Mode */
};

/**
 * @brief Formato interno de la hora UTC
 * 
 */
typedef struct
{
  uint8_t Horas    ;  /**< Valor en horas (00-23) */
  uint8_t Minutos  ;  /**< Valor en minutos (00-59) */
  uint8_t Segundos ;  /**< Valor en segundos (00-59) */
} sUtcOfPosition;

/**
 * @brief Formato interno de los ángulos en latitud y longitud
 * @note Se ha tenido que separar minutos y décimas de minuto debido a que no
 * hay suficiente precisión con el formato de float que incluye Microchip, de
 * 24 bits, y de este modo no se podía almacenar correctamente los valores
 * recibidos por las diferentes tramas. Esto podría ocasionar problemas
 */
typedef struct
{
    uint8_t  Grados ; /**< El valor en grados */
    uint8_t  Minutos; /**< El valor en minutos sin decimales */
    uint16_t Decimas; /**< El valor de las décimas de minuto */
} sAngulo;


typedef struct gps_gsa_sentence_type{
  sUtcOfPosition    UtcOfPosition;       /**< El formato de la hora UTC es* 154002.000 -> 15:40:02 */
  bool              HayLatitud;          /**< Indica si hay valor de latitud.* Si no hay, tampoco hay valor de* dirección en la latitud */
  sAngulo           Latitude;            /**< Formato de la latitud recibida* 4154.2366 >> formato de la latitud* interna: 41º 54.2366' */
  enum  eLatitude   LatiDirection;       /**< N norte o S sur */
  bool              HayLongitud;          /**< Indica si hay valor de longitud. * Si no hay, tampoco hay valor de* dirección en la longitud */
  sAngulo           Longitude;           /**< Formato de la longitud recibida:* 00852.6716 >> Formato de la longitud* interna: 008º 52.6716' */
  enum  eLongitude  LongDirection;       /**< E este o W oeste */
  enum  eGpsQuality GpsQuality;          /**< Indicador de la calidad del GPS */
  int               NumberOfSatellites;  /**< Número de satélites en uso para el
                                          * cálculo de la posición */
  /*float             HorizontalDilution;  *//**< Parámetro HDOP. No se usa */ 
  /*float             AntennaAltitude;     *//**< Altitud de la antena, en
                                          * metros. No se usa */
  /* M */
  /*float             GeoidalSeparation;   *//**< Separación geoidal en metros.
                                          * No se usa */
  /* M */
  /* Campos 13 y 14 desestimados, no se aprecian en las tramas recibidas */
}GGA_sentence_t;


/**
 * @brief Estructura que almacena la trama GGA recibida
 * 
 */
typedef struct
{
  sUtcOfPosition    UtcOfPosition;       /**< El formato de la hora UTC es
                                          * 154002.000 -> 15:40:02 */
  bool              HayLatitud;          /**< Indica si hay valor de latitud.
                                          * Si no hay, tampoco hay valor de
                                          * dirección en la latitud */
  sAngulo           Latitude;            /**< Formato de la latitud recibida:
                                          * 4154.2366 >> formato de la latitud
                                          * interna: 41º 54.2366' */
  enum  eLatitude   LatiDirection;       /**< N norte o S sur */
  bool              HayLongitud;          /**< Indica si hay valor de longitud.
                                          * Si no hay, tampoco hay valor de
                                          * dirección en la longitud */
  sAngulo           Longitude;           /**< Formato de la longitud recibida:
                                          * 00852.6716 >> Formato de la longitud
                                          * interna: 008º 52.6716' */
  enum  eLongitude  LongDirection;       /**< E este o W oeste */
  enum  eGpsQuality GpsQuality;          /**< Indicador de la calidad del GPS */
  int               NumberOfSatellites;  /**< Número de satélites en uso para el
                                          * cálculo de la posición */
  /*float             HorizontalDilution;  *//**< Parámetro HDOP. No se usa */ 
  /*float             AntennaAltitude;     *//**< Altitud de la antena, en
                                          * metros. No se usa */
  /* M */
  /*float             GeoidalSeparation;   *//**< Separación geoidal en metros.
                                          * No se usa */
  /* M */
  /* Campos 13 y 14 desestimados, no se aprecian en las tramas recibidas */
}sTramaGga;

/**
 * @brief Clasificador del campo 1 de la trama GSA: Modo receptor
 * 
 */
enum eModoReceptor
{
  
  RM = 0, /**< @param M: Modo manual */
  RA = 1  /**< @param A: Modo automático */
  
};

/**
 * @brief Clasificador del campo 2 de la trama GSA: Modo fijación
 * 
 */
enum eModoFijacion
{
  
  FIX_NOT_AVAILABLE = 1, /**< @param FIX_NOT_AVAILABLE: * No hay fijación posible */
  FIX_2D = 2,            /**< @param FIX_2D: Fijación en 2D */
  FIX_3D = 3             /**< @param FIX_3D: Fijación en 3D */
  
};

/**
 * @brief Estructura que almacena la trama GSA recibida
 * 
 */
typedef struct
{
  
  enum eModoReceptor ModoReceptor;  /**< campo 1 de la trama GSA */
  enum eModoFijacion ModoFijacion;  /**< campo 2 de la trama GSA */

}GSA_sentence_t;

/**
 * @brief Clasificador del campo 2 de la trama RMC: Status
 * 
 */
enum eStatus
{
  SA = 0, /**< @param A: Status "Valid" */
  SV     /**< @param V: Status "Navigation receiver warning" */
};

/**
 * @brief Fecha recibida por la trama RMC
 * 
 */
typedef struct
{
  
  uint8_t Dia  ;  /**< Valor del día (00-31) */
  uint8_t Mes  ;  /**< Valor del mes (00-12) */
  uint8_t Anyo ;  /**< Valor del año (00-99) */
  
} sFecha;

/**
 * @brief Clasificador del campo 12 de la trama RMC:
 * Position system mode indicator
 * 
 */
enum eIndicator
{
  
  PA = 0, /**< @param A: Autonomous */
  PD = 1, /**< @param D: Differential */
  PE = 2, /**< @param E: Estimated */
  PM = 3, /**< @param M: Manual input */
  PS = 4, /**< @param S: Simulation mode */
  PN      /**< @param N: Data not valid */
  
};

/**
 * @brief Para valores float, se usa coma fija
 * 
 */
typedef struct
{
  uint16_t ParteEntera;  /**< 0 - 360 */
  uint8_t ParteDecimal;  /**< 0 - 99 */
} sComaFija;





typedef struct rmc_sentence_type {
  
  sUtcOfPosition    UtcOfPosition;       /**< El formato de la hora UTC es * 154002.000 -> 15:40:02 */
  enum  eStatus     Status;              /**< Indicador de validez de la trama*/
  bool              HayLatitud;          /**< Indica si hay valor de latitud.* Si no hay, tampoco hay valor de* dirección en la latitud */
  sAngulo           Latitude;            /**< Formato de la latitud recibida:* 4154.2366 >> formato de la latitud* interna: 41º 54.2366' */
  enum  eLatitude   LatiDirection;       /**< N norte o S sur */
  bool              HayLongitud;         /**< Indica si hay valor de longitud.* Si no hay, tampoco hay valor de* dirección en la longitud */
  sAngulo           Longitude;           /**< Formato de la longitud recibida:* 00852.6716 >> Formato de la longitud* interna: 008º 52.6716' */
  enum  eLongitude  LongDirection;       /**< E este o W oeste */
  sComaFija         SpeedOvertheGround;  /**< Velocidad. Medida en Nudos */
  sComaFija         Degrees;             /**< Seguimiento correcto realizado * en grados verdaderos */
  sFecha            Date;                /**< La fecha de la trama recibida */

  enum eIndicator   ModeIndicator;       /**< Indicador del modo de posición* del sistema */
  
}RMC_sentence_t;





/**
 * @brief Estructura que almacena la trama recibida
 *
 * @param gps_buffer es el búfer donde se almacena físicamente la trama
 * recibida, y es el búfer que se modificará cuando se reciba una trama
 * 
 */
typedef struct gps_sentence_type{
  
  uint8_t gps_buffer[MAX_SENTENCE_LENGTH];  /**< La trama que se va recibiendo se* aloja aquí dentro */
  uint8_t position             ;  /**< Es el valor que tiene la última* posición de la entrada del búfer. * Sustituto de strlen */
  bool ProcesaLaTrama                  ;  /**< Le dice al bucle principal que* ya ha recibido una trama completa y * que la procese*/

} gps_sentence_t;



#define ELMS_TRAMA 79





















































#endif //  GPS_EXTENSIONS_H