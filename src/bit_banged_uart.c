//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //
// 06112025:
// starting to implement also a bit_banged_receiver --> therefore it is necessary to consider
// a few details: for example should it perhaps only be possible to adjust the BB_SPEED to the tx?
// or rx? higer clock speeds and lower clock speeds?
// on one hand it sounds nice to have higher throughput but that might not be possible on the low clock speed settings...
// it seems better to stay active for reception ONLY on high clock speeds and that seems in that case 32MHz --> becaue this system shall only be used of rdebugging internally
//  because we are having a really slow clock with 500kHz i dropped the Baudrate down to 4800.
// the easiest way to to so was to increase the Post from 1:1 to 2:1


//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //



#include "bit_banged_uart.h"

#include "Global.h"

#include "timers.h"

#include "generic_union_flgs.h"

#include "xc.h"

// #include "io_port_sfr_names.h"

// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  
#ifndef FILE_BIT_BANGED_UART_DB_ENABLED
#define FILE_BIT_BANGED_UART_DB_ENABLED 0
#endif
#if FILE_BIT_BANGED_UART_DB_ENABLED
#define DB_PRINT(str) G_DB_PRINT(str)
#define UART_int(var) G_UART_INT(var)
#else
#define DB_PRINT(str)
#define UART_int(var)
#endif
// - - - - - - - - - - - - - - - - - - - -  D E B U G G I N G   P R I N T   O U T   - - - - - - - - - - - - - - - - - - - - //  


//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //




//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 

static const uint8_t const_max_str_length = 96;

#if BB_UART_USE_ASM
// Common RAM permits context save/restore without changing the incoming bank.
// Used only with GIE disabled; send_bb_string is not reentrant.
static volatile __near uint8_t bb_asm_w;
static volatile uint8_t bb_asm_context[5]; // Swapped STATUS/BSR, FSR0L/H, PCLATH
static volatile __near uint8_t bb_asm_count;
#endif

//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 
// #define NULL_TERMINATOR '\0'
#define STARTBIT false
#define STOPBIT true


#define BB_TMR            TMR6
#define BB_TMR_ON         T6CONbits.TMR6ON
#define BB_TMR_POSTSCALER T6CONbits.T6OUTPS
#define BB_TMR_PRESCALER  T6CONbits.T6CKPS
#define BB_TMR_IF         PIR3bits.TMR6IF
#define BB_TMR_IE         PIE3bits.TMR6IE

// #define ICSPCLCK 			LATBbits.LATB6
// #define ICSPDAT 			  LATBbits.LATB7

#define BB_TX_UART           LATBbits.LATB6// LATAbits.LATA7 // 
#define BB_RX_UART           LATBbits.LATB7// LATAbits.LATA7 // 


#if MIPS == 1

#define BB_PR	104 // 

#define BB_PSA  TMR6_01_PRESCALER 
#define BB_POST TMR6_02_POSTSCALER

#elif MIPS==2

#define BB_PR	208 // 

#define BB_PSA  TMR6_01_PRESCALER 
#define BB_POST TMR6_02_POSTSCALER

#elif MIPS==4



//BB_BAUD 166667
#define BB_PR	24 // 
#define BB_PSA  TMR6_01_PRESCALER 
#define BB_POST TMR6_01_POSTSCALER

// BAUD 76
#if 0
#define BB_PR	104 // 
#define BB_PSA  TMR6_04_PRESCALER 
#define BB_POST TMR6_02_POSTSCALER
#endif

#elif MIPS==8

// 250000 Baudrate
#define BB_PR	32 // 76923 //9600    // 153600
#define BB_PSA  TMR6_01_PRESCALER 
#define BB_POST TMR6_01_POSTSCALER

#if 0
#define BB_PR	52 // 76923 //9600    // 153600
#define BB_PSA  TMR6_01_PRESCALER 
#define BB_POST TMR6_02_POSTSCALER
#endif

