#include <stdbool.h>
#include "lm4f120h5qr.h"
#include "inc/gpio.h"
#include "final_gpio.h"
#include "final_adc.h"
#include "final_systick.h"
#include "final_spi.h"
#include "final_uart.h"

//LED DISPLAY DEFIES
#define OUTPUT_ENABLE_B 0xEF
#define ENABLES_B 0x0F
#define nROW_0 ~(1 << 0)
#define RED_EN (1 << 4)
#define GREEN_EN (1 << 5)
#define BLUE_EN (1 << 6)
#define ROW_EN (1 << 7)
#define ENABLES_OFF 0x00
#define OUT_EN_B (1 << 4)

uint32_t delay;

void initBoard(void)
{
	//GPIO
  initPortA();
	initPortB();
	initPortC();
	initPortD();
	initPortE();
	initPortF();
	
	//UART
	initUART0();
	initUART2();
	initUART5();
	
	//TIMERS
	initializeTimerA(10000, true);
	initializeSysTick(80000, true);
	initializeWatchdog(80000000);
	
	//ADC
	initializeADC();
	
	//SPI
	initializeSPI(SSI0, 1, 1);
}

//*****************************************************************************
// The ISR sets AlertDebounce to true if the buttons should be examined.  
// If AlertDebounce is false, simply return
//
// If the ISR indicates that the buttons should be examined, the routine will
// debounces SW300-303.  If a button press is detected, it will print out a 
// message indicating with button was pressed.  
//
// When a button is pressed, update set Color to the appropriate value
//      SW300 – Display Off
//      SW301 – Refresh rate displayed in RED
//      SW302 – Refresh rate displayed in BLUE
//      SW303 – Refresh rate displayed in GREEN

//*****************************************************************************
//void examineButtons(void)
//{
//	if(AlertDebounce){
//		//SW301 (SW3)		
//		if(((PortA->Data & 0x80) == 0) && !buttonPressed301) {
//			count301++;
//		} else if (((PortA->Data & 0x80) == 0) && buttonPressed301){
//			count301 = 16; //to prevent a huge count301 variable value
//		} else {
//			count301 = 0;	
//			buttonPressed301 = false;
//		}
//			
//		if(count301 == 15){
//			buttonPressed301 = true;
//			Color = 'R'; //red
//			//print the message
//			uartTxPoll(UART0,"Button 301 Pressed!\n\r");
//		}
//		
//		//SW300 (SW2)		
//		if(((PortA->Data & 0x40) == 0) && !buttonPressed300) {
//			count300++;
//		} else if (((PortA->Data & 0x40) == 0) && buttonPressed300){
//			count300 = 16; //to prevent a huge count300 variable value
//		} else {
//			count300 = 0;	
//			buttonPressed300 = false;
//		}
//			
//		if(count300 == 15){
//			buttonPressed300 = true;
//			Color = 'X'; //display off
//			//print the message
//			uartTxPoll(UART0,"Button 300 Pressed!\n\r");
//		}
//		
//		//SW303 (SW5)		
//		if(((PortD->Data & 0x08) == 0) && !buttonPressed303) {
//			count303++;
//		} else if (((PortD->Data & 0x08) == 0) && buttonPressed303){
//			count303 = 16; //to prevent a huge count303 variable value
//		} else {
//			count303 = 0;	
//			buttonPressed303 = false;
//		}
//			
//		if(count303 == 15){
//			buttonPressed303 = true;
//			Color = 'G'; //green
//			//print the message
//			uartTxPoll(UART0,"Button 303 Pressed!\n\r");
//		}

//		//SW302 (SW4)		
//		if(((PortD->Data & 0x04) == 0) && !buttonPressed302) {
//			count302++;
//		} else if (((PortD->Data & 0x04) == 0) && buttonPressed302){
//			count302 = 16; //to prevent a huge count302 variable value
//		} else {
//			count302 = 0;	
//			buttonPressed302 = false;
//		}
//			
//		if(count302 == 15){
//			buttonPressed302 = true;
//			Color = 'B'; //blue
//			//print the message
//			uartTxPoll(UART0,"Button 302 Pressed!\n\r");
//		}	
//		AlertDebounce = false; //clear the alert
//	}	
//	else{
//		return;
//	}		
//}

