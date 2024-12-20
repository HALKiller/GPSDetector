// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

/******************************************************************************/




#include "DDS.h"

#include "timers.h"

#include "io_port_sfr_names.h"

#include "UART.h"

#include "generic_union_flgs.h"

#include "gps_extensions.h"

#include "gps.h"

#include "pwm_luz.h"

#include <string.h>


//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 

static uint8_t msg_header[] = "<<\r\n";
static uint8_t msg_tail[] = "X\r\n";

//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 


#define AD9954_TRANSMITE_CARACTER_ASCII(X) \
AD9954TransmiteByte((uint8_t)(gTablaAsciiABaudot[ (int)X ]))


//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //

const uint8_t CFR1[]      = { 0x80, 0x00, 0x00, 0x40 };
const uint8_t CFR1Info[]  = { 0x00 };
const uint8_t CFR2[]      = { 0x00, 0x08, 0x24 };
const uint8_t CFR2Info[]  = { 0x01 };
const uint8_t RSCW0[]     = { 0x00, 0x00, 0x00, 0x00, 0x00 };
const uint8_t RSCW0Info[] = { 0x07 };
const uint8_t RSCW1[]     = { 0x00, 0x00, 0x01, 0x04, 0x00 };
const uint8_t RSCW1Info[] = { 0x08 };
const uint8_t RSCW2[]     = { 0x00, 0x00, 0x02, 0x08, 0x00 };
const uint8_t RSCW2Info[] = { 0x09 };
const uint8_t RSCW3[]     = { 0x00, 0x00, 0x03, 0x0C, 0x00 };
const uint8_t RSCW3Info[] = { 0x0A };
const uint8_t FTWInfo[]   = { 0x0B };

uint16_t gTimerBitNormal;
uint16_t gTimerBitFinal;

extern bool gTrueSi150FalseSi300;


//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

static void AD9954Enciende(void);
static void AD9954Apaga(void);
static void SpiTransmite(uint8_t Dato);
static void AD9954TransmiteString(uint8_t *CadenaAscii, uint8_t NumDatos);
static void AD9954EscribeRegistro( uint8_t *DireccionRegistro, uint16_t NumDatos, uint8_t *DatosAEnviar);
static void AD9954TransmiteByte(uint8_t ByteBaudot);
static void AD9954PulsoUpdate(void);
static void AD9954PulsoIoSync(void);
static void select_bank(uint8_t bankbits);

//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //


#if DEBUGGING_BB_IS_ON&&0

void Transmite(bool TransmiteRadiogonio){

	
  uint8_t sync_time = get_sync_time();
  
  DB_PRINT(sentence_buffer.gps_buffer); 
  UART_CRLF;

}

#else
  
void Transmite(bool TransmiteRadiogonio){

  uint8_t sync_time = get_sync_time();

  UART_CRLF;
  DB_PRINT(sentence_buffer.gps_buffer); 
  UART_CRLF;
  
#if DB_67  
  SET_START_STOP = true;
#endif  


  // ENCIENDE_TRANSMISOR();  // That does not exist anymore...
  AD9954Enciende();
  // DB_PRINT("A1\r\n");
  AD9954Configura();
  // DB_PRINT("A2\r\n");
  // Transmisión de sincronismo
  PS1_AD9954 = TransmiteRadiogonio;
  // DB_PRINT("A3\r\n");
  PS0_AD9954 = 1;
  
  // DB_PRINT("B\r\n");
  // we can set up here the tmr1 overflower...
  for ( uint8_t i = 0; i < sync_time; i++ )
  {
        __delay_ms(1000);
    // Delay ( 33250 ); // Equivalente en función a un delay de medio segundo
    // Delay ( 33250 );
  }
  
  // DB_PRINT("C\r\n");
  // Transmisión de pitada de datos
  AD9954TransmiteMensaje();
  // DB_PRINT("D\r\n");
  PS0_AD9954 = 1;
  AD9954Apaga();
  
  DDS_flush_buffer();
  
  // APAGA_TRANSMISOR();
  // DB_PRINT("E\r\n");
#if DB_67  
  SET_START_STOP = false;
#endif  

}

#endif





