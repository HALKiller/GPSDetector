// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

//  * * * * * * *      C O M M E N T   B L O C K     * * * * * * * * * * * * * * * * * * * * * *  //




//   * * * * * *      I N C L U D E S   B L O C K     * * * * * * * * * * * * * * * * * * * * *  //
#include "checksumming.h"



#include <stdint.h>



//   * * * * *      D A T A   T Y P E S ,   S T R U C T S ,   E N U M S     * * * * * * * * * *  //




//   * * * * * * *      C O N S T A N T   E X P R E S S I O N S     * * * * * * * * * * * * *   // 
 



//  * * * * * * *      M A C R O   D E F I N I T I O N S      * * * * * * * * * * * * // 
 



//   * * * * * *     S T A T I C   D A T A   D E C L A R A T I O N S     * * * * * * * * * * *   //





//   * * * * * * * *      P R I V A T E   F U N C T I O N S   P R O T O T Y P E S     * * * * * *  //




//   * * * * * * *      P U B L I C   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *  //

// this function can create a checksum or can get used 
// to test for the correct reception of a sentence -->
uint8_t checksumming_chcksum_creator(uint8_t *the_data, uint8_t data_len){
	
	uint8_t hlooper = 0;
	uint8_t chcksum = 0;
	
	
	for(hlooper = 0; hlooper < data_len; hlooper++)
	{
		chcksum = chcksum ^ (*the_data);
		the_data++;
	}

	return chcksum;
	
}




uint8_t checksumming_chcksum_checker(uint8_t *the_data, uint8_t data_len){
  
  uint8_t hlooper = 0;
  uint8_t chcksum = 0;

  for(hlooper = 0; hlooper < data_len; hlooper++)
  {   
    chcksum = chcksum ^ (*the_data);
		the_data++;
  }

	return chcksum;

}

//   * * * * * *      P R I V A T E   F U N C T I O N S   B O D Y     * * * * * * * * * * * * * *   //



//   * * * * * *      I S R  - -  H A N D E L E R     * * * * * * * * * * * * * *   //



// EOF