////*****************************************************************************
//// The ISR sets AlertRowUpdate to true if the display should be updated.
//// The routine will call getLCDRow() (led_chars.c) to determine  the 8-bit 
//// data value that will be written out to PORTB.
////
//// If AlertRowUpdate is false, simply return
////
////*****************************************************************************
//void updateDisplay(void)
//{
//	uint8_t result;
//	
//  if (AlertRowUpdate){
//		getLCDRow(RefreshRate, Row, &result); //get the data and send it to PortB. active low at this point, yo.

//		PortF->Data |= ~OUTPUT_ENABLE_B; //disable all outputs |= 0x10
//		
//		//BLUE COLOR SELECTED
//		if (Color == 'B') {
//			PortC->Data = BLUE_EN; //enable BLU_EN
//			PortB->Data = ~result; //turn on relevant blue (active low LEDs!)
//			PortC->Data = ENABLES_OFF;
//			PortC->Data = RED_EN;
//			PortB->Data = result; //turn off all red
//			PortC->Data = ENABLES_OFF;
//			PortC->Data = GREEN_EN;
//			PortB->Data = 0xF0 & result; //turn off all green
//			PortC->Data = ENABLES_OFF;
//		}
//		
//		//GREEN COLOR SELECTED
//		if (Color == 'G') {
//			PortC->Data = GREEN_EN;
//			PortB->Data = result & 0xF0;
//			PortC->Data = ENABLES_OFF;
//			PortC->Data = RED_EN;
//			PortB->Data = ~result;
//			PortC->Data = ENABLES_OFF;
//			PortC->Data = BLUE_EN;
//			PortB->Data = ~result & 0x0F;
//			PortC->Data = ENABLES_OFF;
//		}
//		
//		//RED COLOR SELECTED
//		if (Color == 'R') {
//			PortC->Data = RED_EN;
//			PortB->Data = result;
//			PortC->Data = ENABLES_OFF;
//			PortC->Data = BLUE_EN;
//			if (Row%2 == 0)
//				PortB->Data = 0xFF;
//			else 
//				PortB->Data = result;
//			PortC->Data = ENABLES_OFF;
//			PortC->Data = GREEN_EN;
//			if (Row%2 == 1)
//				PortB->Data = 0xFF;
//			else 
//				PortB->Data = result;
//			PortC->Data = ENABLES_OFF;
//		}
//		
//		//CLEAR DISPLAY SELECTED 
//		if (Color == 'X') {
//			PortC->Data = RED_EN;
//			PortB->Data = 0xFF; //all red off
//			PortC->Data = ENABLES_OFF;
//			PortC->Data = BLUE_EN;
//			PortB->Data = 0xFF; //all blue off
//			PortC->Data = ENABLES_OFF;
//			PortC->Data = GREEN_EN;
//			PortB->Data = 0xFF; //all green off
//			PortC->Data = ENABLES_OFF;
//		}
//		
//		PortC->Data = ROW_EN; //enable ROW_EN
//		PortB->Data = ~(1<<Row); //choose row
//		PortC->Data = ENABLES_OFF; //disable ROW_EN
//		
//		PortF->Data = 0xEF;
//		
//		AlertRowUpdate = false; //clear the alert
//	} else {
//		return;
//	}
//}

// GPIO PORT INITILIZATION FUNCTIONS

