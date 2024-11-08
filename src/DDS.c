// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

/******************************************************************************/




#include "DDS.h"

#include "timers.h"

#include "io_port_sfr_names.h"

#include "UART.h"

#include "generic_union_flgs.h"

#include <string.h>


//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 

static uint8_t msg_header[] = "<<\r\n";
static uint8_t msg_tail[] = "X\r\n";

//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 

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
static void SpiTransmite(uint8_t Dato);
static void AD9954TransmiteString( uint8_t NumDatos, uint8_t *CadenaAscii );
static void AD9954EscribeRegistro( uint8_t *DireccionRegistro, uint16_t NumDatos, uint8_t *DatosAEnviar);
static void AD9954TransmiteByte(uint8_t ByteBaudot);
static void AD9954PulsoUpdate(void);
static void AD9954PulsoIoSync(void);
static void select_bank(uint8_t bankbits);

//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

#if 1


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

#endif





#if 1

// POSRED: it would be possible to reduce the ROM footprint if we create a 
// typedef structure and loop over the setting
// 
void AD9954Configura(void){
  
  
  
  
  AD9954Enciende();
  
  // DB_PRINT("A:\r\n");
  
  // Cuidado con las variables const.
  // usar el define AD9954_SPI_MACRO 1 si no envía las palabras correctas
  AD9954PulsoUpdate();
  
  select_bank(0u);
  // AD9954SelectorBanco0();
  
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)CFR1Info, 4, (uint8_t *)CFR1);
  
  // DB_PRINT("B:\r\n");
  
  AD9954PulsoUpdate();
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)CFR2Info, 3, (uint8_t *)CFR2);
  
  // DB_PRINT("C:\r\n");
  
  AD9954PulsoUpdate();
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)RSCW0Info, 5, (uint8_t *)RSCW0);
 
  // DB_PRINT("D:\r\n");
  
  AD9954PulsoUpdate();
  select_bank(1u);
  // AD9954SelectorBanco1();
  
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)RSCW1Info, 5, (uint8_t *)RSCW1);
  // DB_PRINT("E:\r\n");
  AD9954PulsoUpdate();
  select_bank(2u);
  // AD9954SelectorBanco2();
  
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)RSCW2Info, 5, (uint8_t *)RSCW2);
  // DB_PRINT("F:\r\n");
  AD9954PulsoUpdate();
  select_bank(3u);
  // AD9954SelectorBanco3();
  
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)RSCW3Info, 5, (uint8_t *)RSCW3);
  // DB_PRINT("G:\r\n");
  AD9954PulsoUpdate();
  select_bank(0);
  // AD9954SelectorBanco0();
  
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)FTWInfo, 4, FTW0);
  AD9954PulsoUpdate();
  select_bank(1);
  // AD9954SelectorBanco1();
  
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)FTWInfo, 4, FTW1);
  AD9954PulsoUpdate();
  select_bank(2u);
  // AD9954SelectorBanco2();
  
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)FTWInfo, 4, FTW2);
  AD9954PulsoUpdate();
  select_bank(3u);
  // AD9954SelectorBanco3();
  
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)FTWInfo, 4, FTW3);
  AD9954PulsoIoSync();
}

#else
  

