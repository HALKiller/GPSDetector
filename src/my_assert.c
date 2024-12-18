// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#if 1 


//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //


#include "my_assert.h"

#include "Global.h"

#include "UART.h"

// #include "xc.h"

#include <stdint.h>




//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

// function pointer to assertion indicator
// static void (*assert_ind)(void) = NULL;


//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 



//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 


//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //




//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //



//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

// sets user's choice of failure indicator function
// void assert_init(void (*assert_indicator)(void)) {
    // assert_ind = assert_indicator;
// }


// failure function called by macro.
#if ENABLE_ASSERTIONS==1

#if MY_TRY

void assertion_failure(char *expr, char* file){
	
	DB_PRINT("\r\nASSERT FAILURE!");
	
	while(1);
  
}

#elif 1

void assertion_failure(char* expr, char* file, uint16_t linenum) {


    // debugger can look at these
    volatile const char* v_expr;
    volatile const char* v_file;
   
    volatile uint32_t v_linenum;
 
    // assign variables to volatiles
    v_expr     = expr;
    v_file     = file;
    // v_basefile = basefile;
    v_linenum  = linenum;
		
		// UART_CRLF;
		DB_PRINT("\r\nASSERT FAILURE \r\n");
		// UART_int(*expr);
		DB_PRINT("File: ");
		DB_PRINT(file);
		
		DB_PRINT("In line: ");
		UART_int(linenum);
    
		
		// call function that indicates an assertion failure
    // assert_ind();
 
    // halt
    while (1)
    {
    
      __delay_ms(2500);
      RESET();
      
      
    }
    
    
    
    
}

#else
	
void assertion_failure(char* expr, char* file, int linenum) {
uint8_t hlooper = 0;	



    // debugger can look at these
    volatile const char* v_expr;
    volatile const char* v_file;
   
    volatile int v_linenum;
 
    // assign variables to volatiles
    v_expr     = expr;
    v_file     = file;
    // v_basefile = basefile;
    v_linenum  = linenum;
		
		// UART_CRLF;
		DB_PRINT_NON_DMA("\r\nASSERT FAILURE \r\n");
		// UART_int(*expr);
		DB_PRINT_NON_DMA("File: ");
		DB_PRINT_NON_DMA(file);
		
		DB_PRINT_NON_DMA("In line: ");
		UART_int(linenum);
    
		
		// call function that indicates an assertion failure
    // assert_ind();
 
    // halt
    while (1);
}

#endif

#endif


//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //



//   * * * * * *      I S R  - -  H A N D E L E R     * * * * * * * * * * * * * *   //



#else

//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //


#include "my_assert.h"

#include "UART.h"

#include "xc.h"

#include <stdint.h>




//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //

// function pointer to assertion indicator
// static void (*assert_ind)(void) = NULL;


//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 



//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 


//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //




//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //



//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

// sets user's choice of failure indicator function
// void assert_init(void (*assert_indicator)(void)) {
    // assert_ind = assert_indicator;
// }


// failure function called by macro.
#if ENABLE_ASSERTIONS==1

#if MY_TRY
void assertion_failure(char *expr, char* file){
	
	DB_PRINT("\r\nASSERT FAILURE!");
	
	while(1);
}

#elif 1

void assertion_failure(char* expr, char* file, uint16_t linenum) {
  


  // debugger can look at these
  volatile const char* v_expr;
  volatile const char* v_file;
 
  volatile uint16_t v_linenum;

  // assign variables to volatiles
  v_expr     = expr;
  v_file     = file;
  // v_basefile = basefile;
  v_linenum  = linenum;
  
  // UART_CRLF;
  DB_PRINT("\r\nASSERT FAILURE \r\n");
  // UART_int(*expr);
  DB_PRINT("File: ");
  DB_PRINT(file);
  
  DB_PRINT("In line: ");
  UART_int(linenum);
  
  
  // call function that indicates an assertion failure
  // assert_ind();

  // halt
  while (1)
  {
    
    __delay_ms(2500);
    RESET();

  }
  

    
}

#else
	
void assertion_failure(char* expr, char* file, int linenum) {
uint8_t hlooper = 0;	



    // debugger can look at these
    volatile const char* v_expr;
    volatile const char* v_file;
   
    volatile int v_linenum;
 
    // assign variables to volatiles
    v_expr     = expr;
    v_file     = file;
    // v_basefile = basefile;
    v_linenum  = linenum;
		
		// UART_CRLF;
		DB_PRINT_NON_DMA("\r\nASSERT FAILURE \r\n");
		// UART_int(*expr);
		DB_PRINT_NON_DMA("File: ");
		DB_PRINT_NON_DMA(file);
		
		DB_PRINT_NON_DMA("In line: ");
		UART_int(linenum);
    
		
		// call function that indicates an assertion failure
    // assert_ind();
 
    // halt
    while (1);
}

#endif

#endif


//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //



//   * * * * * *      I S R  - -  H A N D E L E R     * * * * * * * * * * * * * *   //


#endif
// EOF