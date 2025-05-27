//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //

// because we are having a really slow clock with 500kHz i dropped the Baudrate down to 4800.
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

#define BB_UART           LATBbits.LATB6// LATAbits.LATA7 // 


#if MIPS == 1

#define BB_PR	104 // 

#define BB_PSA  TMR6_01_PRESCALER 
#define BB_POST TMR6_02_POSTSCALER

#elif MIPS==2

#define BB_PR	208 // 

#define BB_PSA  TMR6_01_PRESCALER 
#define BB_POST TMR6_02_POSTSCALER

#elif MIPS==4

#define BB_PR	104 // 

#define BB_PSA  TMR6_04_PRESCALER 
#define BB_POST TMR6_02_POSTSCALER

#elif MIPS==8

#define BB_PR	52 // 153600

#define BB_PSA  TMR6_01_PRESCALER 
#define BB_POST TMR6_02_POSTSCALER

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
  
	// set_bb_uart(true);	// because UART TX is idle high
	
  BB_UART = true;
  
  
  
}


#if 1

void send_bb_string(const unsigned char *str_pnt){
	
  uint8_t lencnt = 0;
	uint8_t r_shifter = 0x01;
	uint8_t hlooper = 0;
	uint8_t bitwise[10];
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
    
    t_GIE = GIE;
    GIE = false;
    
    BB_TMR_IF = false;
    BB_TMR_ON = true;

    for(hlooper = 0; hlooper < 10; hlooper++)
    {
      
      BB_UART = bitwise[hlooper];

      BB_TMR_IF = false;
      
      while(BB_TMR_IF == false);

    }
    
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
    
    BB_UART = *bit_arr;
    
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