// POSRED: it would be possible to reduce the ROM footprint if we create a 
// typedef structure and loop over the setting
// 
void AD9954Configura(void){
  
 
  
  // Cuidado con las variables const.
  // usar el define AD9954_SPI_MACRO 1 si no envía las palabras correctas
  AD9954PulsoUpdate();
  
  select_bank(0u);

  AD9954PulsoIoSync();
  
  AD9954EscribeRegistro((uint8_t *)CFR1Info, 4, (uint8_t *)CFR1);
  
  AD9954PulsoUpdate();
  
  AD9954PulsoIoSync();
  
  AD9954EscribeRegistro((uint8_t *)CFR2Info, 3, (uint8_t *)CFR2);
  
  AD9954PulsoUpdate();
  AD9954PulsoIoSync();
  
  AD9954EscribeRegistro((uint8_t *)RSCW0Info, 5, (uint8_t *)RSCW0);
  
  AD9954PulsoUpdate();
  select_bank(1u);
  
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)RSCW1Info, 5, (uint8_t *)RSCW1);
  
  AD9954PulsoUpdate();
  select_bank(2u);
  
  
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)RSCW2Info, 5, (uint8_t *)RSCW2);
  
  AD9954PulsoUpdate();
  select_bank(3u);
  
  
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)RSCW3Info, 5, (uint8_t *)RSCW3);
  
  AD9954PulsoUpdate();
  select_bank(0);
  
  
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)FTWInfo, 4, FTW0);
  AD9954PulsoUpdate();
  select_bank(1);
  
  
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)FTWInfo, 4, FTW1);
  AD9954PulsoUpdate();
  select_bank(2u);
  
  
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)FTWInfo, 4, FTW2);
  AD9954PulsoUpdate();
  select_bank(3u);
  
  
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)FTWInfo, 4, FTW3);
  AD9954PulsoIoSync();
  
}





#if 1

void AD9954TransmiteMensaje(void){
  
  // TODO: get the Null_Terminator len_cnt 
#if 0  
  gEntradaDeTrama.PosicionDelBufer = strlen((const char *) gEntradaDeTrama.BuferDeEntrada);
#else
  sentence_buffer.position = gps_buffer_get_len();
#endif

  
#if 1

  AD9954TransmiteString(&msg_header[0], 4);
  // AD9954TransmiteString(4, &msg_header[0]);
  
#else  
  
  AD9954_TRANSMITE_CARACTER_ASCII('<');   // Originalmente se incluyen 8 caracteres de cambio a letras
  AD9954_TRANSMITE_CARACTER_ASCII('<');   // Con 2 también se puede transmitir el mensaje sin problemas
  AD9954_TRANSMITE_CARACTER_ASCII('\n');  // Nueva línea
  AD9954_TRANSMITE_CARACTER_ASCII('\r');
  
#endif
  
  AD9954TransmiteString(sentence_buffer.gps_buffer, sentence_buffer.position);
  // AD9954TransmiteString(gEntradaDeTrama.BuferDeEntrada, gEntradaDeTrama.PosicionDelBufer);
  
  // AD9954TransmiteString(gEntradaDeTrama.PosicionDelBufer, gEntradaDeTrama.BuferDeEntrada);

#if 1
  AD9954TransmiteString(&msg_tail[0], 3);
  // AD9954TransmiteString(3, &msg_tail[0]);
#else  
  AD9954_TRANSMITE_CARACTER_ASCII('X');  // Carácter de fin de trama
  AD9954_TRANSMITE_CARACTER_ASCII('\n'); // Otra nueva línea, para que se pueda detectar en el programa receptor
  AD9954_TRANSMITE_CARACTER_ASCII('\r');
#endif  


  // DDS_flush_buffer();
  
}

#elif 1