void AD9954Configura(void){
  
  
  AD9954Enciende();
  
  // Cuidado con las variables const.
  // usar el define AD9954_SPI_MACRO 1 si no envía las palabras correctas
  AD9954PulsoUpdate();
  AD9954SelectorBanco0();
  AD9954PulsoIoSync();
  AD9954EscribeRegistro((uint8_t *)CFR1Info, 4, (uint8_t *)CFR1);
  AD9954PulsoUpdate();
  AD9954PulsoIoSync();
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

#endif




#if 1

void AD9954TransmiteMensaje(void){
  
  // Actualiza el tamaño del búfer para saber cuántos bits ha de transmitir
  gEntradaDeTrama.PosicionDelBufer = strlen((const char *) gEntradaDeTrama.BuferDeEntrada);
  
#if 1

  AD9954TransmiteString(4, &msg_header[0]);
  
#else  
  
  AD9954_TRANSMITE_CARACTER_ASCII('<');   // Originalmente se incluyen 8 caracteres de cambio a letras
  AD9954_TRANSMITE_CARACTER_ASCII('<');   // Con 2 también se puede transmitir el mensaje sin problemas
  AD9954_TRANSMITE_CARACTER_ASCII('\n');  // Nueva línea
  AD9954_TRANSMITE_CARACTER_ASCII('\r');
  
#endif
  
  AD9954TransmiteString(gEntradaDeTrama.PosicionDelBufer, gEntradaDeTrama.BuferDeEntrada);

#if 1
  AD9954TransmiteString(3, &msg_tail[0]);
#else  
  AD9954_TRANSMITE_CARACTER_ASCII('X');  // Carácter de fin de trama
  AD9954_TRANSMITE_CARACTER_ASCII('\n'); // Otra nueva línea, para que se pueda detectar en el programa receptor
  AD9954_TRANSMITE_CARACTER_ASCII('\r');
#endif  


  AD9954LimpiaBufferTransmision();
  
}

#else
  

void AD9954TransmiteMensaje(void){
  // Actualiza el tamaño del búfer para saber cuántos bits ha de transmitir
  gEntradaDeTrama.PosicionDelBufer = strlen(gEntradaDeTrama.BuferDeEntrada);
  AD9954_TRANSMITE_CARACTER_ASCII('<');   // Originalmente se incluyen 8 caracteres de cambio a letras
  AD9954_TRANSMITE_CARACTER_ASCII('<');   // Con 2 también se puede transmitir el mensaje sin problemas
  AD9954_TRANSMITE_CARACTER_ASCII('\n');  // Nueva línea
  AD9954_TRANSMITE_CARACTER_ASCII('\r');
  AD9954TransmiteString(gEntradaDeTrama.PosicionDelBufer, gEntradaDeTrama.BuferDeEntrada);
  AD9954_TRANSMITE_CARACTER_ASCII('X');  // Carácter de fin de trama
  AD9954_TRANSMITE_CARACTER_ASCII('\n'); // Otra nueva línea, para que se pueda detectar en el programa receptor
  AD9954_TRANSMITE_CARACTER_ASCII('\r');
  AD9954LimpiaBufferTransmision();
}


#endif






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


//   * * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //


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

#else
	


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
    // SDIO_AD9954 = Dato & shifts[7 - hlooper];
  
    TMR2ON = 1;
    while ( TMR2ON );
    SCLK_AD9954 = 1;  // shifted data in
    TMR2ON = 1;
    while ( TMR2ON );
    
  }
  
  SCLK_AD9954 = 0;
  SDIO_AD9954 = 0;
  
  
  
}

#endif

static void AD9954EscribeRegistro(uint8_t *DireccionRegistro, uint16_t NumDatos, uint8_t *DatosAEnviar){
  
  SpiTransmite(DireccionRegistro[0]);
  
  for (uint16_t i = 0; i < NumDatos; i++)
  {
    SpiTransmite(DatosAEnviar[i]);
  }
}




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
    if ( !TX_150BPS )
    
  {
    // 300 bps
    // Ajuste del PostScaler
    // T2CONbits.T2OUTPS = 0b1110; // PostScaler 15     T2CONbits.TOUTPS = 0b1110; // PostScaler 15
    T2_POSTSCALER = 0b1110;	// 0x0E
    // Ajuste del PreScaler
    T2CONbits.T2CKPS = 0b00; // Preescaler 1
    PR2 = 216;
  }
  else
  {
    // 150 bps
    // Ajuste del PostScaler
    T2_POSTSCALER = 0b0111;	// T2CONbits.T2OUTPS = 0b0111; // PostScaler 7  T2CONbits.TOUTPS = 0b0111; // PostScaler 7
    // Ajuste del PreScaler
    T2CONbits.T2CKPS = 0b01; // Preescaler 4
    PR2 = 205;
  }
	
  TMR2 = 0;
  // Se ajustan los flags de interrupción y activación
  TMR2IF = 0;
  TMR2IE = 1;
  // La solución consiste en encender el timer 2 e ir leyendo el estado de
  // encendido del timer 2 hasta comprobar que se ha apagado. Una vez se ha
  // apagado, quiere decir que el timer 2
	
	

	