#if 0
#define BB_PR	208 // 

#define BB_PSA  TMR6_04_PRESCALER 
#define BB_POST TMR6_02_POSTSCALER
#endif


#else	
wat
#endif

#define BB_SLOW	12


//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //





//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

// void send_string(uint8_t *str_pnt);
static void bang_char_out(uint8_t the_char);

static void set_bb_uart(uint8_t b_val);

static void send_it(uint8_t *bit_arr);


//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //







void init_TMR_bitbang_uart(uint8_t clockspeed){
	
  
	BB_TMR_ON = false;
	
	
	// 1:1 = 0
	// 1:2 = 1
	// 1:3 = 2
	// 1:4 = 3
	//		.
	//		.
	//		.
	BB_TMR_POSTSCALER = BB_POST; // 0x01;	
	
	// 1:1 = 0
	// 1:4 = 1
	// 1:16 = 2
	// 1:64 = 3
	BB_TMR_PRESCALER = BB_PSA; // 0x00;
	
  
	PR6 = BB_PR;
	
  if(clockspeed == SLOW_CLOCK)
  {
    PR6 = BB_SLOW;
  }
  
	BB_TMR_IF = false;
	
	TRISBbits.TRISB6 = false; // the UART__BB
  TRISBbits.TRISB7 = true;  // the RX_Pin
	// set_bb_uart(true);	// because UART TX is idle high
	
  BB_TX_UART = true;
  
  
  
}


#if 1

void send_bb_string(const unsigned char *str_pnt){
	
  uint8_t lencnt = 0;
	uint8_t r_shifter = 0x01;
	uint8_t hlooper = 0;
#if BB_UART_USE_ASM
  volatile uint8_t bitwise[10]; // Consumed by assembly, not by C expressions.
#else
	uint8_t bitwise[10];
  const uint8_t *bit_pnt;
#endif
  char temp_char;
  
  uint8_t t_GIE;
  
  if(FAST_CLOCK == FALSE)
  {
    PR6 = BB_SLOW;
  }
  else
  {
    PR6 = BB_PR;
  }

  bitwise[0] = STARTBIT;
  bitwise[9] = STOPBIT;
  
	while((*str_pnt != NULL_TERMINATOR) && (const_max_str_length > lencnt))
	{
    
    temp_char = *str_pnt;
    
    r_shifter = 0x01;
    
    for(hlooper = 1; hlooper < 9; hlooper++)
      
    {
      
      if(temp_char & r_shifter)
      {
        bitwise[hlooper] = 1u;
      }
      else
      {
        bitwise[hlooper] = 0u;
      }
      
      r_shifter = r_shifter << 1;
      
    }

    BB_TMR_ON = false;
    BB_TMR = 0;

#if !BB_UART_USE_ASM
    bit_pnt = bitwise;
    hlooper = sizeof(bitwise);
#endif
    
    t_GIE = GIE;
    GIE = false;
    
    BB_TMR_IF = false;
#if BB_UART_USE_ASM
    // XC8 1.x inline assembly does not declare register clobbers. Preserve all
    // changed CPU registers, including the program-page selector PCLATH.
    // XC8 places this function within one program page, including both labels.
    // This path is tied to RB6 TX and Timer6/PIR3 bit 3, as defined above.
    // Continuing bit: 14 instruction cycles + 3 per unsuccessful timer poll.
    asm("movwf _bb_asm_w");
    asm("swapf BSR,w");
    asm("banksel _bb_asm_context");
    asm("movwf (_bb_asm_context+1) & 0x7f");
    asm("swapf STATUS,w");
    asm("movwf _bb_asm_context & 0x7f");
    asm("movf FSR0L,w");
    asm("movwf (_bb_asm_context+2) & 0x7f");
    asm("movf FSR0H,w");
    asm("movwf (_bb_asm_context+3) & 0x7f");
    asm("movf PCLATH,w");
    asm("movwf (_bb_asm_context+4) & 0x7f");
    asm("movlw low(send_bb_string@bitwise)");
    asm("movwf FSR0L");
    asm("movlw high(send_bb_string@bitwise)");
    asm("movwf FSR0H");
    asm("movlw 10");
    asm("movwf _bb_asm_count");
    asm("pagesel bb_uart_asm_bit");
    asm("banksel T6CON");
    asm("bsf T6CON & 0x7f,2");
    asm("bb_uart_asm_bit:");
    asm("moviw FSR0++");
    asm("rrf WREG,w");
    asm("banksel LATB");
    asm("btfss STATUS,0");
    asm("bcf LATB & 0x7f,6");
    asm("btfsc STATUS,0");
    asm("bsf LATB & 0x7f,6");
    asm("banksel PIR3");
    asm("bcf PIR3 & 0x7f,3");
    asm("bb_uart_asm_wait:");
    asm("btfss PIR3 & 0x7f,3");
    asm("goto bb_uart_asm_wait");
    asm("decfsz _bb_asm_count,f");
    asm("goto bb_uart_asm_bit");
    asm("banksel _bb_asm_context");
    asm("movf (_bb_asm_context+2) & 0x7f,w");
    asm("movwf FSR0L");
    asm("movf (_bb_asm_context+3) & 0x7f,w");
    asm("movwf FSR0H");
    asm("movf (_bb_asm_context+4) & 0x7f,w");
    asm("movwf PCLATH");
    asm("swapf _bb_asm_context & 0x7f,w");
    asm("movwf STATUS");
    asm("swapf (_bb_asm_context+1) & 0x7f,w");
    asm("movwf BSR");
    asm("swapf _bb_asm_w,f");
    asm("swapf _bb_asm_w,w");
#else
    BB_TMR_ON = true;

    do
    {
      
      BB_TX_UART = *bit_pnt++;

      BB_TMR_IF = false;
      
      while(BB_TMR_IF == false);

    } while(--hlooper != 0);
#endif
    
    BB_TMR_ON = false;
    BB_TMR_IF = false;
    GIE = t_GIE;  

    str_pnt++;
    lencnt++;
    
  }

}




