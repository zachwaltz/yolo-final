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
#include "final_spi.h"
#include "final_uart.h"
//#include "./lib/pll.lib" DON'T DO THIS, EVER.


/******************************************************************************
 * Defines
 *****************************************************************************/
#define PORTA   0x40004000
#define PORTB   0x40005000
#define PORTC   0x40006000
#define PORTD   0x40007000
#define PORTE   0x40024000
#define PORTF   0x40025000

bool alertRowUpdate;
bool alertADC0;
bool alertDebounce;

//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
	char data[] = "This is a test message..";
	
	//CONFIGURE BOARD
  PLL_Init(); //given to us
	initBoard();
	
	//MAIN LOOP
  while(1)
  {
		uartTxPoll(UART0, data);
  };

  
}