void AD9954TransmiteMensaje(void){
  // Actualiza el tamaño del búfer para saber cuántos bits ha de transmitir
  // sentence_buffer.position = strlen(sentence_buffer.gps_buffer);
  sentence_buffer.position = gps_buffer_get_len();
  AD9954_TRANSMITE_CARACTER_ASCII('<');   // Originalmente se incluyen 8 caracteres de cambio a letras
  AD9954_TRANSMITE_CARACTER_ASCII('<');   // Con 2 también se puede transmitir el mensaje sin problemas
  AD9954_TRANSMITE_CARACTER_ASCII('\n');  // Nueva línea
  AD9954_TRANSMITE_CARACTER_ASCII('\r');
  AD9954TransmiteString( sentence_buffer.gps_buffer, sentence_buffer.position);
  // AD9954TransmiteString(gEntradaDeTrama.PosicionDelBufer, gEntradaDeTrama.BuferDeEntrada);
  AD9954_TRANSMITE_CARACTER_ASCII('X');  // Carácter de fin de trama
  AD9954_TRANSMITE_CARACTER_ASCII('\n'); // Otra nueva línea, para que se pueda detectar en el programa receptor
  AD9954_TRANSMITE_CARACTER_ASCII('\r');
  
  
  
}


#else
  

void AD9954TransmiteMensaje(void){
  // Actualiza el tamaño del búfer para saber cuántos bits ha de transmitir
  sentence_buffer.position = strlen(gEntradaDeTrama.BuferDeEntrada);
  AD9954_TRANSMITE_CARACTER_ASCII('<');   // Originalmente se incluyen 8 caracteres de cambio a letras
  AD9954_TRANSMITE_CARACTER_ASCII('<');   // Con 2 también se puede transmitir el mensaje sin problemas
  AD9954_TRANSMITE_CARACTER_ASCII('\n');  // Nueva línea
  AD9954_TRANSMITE_CARACTER_ASCII('\r');
  AD9954TransmiteString( gEntradaDeTrama.BuferDeEntrada, gEntradaDeTrama.PosicionDelBufer);
  // AD9954TransmiteString(gEntradaDeTrama.PosicionDelBufer, gEntradaDeTrama.BuferDeEntrada);
  AD9954_TRANSMITE_CARACTER_ASCII('X');  // Carácter de fin de trama
  AD9954_TRANSMITE_CARACTER_ASCII('\n'); // Otra nueva línea, para que se pueda detectar en el programa receptor
  AD9954_TRANSMITE_CARACTER_ASCII('\r');
  DDS_flush_buffer();
}


#endif




//   * * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //



// this got tested and worked
static void SpiTransmite(uint8_t Dato)
{

  uint8_t hlooper = 0;

  TMR2 = 0;
  TMR2IF = 0;
  TMR2IE = 1;


  // for(hlooper = 0; hlooper < 8; hlooper++)
  for(hlooper = 8; hlooper > 0; hlooper--)
  {
    
    SCLK_AD9954 = 0;
    
    SDIO_AD9954 = (Dato >> (hlooper - 1)) & 0x01;;  // Dato & shifts[7 - hlooper];
    
  
    TMR2ON = 1;
    while ( TMR2ON );
    SCLK_AD9954 = 1;  // shifted data in
    TMR2ON = 1;
    while ( TMR2ON );
    
  }
  
  SCLK_AD9954 = 0;
  SDIO_AD9954 = 0;
  
  
  
}


static void AD9954EscribeRegistro(uint8_t *DireccionRegistro, uint16_t NumDatos, uint8_t *DatosAEnviar){
  
  SpiTransmite(DireccionRegistro[0]);
  
  for (uint16_t i = 0; i < NumDatos; i++)
  {
    SpiTransmite(DatosAEnviar[i]);
  }
}

#if READBACK_DDS

static void dds_read_register(uint8_t *DireccionRegistro, uint16_t NumDatos, uint8_t *DatosAEnviar){
  
  
  SpiTransmite(DireccionRegistro[0]);
  
  for (uint16_t i = 0; i < NumDatos; i++)
  {
    SpiReceive(DatosAEnviar[i]);
  }
  
  
}



#endif


