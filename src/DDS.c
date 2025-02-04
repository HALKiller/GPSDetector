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

#include "eeprom.h"

#include <string.h>


//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 

static uint8_t msg_header[] = "<<\r\n";
static uint8_t msg_tail[] = "X\r\n";

static uint8_t FTW_TX[5];

//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 



#define SDIO_TRIS TRISAbits.TRISA1
#define PIN_INPUT 1u
#define PIN_OUTPUT 0u
#define AD9954_SDI PORTAbits.RA1
#define FTW_CHANNELS_IN_USE 2
#define TRANSMIT_TWICE 0

#define AD9954_TRANSMITE_CARACTER_ASCII(X) \
AD9954TransmiteByte((uint8_t)(gTablaAsciiABaudot[ (int)X ]))


//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //

#if USE_OLD_CFG_SETTER

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

#elif 1

// this is with the default values which are not getting changed from the DDS
// first byte is the address and then from MSB to LSB
const uint8_t CFR1Info[]  = { 0x00, 0x80, 0x00, 0x00, 0x40 };

const uint8_t CFR2Info[]  = { 0x01, 0x18, 0x08, 0x24 }; // <-- here ...  aaaa nd...

const uint8_t RSCW0Info[] = { 0x07, 0x00, 0x00, 0x00, 0x00, 0x00 };

const uint8_t RSCW1Info[] = { 0x08, 0x00, 0x00, 0x01, 0x04, 0x00 };

const uint8_t RSCW2Info[] = { 0x09, 0x00, 0x00, 0x02, 0x08, 0x00 };

const uint8_t RSCW3Info[] = { 0x0A, 0x00, 0x00, 0x03, 0x0C, 0x00 };

const uint8_t FTWInfo[]   = { 0x0B };

// these are EEPROM directions
const uint8_t c_base_address[4] = { 0x22u, 0x27u, 0x2Cu, 0x31u };


const uint8_t *reg_pnt[] = {
  
  &RSCW0Info[0],
  &RSCW1Info[0],
  &RSCW2Info[0],
  &RSCW3Info[0],
  
};



#else

// first byte is the address and then from MSB to LSB
const uint8_t CFR1Info[]  = { 0x00, 0x80, 0x00, 0x00, 0x40 };

const uint8_t CFR2Info[]  = { 0x01, 0x00, 0x08, 0x24 };

const uint8_t RSCW0Info[] = { 0x07, 0x00, 0x00, 0x00, 0x00, 0x00 };

const uint8_t RSCW1Info[] = { 0x08, 0x00, 0x00, 0x01, 0x04, 0x00 };

const uint8_t RSCW2Info[] = { 0x09, 0x00, 0x00, 0x02, 0x08, 0x00 };

const uint8_t RSCW3Info[] = { 0x0A, 0x00, 0x00, 0x03, 0x0C, 0x00 };

const uint8_t FTWInfo[]   = { 0x0B };

// these are EEPROM directions
// const uint8_t c_base_address[4] = { 0x27u, 0x22u, 0x31u, 0x2Cu };
const uint8_t c_base_address[4] = { 0x22u, 0x27u, 0x2Cu, 0x31u };


const uint8_t *reg_pnt[] = {
  
  &RSCW0Info[0],
  &RSCW1Info[0],
  &RSCW2Info[0],
  &RSCW3Info[0],
  
};


#endif


const uint8_t shifts_8bit[8] = {
	
	(1U << 0), (1U << 1), (1U << 2), (1U << 3), 
	(1U << 4), (1U << 5), (1U << 6), (1U << 7), 

	
}; 





uint16_t gTimerBitNormal;
uint16_t gTimerBitFinal;

extern bool gTrueSi150FalseSi300;


//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

static void AD9954Enciende(void);
static void AD9954Apaga(void);
static void SpiTransmite(uint8_t Dato);
static void AD9954TransmiteString(uint8_t *CadenaAscii, uint8_t NumDatos);
  
static void AD9954EscribeRegistro( uint8_t *DireccionRegistro, uint8_t NumDatos);
static void get_next_frequ_from_EEPROM(uint8_t base_address_indexer);
static uint8_t SpiReceive(void);


static void AD9954TransmiteByte(uint8_t ByteBaudot);
static void AD9954PulsoUpdate(void);
// static void AD9954PulsoIoSync(void);
static void select_bank(uint8_t bankbits);

static uint8_t dds_read_all(void);
static uint8_t dds_read_register(uint8_t *DireccionRegistro, uint8_t NumDatos);

//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //


#if DEBUGGING_BB_IS_ON&&0