#if 1	
	for(hlooper = 8; hlooper > 0; hlooper--)
	{
		PS0_AD9954 = (datoconvertido.reg >> (hlooper - 1)) & 0x01;	// datoconvertido.b7;
		TMR2ON = 1; 
		while ( TMR2ON );	
	}
#else	
	PS0_AD9954 = datoconvertido.b7;
	TMR2ON = 1; 
	while ( TMR2ON );		
  PS0_AD9954 = datoconvertido.b6;
  TMR2ON = 1; while ( TMR2ON );
  PS0_AD9954 = datoconvertido.b5;
  TMR2ON = 1; while ( TMR2ON );
  PS0_AD9954 = datoconvertido.b4;
  TMR2ON = 1; while ( TMR2ON );
  PS0_AD9954 = datoconvertido.b3;
  TMR2ON = 1; while ( TMR2ON );
  PS0_AD9954 = datoconvertido.b2;
  TMR2ON = 1; while ( TMR2ON );
  PS0_AD9954 = datoconvertido.b1;
  TMR2ON = 1; while ( TMR2ON );
	

  PS0_AD9954 = datoconvertido.b0;
  TMR2ON = 1; while ( TMR2ON );
#endif	
	
	
	
	
}



// trying a few tweaks to make it work...
#if DEBUG_16F1936_PITADA && 0

static void AD9954TransmiteString(uint8_t NumDatos, uint8_t *CadenaAscii){
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
	UWT(&readback);
	UART_CRLF;
}




#else
	
static void AD9954TransmiteString(uint8_t NumDatos, uint8_t *CadenaAscii)
{
  for (volatile uint8_t i = 0; i < NumDatos; i++)
  {
    AD9954TransmiteByte((uint8_t)(gTablaAsciiABaudot[ (int)CadenaAscii[i] ]));
  }
}

#endif






void AD9954LimpiaBufferTransmision(void)
{
  // Hay que reciclar funciones, por lo que será necesario minimizar tanto el tamaño de la RAM como el de la ROM
#ifdef GPSPARSER_H

  IniciaBuferDeTramas();
  
#else
  
  volatile int8_t i;
  for ( i = 0; i < ELMS_TRAMA; i++ )
  {
    gEntradaDeTrama.BuferDeEntrada[i] = '\0';
  }
  gEntradaDeTrama.PosicionDelBufer = 0;
  gEntradaDeTrama.ProcesaLaTrama = false;
  
#endif

}

// It seems that this function is never called --> 
void AD9954InsertarEnBuffer(uint8_t *Cadena)
{
  strcpy(gEntradaDeTrama.BuferDeEntrada, Cadena);
}


static void AD9954Enciende(void){
  
	unsigned char hlooper = 0;

  T2_POSTSCALER = 0u; // 0b0000; // PostScaler 1  T2CONbits.TOUTPS = 0b0000; // PostScaler 1

  T2_PRESCALER = 0u;  // 0b00; // Preescaler 1
  
  PR2 = 80u;  // with the 32MHz clock

  TMR2 = 0u;

  TMR2_IF = false;
  
  TMR2_IE = true;

  VDD_AD9954 = true;	
  
  __delay_ms(110);  // that should get handled by a timer of i think....
  
  RESET_AD9954 = false;
  
}

void AD9954Apaga(void)
{
  
  RESET_AD9954 = false;
  VDD_AD9954 = false;
  
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








// EOF