// This is used D. 02062021
static void AD9954TransmiteByte(uint8_t ByteBaudot){
	
	uint8_t hlooper = 0;
  
  // t_byte datoconvertido;
  union8_t datoconvertido;
  datoconvertido.reg = ByteBaudot;
	
#if 1	// it is easy to change the lookup table so that this instruction would not be necessay --> Speed...
  datoconvertido.reg ^= 0xFF;	// inverting the byte because...? the lookup table is not prepared...weak!
#endif
  /****************************************************************************/
  /*                      NUEVO ENFOQUE: USAR TIMER 2                         */
  /****************************************************************************/
  // Se utilizará el timer 2 en la transmisión de los bits individuales de cada
  //carácter por el AD9954
  // Los 7 primeros bits tienen igual ancho temporal
  // if ( !gTrueSi150FalseSi300 )
#if 1
  if ( !TX_150BPS )    
  {
    
    // 300 bps
    // that creates 3332us --> 1000/300 = 3.333ms
    T2_PRESCALER = TMR2_300BAUD_PRE;
    
    T2_POSTSCALER = TMR2_300BAUD_POST;
   
    PR2 = TMR2_300BAUD_PR;  // 216;
    
    
    
  }
  else
  {
    // 6,56ms --> wtf...
    // 150 bps
    // Ajuste del PostScaler
    T2_POSTSCALER = TMR2_150BAUD_POST;
    // Ajuste del PreScaler
    T2_PRESCALER = TMR2_150BAUD_PRE; 
    
    PR2 = TMR2_150BAUD_PR;
  }

#else
  
  if ( !TX_150BPS )    
  {
    // 300 bps
    // that creates only 3,24ms --> that could get better...
    // Ajuste del PostScaler
    // T2CONbits.T2OUTPS = 0b1110; // PostScaler 15     T2CONbits.TOUTPS = 0b1110; // PostScaler 15
    T2_POSTSCALER = 0b1110;	// 0x0E
    // Ajuste del PreScaler
    T2CONbits.T2CKPS = 0b00; // Preescaler 1
    PR2 = 216;
    
    
    
  }
  else
  {
    // 6,56ms --> wtf...
    // 150 bps
    // Ajuste del PostScaler
    T2_POSTSCALER = 0b0111;	// T2CONbits.T2OUTPS = 0b0111; // PostScaler 7  T2CONbits.TOUTPS = 0b0111; // PostScaler 7
    // Ajuste del PreScaler
    T2CONbits.T2CKPS = 0b01; // Preescaler 4
    PR2 = 205;
  }

#endif	
  TMR2 = 0;
  // Se ajustan los flags de interrupción y activación
  TMR2IF = 0;
  TMR2IE = 1;
  // La solución consiste en encender el timer 2 e ir leyendo el estado de
  // encendido del timer 2 hasta comprobar que se ha apagado. Una vez se ha
  // apagado, quiere decir que el timer 2
	
	

	for(hlooper = 8; hlooper > 0; hlooper--)
	{
		PS0_AD9954 = (datoconvertido.reg >> (hlooper - 1)) & 0x01;	// datoconvertido.b7;
		TMR2ON = 1; 
		while ( TMR2ON );	
	}

	
}



// trying a few tweaks to make it work...
#if DEBUG_16F1936_PITADA && 0

static void AD9954TransmiteString(uint8_t *CadenaAscii, uint8_t NumDatos){
	unsigned char readback[75];
	unsigned char *rb_pnt;
	rb_pnt = &readback;
#if DEBUG_16F1936_PITADA
	unsigned char d_byte = 0;
#endif	
  for (volatile uint8_t i = 0; i < NumDatos; i++){
		// this line helps me to see the start of each new byte sent 
	#if DEBUG_16F1936_PITADA
		DB_LED = !DB_LED;
	#endif

		// lets change it to a pure pointer and no conversion to int....
		#if 0
		d_byte = (uint8_t)(gTablaAsciiABaudot[ *CadenaAscii ]);
		CadenaAscii++;
		#else
			d_byte = (uint8_t)(gTablaAsciiABaudot[ (int)CadenaAscii[i] ]);
		#endif
		*rb_pnt = d_byte;
		rb_pnt++;


		AD9954TransmiteByte(d_byte);
		
		UART_char(d_byte);
		UART_CRLF;
		
			// AD9954TransmiteByte((uint8_t)(gTablaAsciiABaudot[ (int)CadenaAscii[i] ]));
	#if DEBUG_16F1936_PITADA && 0
		if((CadenaAscii[i] == '<') || (CadenaAscii[i] == '>')){
			DB_LED = !DB_LED;
			AD9954TransmiteByte(d_byte);
		}
	#endif
  }
	*rb_pnt = NULL_TERMINATOR;
	DB_PRINT(&readback);
	UART_CRLF;
}

#elif 1