void Transmite(bool TransmiteRadiogonio){

	
  uint8_t sync_time = get_sync_time();
  
  DB_PRINT(sentence_buffer.gps_buffer); 
  UART_CRLF;

}

#elif TRANSMIT_TWICE

  
void Transmite(bool TransmiteRadiogonio){

  uint8_t sync_time = get_sync_time();

  uint8_t re_cfg_cnt = 0;
  


#if CREATE_TX_MESSAGE_AFTER_DDS_CFG&&0    
  messages_before_transmission();
#endif


  // ENCIENDE_TRANSMISOR();  // That does not exist anymore...
  AD9954Enciende();



#if READBACK_DDS

  AD9954Configura();

  if(dds_read_all() == false)
  {

    DDS_CFG_ERR = true;
    // todo: set err flag for txing...
  }
 
#else
  
  AD9954Configura();  
  
#endif  
  
  
#if CREATE_TX_MESSAGE_AFTER_DDS_CFG&&1   
  messages_before_transmission();
#endif

  
  // Transmisión de sincronismo
  PS1_AD9954 = false; // TransmiteRadiogonio;

  PS0_AD9954 = 1;
  
  // we can set up here the tmr1 overflower...
  for ( uint8_t i = 0; i < sync_time; i++ )
  {
    __delay_ms(1000);
  }

  AD9954TransmiteMensaje();

  PS0_AD9954 = 1;
 
#if READBACK_DDS

  if(DDS_CFG_ERR == true)
  {

    while(re_cfg_cnt < 5)
    {
      AD9954Configura();
    
      if(dds_read_all() == true)
      {
        re_cfg_cnt = 5;
      }
      else
      {
        re_cfg_cnt++;
        DDS_CFG_ERR = true;
        // todo: set err flag for txing...
      }
      
    }

#if CREATE_TX_MESSAGE_AFTER_DDS_CFG&&1   
    messages_before_transmission();
#endif

    // Transmisión de sincronismo
    PS1_AD9954 = false; // TransmiteRadiogonio;

    PS0_AD9954 = 1;
    
   
    // we can set up here the tmr1 overflower...
    for ( uint8_t i = 0; i < sync_time; i++ )
    {
      __delay_ms(1000);
    }

    AD9954TransmiteMensaje();

    PS0_AD9954 = 1;

    DDS_CFG_ERR = false;
    
  }
 
  
#endif 



 
  AD9954Apaga();
  
#if DEBUGGING_BB_IS_ON
  UART_CRLF;
  DB_PRINT(sentence_buffer.gps_buffer); 
  UART_CRLF;
#endif  
  
  DDS_flush_buffer();

#if DB_67&&0
  SET_START_STOP = false;
#endif  

}


#else
  
void Transmite(bool TransmiteRadiogonio){

  uint8_t sync_time = get_sync_time();

  uint8_t re_cfg_cnt = 0;
  
#if DEBUGGING_BB_IS_ON&&0
  UART_CRLF;
  DB_PRINT(sentence_buffer.gps_buffer); 
  UART_CRLF;
#endif
  
#if DB_67&&0
  SET_START_STOP = true;
#endif  

  DDS_CFG_ERR = false;
  
#if CREATE_TX_MESSAGE_AFTER_DDS_CFG&&0    
  messages_before_transmission();
#endif


  // ENCIENDE_TRANSMISOR();  // That does not exist anymore...
  AD9954Enciende();



#if READBACK_DDS
  while(re_cfg_cnt < 5)
  {
    AD9954Configura();
  
    if(dds_read_all() == true)
    {
      re_cfg_cnt = 5;
    }
    else
    {
      re_cfg_cnt++;
      DDS_CFG_ERR = true;
      // todo: set err flag for txing...
    }
    
  }
#else
  
  AD9954Configura();  
  
#endif  
  
  
#if CREATE_TX_MESSAGE_AFTER_DDS_CFG&&1   
  messages_before_transmission();
#endif
  
  
  
  // Transmisión de sincronismo
  PS1_AD9954 = false; // TransmiteRadiogonio;

  PS0_AD9954 = 1;
  
 
  // we can set up here the tmr1 overflower...
  for ( uint8_t i = 0; i < sync_time; i++ )
  {
    __delay_ms(1000);
  }
  

  AD9954TransmiteMensaje();

  PS0_AD9954 = 1;
  
  AD9954Apaga();
  
#if DEBUGGING_BB_IS_ON
  UART_CRLF;
  DB_PRINT(sentence_buffer.gps_buffer); 
  UART_CRLF;
#endif  
  
  DDS_flush_buffer();

#if DB_67&&0
  SET_START_STOP = false;
#endif  

}

