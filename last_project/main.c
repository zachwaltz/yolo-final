//*****************************************************************************
//
//*****************************************************************************
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "lm4f120h5qr.h"
#include "team.h"
#include "final_gpio.h"
#include "final_adc.h"
#include "final_systick.h"


/******************************************************************************
 * Defines
 *****************************************************************************/
#define PORTA   0x40004000
#define PORTB   0x40005000
#define PORTC   0x40006000
#define PORTD   0x40007000
#define PORTE   0x40024000
#define PORTF   0x40025000


//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
	//CONFIGURE BOARD
  PLL_Init(); //given to us
	initializeGpioPins();
	initializeADC();
	
	//MAIN LOOP
  while(1)
  {

  };

  
}