static void AD9954TransmiteString(uint8_t *CadenaAscii, uint8_t NumDatos){
  
  
  uint8_t hlooper = 0;
  
  for(hlooper = 0; hlooper < NumDatos; hlooper++)
  {
    
    AD9954TransmiteByte((uint8_t)(gTablaAsciiABaudot[ (int)CadenaAscii[hlooper] ]));
    
  }
  
}


#else
	
static void AD9954TransmiteString(uint8_t *CadenaAscii, uint8_t NumDatos){
  
  
  uint8_t hlooper = 0;
  
  for(hlooper = 0; hlooper < NumDatos; hlooper++)
  {
    
    AD9954TransmiteByte((uint8_t)(gTablaAsciiABaudot[ (int)CadenaAscii[hlooper] ]));
    
  }
  
}

#endif






void DDS_flush_buffer(void)
{

#ifdef GPSPARSER_H

  IniciaBuferDeTramas();
  
#else
  
  volatile int8_t i;
  for ( i = 0; i < ELMS_TRAMA; i++ )
  {
    sentence_buffer.gps_buffer[i] = '\0';
  }
  sentence_buffer.position = 0;

  
#endif

}




static void AD9954Enciende(void){
  
  TMR2_ON = FALSE;
  
  T2_POSTSCALER = 0u; // 0b0000; // PostScaler 1  T2CONbits.TOUTPS = 0b0000; // PostScaler 1

  T2_PRESCALER = 0u;  // 0b00; // Preescaler 1
  
  PR2 = TMR2_DDS_CFG_PR;

  TMR2 = 0u;

  TMR2_IF = false;
  
  TMR2_IE = true;

  VALIM_TRANSMISSION_ON();
#if DO_TRANSMIT_RF  
  KS_50_ON;
#endif  
  // VDD_AD9954 = true;	// this switches on the modulator and also the power stage...
#if 0
  __delay_ms(250u); // give a reaaaally long time here and check if the while thing of not transmitting dissapears...
#else  
  __delay_ms(110u);  // that should get handled by a timer but that creates overhead...
#endif  
  RESET_AD9954 = false;
  
}

static void AD9954Apaga(void)
{
  
  RESET_AD9954 = false;
  VALIM_TRANSMISSION_OFF();
  KS_50_OFF;
  // VDD_AD9954 = false;
  
}


static void select_bank(uint8_t bankbits){
  
  PS1_AD9954 = bankbits & 0x02u;
  PS0_AD9954 = bankbits & 0x01u;
  
}


static void AD9954PulsoUpdate(void){
  
  TMR2ON = true;
  
  UPDATE_AD9954 = 1;
  
  while ( TMR2ON );

  UPDATE_AD9954 = 0;
  
}

static void AD9954PulsoIoSync(void){
  
  TMR2ON = true;
  SYNC_AD9954 = 1;
  while ( TMR2ON );
  SYNC_AD9954 = 0;
  
}


// OBSOLETE-----------------------------


#if 0