#endif




// POSRED: it would be possible to reduce the ROM footprint if we create a 
// typedef structure and loop over the setting
// 


static void get_next_frequ_from_EEPROM(uint8_t base_address_indexer){
  
  uint8_t base_address = c_base_address[base_address_indexer];
  uint8_t hlooper = 0;
  
  FTW_TX[0] = 0x0B;
  
  for ( hlooper = 0; hlooper < 4; hlooper++ )
  {
    
    FTW_TX[hlooper + 1] = LeerEeprom ( base_address + hlooper );

  }
  
  
  
}



void AD9954Configura(void){
  
  uint8_t hlooper = 0;
  
  select_bank(0u);

  AD9954PulsoUpdate();
  
  AD9954EscribeRegistro((uint8_t *)CFR1Info, 5);
  
  AD9954EscribeRegistro((uint8_t *)CFR2Info, 4);
  

  for(hlooper = 0; hlooper < FTW_CHANNELS_IN_USE; hlooper++)
  {
    
    select_bank(hlooper);
    
    AD9954PulsoUpdate();
    
    AD9954EscribeRegistro(reg_pnt[hlooper], 6);
    
    get_next_frequ_from_EEPROM(hlooper);

    AD9954EscribeRegistro(FTW_TX, 5);

  }
  
}





#if 1

void AD9954TransmiteMensaje(void){
  
  // TODO: get the Null_Terminator len_cnt 
#if 0  
  gEntradaDeTrama.PosicionDelBufer = strlen((const char *) gEntradaDeTrama.BuferDeEntrada);
#else
  sentence_buffer.position = gps_buffer_get_len();
#endif


#if DO_TRANSMIT_RF
  
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


#endif  // DO_TRANSMIT_RF

  // DDS_flush_buffer();
  
}




#else
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





#endif




//   * * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //



// this got tested and worked
// AD9954 accepts data change on falling clock edge
// AD9954 shifts data in at rising clock edge
static void SpiTransmite(uint8_t Dato)
{

  uint8_t hlooper = 0;
  uint8_t shifter = 0x80;

  for(hlooper = 0; hlooper < 8; hlooper++)
  {

    SCLK_AD9954 = 0;
    // data changes...
    if(Dato & shifter)
    {
      SDIO_AD9954 = true;
    }
    else
    {
      SDIO_AD9954 = false;
    }

    asm("nop");
    asm("nop");
    // data moves into AD9954
    SCLK_AD9954 = 1;

    asm("nop");
    asm("nop");

    shifter = shifter >> 1;
  
  }

}








static void AD9954EscribeRegistro(uint8_t *DireccionRegistro, uint8_t NumDatos){
  
#if DEBUGGING_BB_IS_ON&&0

  DB_PRINT("R_write: ");

  for (int8_t i = 0; i < NumDatos; i++)
  {
    uart_hex(DireccionRegistro[i]);
    DB_PRINT(" ");
  }
  UART_CRLF;
#endif  
  
  SYNC_AD9954 = 0;
  
  for (int8_t i = 0; i < NumDatos; i++)
  {
    SpiTransmite(DireccionRegistro[i]);
  }
  
  SYNC_AD9954 = 1;
  
  AD9954PulsoUpdate();
  
}



static uint8_t SpiReceive(void){

  uint8_t hlooper = 0;
  uint8_t received = 0;
  uint8_t r_shifter = 0x80;

  for(hlooper = 0; hlooper < 8; hlooper++)
  {
    
    SCLK_AD9954 = 0;
 
    asm("nop");
    asm("nop");
 
    SCLK_AD9954 = 1;  // shifted data out

    asm("nop");
    asm("nop");

    if(AD9954_SDI == true)
    { 
      received |= r_shifter;
    }
    
    r_shifter = r_shifter >> 1;
 
  }

  return received;
  
}



#if READBACK_DDS


#if USE_NEW_SPI

// cfg for 2 frequencies and also readback only for 2 therefore...
static uint8_t dds_read_all(void){
  
  uint8_t hlooper = 0;
  uint8_t ret_value = true;
  
  select_bank(0u);

  AD9954PulsoUpdate();
    
  if(dds_read_register((uint8_t *)CFR1Info, 5) == false)
  {
    ret_value = false;
  }
  
  AD9954PulsoUpdate();
    
  if(dds_read_register((uint8_t *)CFR2Info, 4) == false)
  {
    ret_value = false;
  }
  AD9954PulsoUpdate();
  
  for(hlooper = 0; hlooper < FTW_CHANNELS_IN_USE; hlooper++)
  {
    
    select_bank(hlooper);
    
    AD9954PulsoUpdate();

    if(dds_read_register(reg_pnt[hlooper], 6) == false)
    {
      ret_value = false;
    }
    get_next_frequ_from_EEPROM(hlooper);

    if(dds_read_register(FTW_TX, 5) == false)
    {
      ret_value = false;
    }      
  }
  
  return ret_value;
  
}