// PORTA CONFIGURATION 
// UART0 (0-1), SPI_CLK (2), /SPI_CS (3), SPI_MISO (4), SPI_MOSI (5), SW2 (6), SW3 (7)
void initPortA(void){
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
  delay = SYSCTL_RCGCGPIO_R;
  
  GPIO_PORTA_DEN_R  = 0xFF; // Set Digital Enable
  GPIO_PORTA_DIR_R  = 0x00; // Set PA7-6 as inputs
  GPIO_PORTA_PUR_R = 0xC0; // set pull-ups for PA7-6
  GPIO_PORTA_AFSEL_R = 0x3F; // Set Alternate Function for PA5-0  
  GPIO_PORTA_PCTL_R = GPIO_PCTL_PA5_SSI0TX | GPIO_PCTL_PA4_SSI0RX | GPIO_PCTL_PA3_SSI0FSS | 
											GPIO_PCTL_PA2_SSI0CLK | GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX; // Set Port Control Register for PA5-0
}

// PORTB CONFIGURATION
// DATA (0-7)
void initPortB(void){
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
  delay = SYSCTL_RCGCGPIO_R;
  
  GPIO_PORTB_DEN_R  = 0xFF; // Set Digital Enable
  GPIO_PORTB_DIR_R  = 0xFF; // Set PB7-0 as outputs
}

// PORTC CONFIGURATION
// JTAG (0-3), RED (4), GRN (5), BLU (6), ROW (7)
void initPortC(void){
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2;
  delay = SYSCTL_RCGCGPIO_R;
  
  GPIO_PORTC_DEN_R  = 0xFF; // Set Digital Enable
  GPIO_PORTC_DIR_R  = 0xF0; // Set PC7-4 as outputs
  GPIO_PORTC_PUR_R = 0x0F; // set pull-ups for PC3-0
  GPIO_PORTC_AFSEL_R = 0x0F; // Set Alternate Function for PC3-0
  GPIO_PORTC_PCTL_R = (GPIO_PCTL_PC0_TCK | GPIO_PCTL_PC1_TMS | GPIO_PCTL_PC2_TDI |GPIO_PCTL_PC3_TDO); // Set Port Control Register for PC3-0
}

// PORTD CONFIGURATION
// SW4 (2), SW5 (3), UART2_RX (6), UART2_TX (7)
void initPortD(void){  
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;
  delay = SYSCTL_RCGCGPIO_R;
  
	GPIO_PORTD_LOCK_R = 0x4C4F434B; //unlock register
  GPIO_PORTD_DEN_R  = 0xCC; // Set Digital Enable
  GPIO_PORTD_DIR_R  = 0x00; // Set PD3-2 as inputs
  GPIO_PORTD_PUR_R = 0x0C; // set pull-ups for PC3-2
  GPIO_PORTD_AFSEL_R = 0xC0; // Set Alternate Function for PD7-6
  GPIO_PORTD_PCTL_R = GPIO_PCTL_PD7_U2TX | GPIO_PCTL_PD6_U2RX; // Set Port Control Register for PD7-6
	GPIO_PORTD_LOCK_R = 0x4C4F434B; //unlock register
}

// PORTE CONFIGURATION
// ADC1 (2), ADC0 (3), UART5_RX (4), UART5_TX (5)
void initPortE(void){  
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
  delay = SYSCTL_RCGCGPIO_R;
  
  GPIO_PORTE_DEN_R  = 0x30; // Set Digital Enable
  GPIO_PORTE_DIR_R  = 0x00; // Set PD3-2 as inputs
  GPIO_PORTE_AFSEL_R = 0x3C; // Set Alternate Function for PD7-6
	GPIO_PORTE_AMSEL_R = 0x0C;
  GPIO_PORTE_PCTL_R = GPIO_PCTL_PE5_U5TX | GPIO_PCTL_PE4_U5RX; // Set Port Control Register for PD7-6 
}

// PORTF CONFIGURATION
// SW2 (1), /OE (4)
void initPortF(void){
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTF_LOCK_R = 0x4C4F434B; //unlock register
  GPIO_PORTF_DEN_R  = 0x12; // Set Digital Enable
  GPIO_PORTF_DIR_R  = 0x10; // Set PF1 as input, PF2 as output
  GPIO_PORTF_PUR_R = 0x02; // set pull-ups for PF1
	GPIO_PORTF_LOCK_R = 0x00000000; //relock register
}