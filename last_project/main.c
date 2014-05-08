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

extern void EnableInterrupts(void);
extern void DisableInterrupts(void);

/******************************************************************************
 * Defines
 *****************************************************************************/
#define PORTA   0x40004000
#define PORTB   0x40005000
#define PORTC   0x40006000
#define PORTD   0x40007000
#define PORTE   0x40024000
#define PORTF   0x40025000

#define     UART_CMD_DISCOVERY        'A'
#define     UART_CMD_SLAVE_FOUND      'B'
#define     UART_CMD_RESET_DISPLAY    'C'

extern bool check;
volatile bool OneSecond = false;

bool detectMaster(void)
{
 char myChar = 0;
 
 while(1)
 {
	 // Send discovery packets to U5
	 uart5Tx(UART_CMD_DISCOVERY);
	 
	 // Check for a discovery packert on U2
	 myChar = uart2Rx(false);
	 
	 // If we recieved a discovery packet on U2, send a SLAVE_FOUND command
	 // back on U2 and return false.  
	 if( myChar ==  UART_CMD_DISCOVERY )
	 {
		 uartTxPoll(UART0,"SLAVE: TX UART_CMD_SLAVE_FOUND\n\r");
		 uart2Tx(UART_CMD_SLAVE_FOUND);
			 
		 //This device is the slave
		 return false;
	 }
	 
	 // Check for a SLAVE_FOUND command on U5.  If you receive the SLAVE_FOUND
	 // command, return true, else check everything again
	 myChar = uart5Rx(false);
	 if(myChar == UART_CMD_SLAVE_FOUND)
	 {
		 // This device is the master
		 uartTxPoll(UART0,"MASTER: RX UART_CMD_SLAVE_FOUND\n\r");
		 return true;
	 }
	
		// If no messages are received on either of the UARTs,
		// wait 1 second before sending another discovery message
		uartTxPoll(UART0,"Discovery: Waiting 1 Second\n\r");     
		while(OneSecond == false){};
		uartTxPoll(UART0,"Discovery: Waiting Done\n\r");
		OneSecond=false;
	}
}
 

void slaveApp(void)
{
	uartTxPoll(UART0, "Hello this the slave talking..\n\r");
	while(1) {}
}
	
void masterApp(void)
{
	uartTxPoll(UART0, "Hi! This is your master talking!\n\r");
	while(1) {}
}
//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
	char data[80];
	uint32_t adctest;
	bool masterDevice = true;
	
	//CONFIGURE BOARD
  PLL_Init(); //given to us
	initBoard();
	
	sprintf(data, "About to detect master.. \n\r");
	uartTxPoll(UART0, data);
	
	masterDevice = detectMaster();

	sprintf(data, "Detected master.. \n\r");
	uartTxPoll(UART0, data);
	
	if (masterDevice)
	{
			masterApp();
	}
	else
	{
			slaveApp();
	}
	
	//MAIN LOOP
  while(1)
  {
//		if (check)
//		{
//			adctest = GetADCval(1);
//			sprintf(data, "ADCVAL: %d \n\r", adctest);
//			uartTxPoll(UART0, data);
//			check = false;
//		}
//		examineButtons();
		
		examineButtons();
  	updateDisplay();
		updateGenerationRate();
  };

  
}

