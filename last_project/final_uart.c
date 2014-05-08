#include "final_uart.h"
#include "final_gpio.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "lm4f120h5qr.h"
#include "circular_buffer.h"

#define BUFFERSIZE 16

CircularBuffer rxBuf2, txBuf2;
CircularBuffer rxBuf5, txBuf5;

/****************************************************************************
 * Configure UART 0 for 8-n-1 with RX and TX interrupts enabled.
 * Enable the RX and TX FIFOs as well.
 ****************************************************************************/
void initUART0(void){
	//VARIABLES
  uint32_t delay;
  UART_PERIPH *myUart = (UART_PERIPH *)UART0;
		
	//CLOCK GATING REGISTER FOR UART0
	SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0; 
  delay = 10000;
  while( delay != 0) delay--;
	
	//SET BAUD RATE FOR 115200
	myUart->IntegerBaudRateDiv = 43;
	myUart->FracBaudRateDiv = 26;
	
	//CONFIGURE THE LINE CONTROL FOR 8N1
  myUart->LineControl = UART_LCRH_WLEN_8 | UART_LCRH_FEN;
  
	//SET INTERRUPT LEVELS
	myUart->IntFIFOLevelSel = UART_IFLS_RX1_8 | UART_IFLS_TX1_8; 
	myUart->IntMask = UART_IM_RXIM | UART_IM_TXIM | UART_IM_RTIM;
	
	//REENABLE UART, TX, AND RX
	myUart->UARTControl = UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN;

	//SET NVIC PRIORITY FOR INTERRUPTS, THEN ENABLE (5)
  NVIC_PRI1_R |= 0x2000; //setting 13th bit to 1, level 1
	NVIC_EN0_R |= NVIC_EN0_INT5;
  
	//WAIT UNTIL UART0 IS AVAILABLE
  while( !(SYSCTL_PRUART_R & SYSCTL_PRUART_R0 )){}
  
	//ADDITIONAL DELAY, JUST TO BE SAFE!
	delay = 500;
  while( delay != 0) delay--;
}

void initUART2(void){
	//VARIABLES
  uint32_t delay;
  UART_PERIPH *myUart = (UART_PERIPH *)UART2;
		
	//CLOCK GATING REGISTER FOR UART0
	SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R2; 
  delay = 10000;
  while( delay != 0) delay--;
	
	//SET BAUD RATE (AND RETURN FALSE FOR INVALID BAUD RATES)
	myUart->IntegerBaudRateDiv = 43;
	myUart->FracBaudRateDiv = 26;
  
	//CONFIGURE THE LINE CONTROL FOR 8N1
  myUart->LineControl = UART_LCRH_WLEN_8 | UART_LCRH_FEN;
  
	//SET INTERRUPT LEVELS
	myUart->IntFIFOLevelSel = UART_IFLS_RX1_8 | UART_IFLS_TX1_8; //TODO: change to appropriate conditionds
	myUart->IntMask = UART_IM_RXIM | UART_IM_TXIM | UART_IM_RTIM; //configure what events will generate IRQs
	
	//REENABLE UART, TX, AND RX
	myUart->UARTControl = UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN; //enable

	//SET NVIC PRIORITY FOR INTERRUPTS, THEN ENABLE. TODO: PRIORITY LEVELS (33)
  NVIC_PRI8_R |= 0x2000; //setting 13th bit to 1, level 1
	NVIC_EN1_R |= 0x00000002; //NVIC_EN1_INT33
  
	//WAIT UNTIL UART0 IS AVAILABLE
  while( !(SYSCTL_PRUART_R & SYSCTL_PRUART_R2 )){}
  
	//ADDITIONAL DELAY, JUST TO BE SAFE!
	delay = 500;
  while( delay != 0)
  {
    delay--;
  }
	
	cBufInit(&rxBuf2, BUFFERSIZE);
	cBufInit(&txBuf2, BUFFERSIZE);
}
	
void initUART5(void){
	//VARIABLES
  uint32_t delay;
  UART_PERIPH *myUart = (UART_PERIPH *)UART5;
		
	//CLOCK GATING REGISTER FOR UART0
	SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R5; 
  delay = 10000;
  while( delay != 0) delay--;
	
	//SET BAUD RATE (AND RETURN FALSE FOR INVALID BAUD RATES)
	myUart->IntegerBaudRateDiv = 43;
	myUart->FracBaudRateDiv = 26;
  
	//CONFIGURE THE LINE CONTROL FOR 8N1
  myUart->LineControl = UART_LCRH_WLEN_8 | UART_LCRH_FEN;
  
	//SET INTERRUPT LEVELS
	myUart->IntFIFOLevelSel = UART_IFLS_RX1_8 | UART_IFLS_TX1_8; //TODO: change to appropriate conditionds
	myUart->IntMask = UART_IM_RXIM | UART_IM_TXIM | UART_IM_RTIM; //configure what events will generate IRQs
	
	//REENABLE UART, TX, AND RX
	myUart->UARTControl = UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN; //enable

	//SET NVIC PRIORITY FOR INTERRUPTS, THEN ENABLE. TODO: PRIORITY LEVELS (61)
  NVIC_PRI15_R |= 0x2000; //setting 13th bit to 1, level 1
	NVIC_EN1_R |= 0x20000000;
  
	//WAIT UNTIL UART0 IS AVAILABLE
  while( !(SYSCTL_PRUART_R & SYSCTL_PRUART_R5 )){}
  
	//ADDITIONAL DELAY, JUST TO BE SAFE!
	delay = 500;
  while( delay != 0)
  {
    delay--;
  }
	
	cBufInit(&rxBuf5, BUFFERSIZE);
	cBufInit(&txBuf5, BUFFERSIZE);
}