#else
 

// cfg for 2 frequencies and also readback only for 2 therefore...
// thsi works but above is the version where we give a return value...
static uint8_t dds_read_all(void){
  
  uint8_t hlooper = 0;
  uint8_t ret_value = true;
  
  select_bank(0u);

  AD9954PulsoUpdate();
    
  dds_read_register((uint8_t *)CFR1Info, 5);

  AD9954PulsoUpdate();
    
  dds_read_register((uint8_t *)CFR2Info, 4);
  
  AD9954PulsoUpdate();
  
  for(hlooper = 0; hlooper < FTW_CHANNELS_IN_USE; hlooper++)
  {
    
    select_bank(hlooper);
    
    AD9954PulsoUpdate();

    dds_read_register(reg_pnt[hlooper], 6);

    get_next_frequ_from_EEPROM(hlooper);

    dds_read_register(FTW_TX, 5);
  }
  
  return ret_value;
  
}


#endif


// the readback is in the way that the highest bit needs to get set....

static uint8_t dds_read_register(uint8_t *DireccionRegistro, uint8_t NumDatos){
  
  // set highest bit of the address....
  uint8_t rx_tx_byte = DireccionRegistro[0] | 0x80;
  uint8_t hlooper = 0;
  uint8_t ret_value = 1u;
  
#if 0

  DB_PRINT("Reg_address: ");
  uart_hex(rx_tx_byte);
  DB_PRINT(" ");
 
#endif  
  
  SYNC_AD9954 = 0;
  
  SpiTransmite(rx_tx_byte);
  
  // therefore invert the TRIS register for the SDIO PIN
  SDIO_TRIS = PIN_INPUT;

  // now we need to change to receive information...
  for (hlooper = 1; hlooper < NumDatos; hlooper++)
  {
    rx_tx_byte = SpiReceive();
    
#if DEBUGGING_BB_IS_ON&&0
     uart_hex(rx_tx_byte);
#endif
    
    if(rx_tx_byte != DireccionRegistro[hlooper])
    {
      
#if DEBUGGING_BB_IS_ON
      DB_PRINT("\r\nB:");
      uart_hex(DireccionRegistro[0] | 0x80);
      uart_hex(rx_tx_byte);
      ret_value = false;
#else
      // in release we abort instantly
    
      SYNC_AD9954 = 1;
      SDIO_TRIS = PIN_OUTPUT;
      return false;
      
#endif      

    }

  }
  
  SYNC_AD9954 = 1;
  
  SDIO_TRIS = PIN_OUTPUT;
  
  return ret_value;
  
}



#endif




// This is used D. 02062021
static void AD9954TransmiteByte(uint8_t ByteBaudot){
	
	uint8_t hlooper = 0;
  
  uint8_t shifter = 0x80;
  
  union8_t datoconvertido;
  
  datoconvertido.reg = ByteBaudot;
	
#if 1	// it is easy to change the lookup table so that this instruction would not be necessay --> Speed...
  datoconvertido.reg ^= 0xFF;	// inverting the byte because...? the lookup table is not prepared...weak!
#endif



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

#endif	

  TMR2 = 0;
  TMR2IF = 0;
  TMR2IE = 1;


	for(hlooper = 0; hlooper < 8; hlooper++)
	{

    if(datoconvertido.reg & shifter)
    {
      PS0_AD9954 = true;
    }
    else
    {
      PS0_AD9954 = false;
    }
    
    shifter = shifter >> 1;
  
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


  
  PS1_AD9954 = (bankbits>>1) & 0x01u;
  PS0_AD9954 = bankbits & 0x01u;
  
}



static void AD9954PulsoUpdate(void){
  
  TMR2ON = true;
  
  UPDATE_AD9954 = 1;
  
  while ( TMR2ON );

  UPDATE_AD9954 = 0;
  
}







// OBSOLETE-----------------------------


#if 0
static void AD9954PulsoIoSync(void){
  
  TMR2ON = true;
  SYNC_AD9954 = 1;
  while ( TMR2ON );
  SYNC_AD9954 = 0;
  
}



#endif





// EOF