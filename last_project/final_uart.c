#include "inc/final_uart.h"
#include "inc/final_gpio.h"

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
  delay = SYSCTL_RCGCUART_R;
	
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
  while( delay != 0)
  {
    delay--;
  }
}

void initUART2(void){
	//VARIABLES
  uint32_t delay;
  UART_PERIPH *myUart = (UART_PERIPH *)UART2;
		
	//CLOCK GATING REGISTER FOR UART0
	SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R2; 
  delay = SYSCTL_RCGCUART_R;
	
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
	NVIC_EN1_R |= NVIC_EN1_INT33;
  
	//WAIT UNTIL UART0 IS AVAILABLE
  while( !(SYSCTL_PRUART_R & SYSCTL_PRUART_R2 )){}
  
	//ADDITIONAL DELAY, JUST TO BE SAFE!
	delay = 500;
  while( delay != 0)
  {
    delay--;
  }
}
	
void initUART5(void){
	//VARIABLES
  uint32_t delay;
  UART_PERIPH *myUart = (UART_PERIPH *)UART5;
		
	//CLOCK GATING REGISTER FOR UART0
	SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R5; 
  delay = SYSCTL_RCGCUART_R;
	
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
	NVIC_EN1_R |= NVIC_EN1_INT61;
  
	//WAIT UNTIL UART0 IS AVAILABLE
  while( !(SYSCTL_PRUART_R & SYSCTL_PRUART_R5 )){}
  
	//ADDITIONAL DELAY, JUST TO BE SAFE!
	delay = 500;
  while( delay != 0)
  {
    delay--;
  }
}