#else

void send_bb_string(const unsigned char *str_pnt){
	
  uint8_t lencnt = 0;
	
#if 1  
  if(FAST_CLOCK == FALSE)
  {
    PR6 = BB_SLOW;
  }
  else
  {
    PR6 = BB_PR;
  }
#endif  
  
	while((*str_pnt != NULL_TERMINATOR) && (const_max_str_length > lencnt))
	{
		bang_char_out(*str_pnt);
		str_pnt++;
		lencnt++;
	}
	
	
	
}




//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //


	
static void bang_char_out(uint8_t the_char){
	
	uint8_t the_bit = false;
	uint8_t hlooper = 0;
	uint8_t bitwise[10];
	
  bitwise[0] = STARTBIT;
	for(hlooper = 0; hlooper < 8; hlooper++)
	{
		
		bitwise[hlooper + 1] = ((the_char >> hlooper) & 1);
		
	}
	bitwise[9] = STOPBIT;
  
	send_it(&bitwise[0]);

}


static void send_it(uint8_t *bit_arr){
	
	uint8_t hlooper = 0;
	
	BB_TMR_ON = false;
	BB_TMR = 0;
	
	GIE = false;
	
	BB_TMR_IF = false;
	BB_TMR_ON = true;

	for(hlooper = 0; hlooper < 10; hlooper++)
	{
    
    BB_TX_UART = *bit_arr;
    
		bit_arr++;
		
		BB_TMR_IF = false;
    
		while(BB_TMR_IF == false);

	}
  
  BB_TMR_ON = false;
  BB_TMR_IF = false;
	GIE = true;

}


#endif



//   * * * * * *      I S R  - -  H A N D E L E R     * * * * * * * * * * * * * *   //



// EOF