int uart2Rx(bool block)
{
	//VARIABLES
	char data;
	
	//RX BUFFER HAS DATA IN IT
	if(cBufGetFreeCount(&rxBuf2) != BUFFERSIZE){
		DisableInterrupts();
		cBufGetChar(&rxBuf2, &data);
	  EnableInterrupts();
		return (int) data;
	}
	//RX BUFFER HAS NO DATA IN IT
	else {
		//BUSY WAIT UNTIL IT DOES..
		if(block == true){
			while(cBufGetFreeCount(&rxBuf2) == BUFFERSIZE){}
			DisableInterrupts();
			cBufGetChar(&rxBuf2, &data);
		  EnableInterrupts();
			return (int) data;
		}
		//SCREW IT, LET'S GO!
		else{
			return -1;
		}
	}
}

/****************************************************************************
 * This function accepts a single character and places it into Tx circular 
 * buffer if there is not room in the Tx hardware FIFO.
 ****************************************************************************/
void uart2Tx(int data)
{
	UART_PERIPH *myUart = (UART_PERIPH *)UART2; //get periph
	
	if((myUart->Flag & UART_FR_TXFF) == 0){//not full!
		while((myUart->Flag & UART_FR_BUSY) != 0){}//wait until UART is available to accept characters
		myUart->Data = data; //insert data into hardware tx FIFO
	}
	else{
		//
		while(cBufGetFreeCount(&txBuf2) == 0){}//wait until tx circular buffer is available to accept characters
		DisableInterrupts();
		cBufAddChar(&txBuf2, data);
	  EnableInterrupts();
	}
	myUart->IntMask |= UART_IM_TXIM; //re-enable tx interrupts
}

int uart5Rx(bool block)
{
	//VARIABLES
	char data;
	
	//RX BUFFER HAS DATA IN IT
	if(cBufGetFreeCount(&rxBuf5) != BUFFERSIZE){
		DisableInterrupts();
		cBufGetChar(&rxBuf5, &data);
	  EnableInterrupts();
		return (int) data;
	}
	//RX BUFFER HAS NO DATA IN IT
	else {
		//BUSY WAIT UNTIL IT DOES..
		if(block == true){
			while(cBufGetFreeCount(&rxBuf5) == BUFFERSIZE){}
			DisableInterrupts();
			cBufGetChar(&rxBuf5, &data);
		  EnableInterrupts();
			return (int) data;
		}
		//SCREW IT, LET'S GO!
		else{
			return -1;
		}
	}
}

/****************************************************************************
 * This function accepts a single character and places it into Tx circular 
 * buffer if there is not room in the Tx hardware FIFO.
 ****************************************************************************/
void uart5Tx(int data)
{
	UART_PERIPH *myUart = (UART_PERIPH *)UART5; //get periph
	
	if((myUart->Flag & UART_FR_TXFF) == 0){//not full!
		while((myUart->Flag & UART_FR_BUSY) != 0){}//wait until UART is available to accept characters
		myUart->Data = data; //insert data into hardware tx FIFO
	}
	else{
		//
		while(cBufGetFreeCount(&txBuf5) == 0){}//wait until tx circular buffer is available to accept characters
		DisableInterrupts();
		cBufAddChar(&txBuf5, data);
	  EnableInterrupts();
	}
	myUart->IntMask |= UART_IM_TXIM; //re-enable tx interrupts
}

void UART0IntHandler(void)
{
	
}

void UART2IntHandler(void)
{
	
}

void UART5IntHandler(void)
{
	
}

/****************************************************************************
 * This routine returns a character received from the UART.
 * If blocking is enabled, this routine should not return until data
 * is available. If blocking is disabled and no data is available,
 * this function should return 0.
 ****************************************************************************/
char uartRxPoll(uint32_t base, bool block)
{
  UART_PERIPH *myPeriph = (UART_PERIPH *)base;
  
  if (block)
  {
    while(myPeriph->Flag & UART_FR_RXFE)
    {
      // Wait
    }
     return myPeriph->Data;
  }
  else
  {
    if(myPeriph->Flag & UART_FR_RXFE)
      return 0;
    else
      return myPeriph->Data;
  }
}


/****************************************************************************
 * This routine transmits a character string out the UART 
 * Only the lower 8 bits of the 'data' variable are transmitted.
 ****************************************************************************/
void uartTxPoll(uint32_t base, char *data)
{
  UART_PERIPH *myPeriph = (UART_PERIPH *)base;
  
  if ( data != 0)
  {
    while(*data != '\0')
    {
      while( ((myPeriph->Flag)&(UART_FR_TXFF)) != 0 );
      myPeriph->Data = *data;
      data++;
    }
  }
  return;
}

/****************************************************************************
 * This routine transmits a character out the UART / COM port.
 * Only the lower 8 bits of the 'data' variable are transmitted.
 ****************************************************************************/
void uartTxCharPoll(uint32_t base, char data)
{
  UART_PERIPH *myPeriph = (UART_PERIPH *)base;
  
  while( ((myPeriph->Flag)&(UART_FR_TXFF)) != 0 );
  myPeriph->Data = data;

  return;
}