static void SpiTransmite(uint8_t Dato)
{
  // Se resetea el valor del timer 2
  TMR2 = 0;
  // Se ajustan los flags de interrupción y activación
  TMR2IF = 0;
  TMR2IE = 1;

  SCLK_AD9954 = 0;
  SDIO_AD9954 = ((t_byte *) &Dato)->b7;
  
  TMR2ON = 1;
  SCLK_AD9954 = ~SCLK_AD9954;
  while ( TMR2ON );
  
  TMR2ON = 1;
  SCLK_AD9954 = ~SCLK_AD9954;
  SDIO_AD9954 = ((t_byte *) &Dato)->b6;
  while ( TMR2ON );
  
  TMR2ON = 1;
  SCLK_AD9954 = ~SCLK_AD9954;
  while ( TMR2ON );
  
  TMR2ON = 1;
  SCLK_AD9954 = ~SCLK_AD9954;
  SDIO_AD9954 = ((t_byte *) &Dato)->b5;
  while ( TMR2ON );
  
  TMR2ON = 1;
  SCLK_AD9954 = ~SCLK_AD9954;
  while ( TMR2ON );
  
  TMR2ON = 1;
  SCLK_AD9954 = ~SCLK_AD9954;
  SDIO_AD9954 = ((t_byte *) &Dato)->b4;
  while ( TMR2ON );
  
  TMR2ON = 1;
  SCLK_AD9954 = ~SCLK_AD9954;
  while ( TMR2ON );
  
  TMR2ON = 1;
  SCLK_AD9954 = ~SCLK_AD9954;
  SDIO_AD9954 = ((t_byte *) &Dato)->b3;
  while ( TMR2ON );
  
  TMR2ON = 1;
  SCLK_AD9954 = ~SCLK_AD9954;
  while ( TMR2ON );
  
  TMR2ON = 1;
  SCLK_AD9954 = ~SCLK_AD9954;
  SDIO_AD9954 = ((t_byte *) &Dato)->b2;
  while ( TMR2ON );
  
  TMR2ON = 1;
  SCLK_AD9954 = ~SCLK_AD9954;
  while ( TMR2ON );
  
  TMR2ON = 1;
  SCLK_AD9954 = ~SCLK_AD9954;
  SDIO_AD9954 = ((t_byte *) &Dato)->b1;
  while ( TMR2ON );
  
  TMR2ON = 1;
  SCLK_AD9954 = ~SCLK_AD9954;
  while ( TMR2ON );
  
  TMR2ON = 1;
  SCLK_AD9954 = ~SCLK_AD9954;
  SDIO_AD9954 = ((t_byte *) &Dato)->b0;
  while ( TMR2ON );
  
  TMR2ON = 1;
  SCLK_AD9954 = ~SCLK_AD9954;
  while ( TMR2ON );
  
  SCLK_AD9954 = ~SCLK_AD9954;
  SDIO_AD9954 = 0;
}







void AD9954SelectorBanco0(void)
{
  PS1_AD9954 = 0;
  PS0_AD9954 = 0;
}

void AD9954SelectorBanco1(void) 
{
  PS1_AD9954 = 0;
  PS0_AD9954 = 1;
}

void AD9954SelectorBanco2(void)
{
  PS1_AD9954 = 1;
  PS0_AD9954 = 0;
}

void AD9954SelectorBanco3(void)
{
  PS1_AD9954 = 1;
  PS0_AD9954 = 1;
}



void AD9954Configura(void){
  
  
  
  
  // Cuidado con las variables const.
  // usar el define AD9954_SPI_MACRO 1 si no envía las palabras correctas
  AD9954PulsoUpdate();

  AD9954SelectorBanco0();
  AD9954PulsoIoSync();
  // DB_PRINT("B1\r\n");
  AD9954EscribeRegistro((uint8_t *)CFR1Info, 4, (uint8_t *)CFR1);
  // DB_PRINT("B2\r\n");
  AD9954PulsoUpdate();
  // DB_PRINT("B3\r\n");
  AD9954PulsoIoSync();
  // DB_PRINT("B4\r\n");
  AD9954EscribeRegistro((uint8_t *)CFR2Info, 3, (uint8_t *)CFR2);
  AD9954PulsoUpdate();
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)RSCW0Info, 5, (uint8_t *)RSCW0);
  AD9954PulsoUpdate();
  AD9954SelectorBanco1();
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)RSCW1Info, 5, (uint8_t *)RSCW1);
  AD9954PulsoUpdate();
  AD9954SelectorBanco2();
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)RSCW2Info, 5, (uint8_t *)RSCW2);
  AD9954PulsoUpdate();
  AD9954SelectorBanco3();
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)RSCW3Info, 5, (uint8_t *)RSCW3);
  AD9954PulsoUpdate();
  AD9954SelectorBanco0();
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)FTWInfo, 4, FTW0);
  AD9954PulsoUpdate();
  AD9954SelectorBanco1();
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)FTWInfo, 4, FTW1);
  AD9954PulsoUpdate();
  AD9954SelectorBanco2();
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)FTWInfo, 4, FTW2);
  AD9954PulsoUpdate();
  AD9954SelectorBanco3();
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)FTWInfo, 4, FTW3);
  AD9954PulsoIoSync();
}





#if 0
// it seems to me that this is not getting used at all actually....
void AD9954AjustaVelocidadDeTransmision(int Bps)
{
  switch(Bps)
  {
    case 300:
      gTrueSi150FalseSi300 = false;
      break;
    case 150:
      gTrueSi150FalseSi300 = true;
      break;
  }
}

#endif




#endif





// EOF