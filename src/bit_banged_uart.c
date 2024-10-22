//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //



#include "bit_banged_uart.h"
// #include "bsp.h"
#include "io_port_sfr_names.h"
#include "xc.h"
#include "Global.h"

#include <stdint.h>





//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //




//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 

static const uint8_t const_max_str_length = 64;

//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 
#define NULL_TERMINATOR '\0'

#define STARTBIT false	
#define STOPBIT true

#define TMR2_ON T2CONbits.TMR2ON
#define TMR2_POSTSCALER T2CONbits.T2OUTPS
#define TMR2_PRESCALER  T2CONbits.T2CKPS
#define TMR2_IF PIR1bits.TMR2IF


// #define TMR2_ON T2CONbits.TMR2ON
// #define TMR2_POSTSCALER T2CONbits.T2OUTPS
// #define TMR2_PRESCALER  T2CONbits.T2CKPS
// #define TMR2_IF PIR1bits.TMR2IF

//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //





//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //

// void send_string(uint8_t *str_pnt);
static void bang_char_out(uint8_t the_char);

static void set_bb_uart(uint8_t b_val);
static void send_it(const uint8_t *bit_arr);

//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //






// setting up for 104us --> 1 bit length time for 9600 Baud
// by 16MHz clock
void init_TMR_bitbang_uart(void){
	
	TMR2_ON = false;
	
	
	// 1:1 = 0
	// 1:2 = 1
	// 1:3 = 2
	// 1:4 = 3
	//		.
	//		.
	//		.
	TMR2_POSTSCALER = 0x01;	
	
	// 1:1 = 0
	// 1:4 = 1
	// 1:16 = 2
	// 1:64 = 3
	TMR2_PRESCALER = 0x00;
	
	PR2 = 208;
	
	TMR2_IF = false;
	
	
	TRISAbits.TRISA2 = false; // the UART__BB
  
	set_bb_uart(true);	// because UART TX is idle high
	
}



void send_string_bit_banged(const char *str_pnt){
	uint8_t lencnt = 0;
	
	while((*str_pnt != NULL_TERMINATOR) && (const_max_str_length > lencnt))
	{
		bang_char_out(*str_pnt);
		str_pnt++;
		lencnt++;
	}
	
	
	
}

void send_bit_banged_char(uint8_t the_char){
	
	bang_char_out(the_char);
	
	
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

#if 0
static void send_bit(uint8_t the_bit){
	
	LED_GREEN = true;
	
	TMR2_ON = false;
	TMR2 = 0;
	TMR2_IF = false;
		
	GIE = false;
	
	set_bb_uart(the_bit);
	

	
	TMR2_ON = true;
	
	while(TMR2_IF == false);
	// {
		// CLRWDT();
	// }
	
	GIE = true;
	
	LED_GREEN = false;
	
}

#endif


#if 1

static void send_it(const uint8_t *bit_arr){
	
	uint8_t hlooper = 0;
	
	
	TMR0_ON = false;
	TMR0 = 0;
	
	// GIE = false;
	
	TMR0_IF = false;
	TMR0_ON = true;

	for(hlooper = 0; hlooper < 10; hlooper++)
	{
		set_bb_uart(*bit_arr);
		bit_arr++;
		
		TMR0_IF = false;
		while(TMR0_IF == false);
		// {
			// CLRWDT();
		// }
		

	}
	
	
  TMR0_ON = false;
  TMR0_IF = false;
	
	// GIE = true;

}


#else
	
static void send_it(uint8_t *bit_arr){
	
	uint8_t hlooper = 0;
	
	
	TMR2_ON = false;
	TMR2 = 0;
	TMR2_IF = false;
	GIE = false;
	set_bb_uart(false);
	
	TMR2_ON = true;
	while(TMR2_IF == false);
	// {
		// CLRWDT();
	// }
	
	for(hlooper = 0; hlooper < 8; hlooper++)
	{
		set_bb_uart(*bit_arr);
	
		bit_arr++;
		TMR2_IF = false;
		while(TMR2_IF == false);
		// {
			// CLRWDT();
		// }
		

	}
	
	set_bb_uart(true);
	TMR2_IF = false;
	while(TMR2_IF == false);
	// {
		// CLRWDT();
	// }
	
	GIE = true;
	TMR2_ON = false;;

}

#endif


static void set_bb_uart(uint8_t b_val){
	
	if(b_val == true)
	{
		TX_LUZ = false;
	}
	else
	{
		TX_LUZ = true;
	}

	
}





//   * * * * * *      I S R  - -  H A N D E L E R     * * * * * * * * * * * * * *   //



// EOF