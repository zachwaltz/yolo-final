#include "final_gpio.h"

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

void initializeGpioPins(void)
{
  initPortA();
	initPortB();
	initPortC();
	initPortD();
	initPortE();
	initPortF();
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
void examineButtons(void)
{
	if(AlertDebounce){
		//SW301 (SW3)		
		if(((PortA->Data & 0x80) == 0) && !buttonPressed301) {
			count301++;
		} else if (((PortA->Data & 0x80) == 0) && buttonPressed301){
			count301 = 16; //to prevent a huge count301 variable value
		} else {
			count301 = 0;	
			buttonPressed301 = false;
		}
			
		if(count301 == 15){
			buttonPressed301 = true;
			Color = 'R'; //red
			//print the message
			uartTxPoll(UART0,"Button 301 Pressed!\n\r");
		}
		
		//SW300 (SW2)		
		if(((PortA->Data & 0x40) == 0) && !buttonPressed300) {
			count300++;
		} else if (((PortA->Data & 0x40) == 0) && buttonPressed300){
			count300 = 16; //to prevent a huge count300 variable value
		} else {
			count300 = 0;	
			buttonPressed300 = false;
		}
			
		if(count300 == 15){
			buttonPressed300 = true;
			Color = 'X'; //display off
			//print the message
			uartTxPoll(UART0,"Button 300 Pressed!\n\r");
		}
		
		//SW303 (SW5)		
		if(((PortD->Data & 0x08) == 0) && !buttonPressed303) {
			count303++;
		} else if (((PortD->Data & 0x08) == 0) && buttonPressed303){
			count303 = 16; //to prevent a huge count303 variable value
		} else {
			count303 = 0;	
			buttonPressed303 = false;
		}
			
		if(count303 == 15){
			buttonPressed303 = true;
			Color = 'G'; //green
			//print the message
			uartTxPoll(UART0,"Button 303 Pressed!\n\r");
		}

		//SW302 (SW4)		
		if(((PortD->Data & 0x04) == 0) && !buttonPressed302) {
			count302++;
		} else if (((PortD->Data & 0x04) == 0) && buttonPressed302){
			count302 = 16; //to prevent a huge count302 variable value
		} else {
			count302 = 0;	
			buttonPressed302 = false;
		}
			
		if(count302 == 15){
			buttonPressed302 = true;
			Color = 'B'; //blue
			//print the message
			uartTxPoll(UART0,"Button 302 Pressed!\n\r");
		}	
		AlertDebounce = false; //clear the alert
	}	
	else{
		return;
	}		
}

//*****************************************************************************
// The ISR sets AlertRowUpdate to true if the display should be updated.
// The routine will call getLCDRow() (led_chars.c) to determine  the 8-bit 
// data value that will be written out to PORTB.
//
// If AlertRowUpdate is false, simply return
//
//*****************************************************************************
void updateDisplay(void)
{
	uint8_t result;
	
  if (AlertRowUpdate){
		getLCDRow(RefreshRate, Row, &result); //get the data and send it to PortB. active low at this point, yo.

		PortF->Data |= ~OUTPUT_ENABLE_B; //disable all outputs |= 0x10
		
		//BLUE COLOR SELECTED
		if (Color == 'B') {
			PortC->Data = BLUE_EN; //enable BLU_EN
			PortB->Data = ~result; //turn on relevant blue (active low LEDs!)
			PortC->Data = ENABLES_OFF;
			PortC->Data = RED_EN;
			PortB->Data = result; //turn off all red
			PortC->Data = ENABLES_OFF;
			PortC->Data = GREEN_EN;
			PortB->Data = 0xF0 & result; //turn off all green
			PortC->Data = ENABLES_OFF;
		}
		
		//GREEN COLOR SELECTED
		if (Color == 'G') {
			PortC->Data = GREEN_EN;
			PortB->Data = result & 0xF0;
			PortC->Data = ENABLES_OFF;
			PortC->Data = RED_EN;
			PortB->Data = ~result;
			PortC->Data = ENABLES_OFF;
			PortC->Data = BLUE_EN;
			PortB->Data = ~result & 0x0F;
			PortC->Data = ENABLES_OFF;
		}
		
		//RED COLOR SELECTED
		if (Color == 'R') {
			PortC->Data = RED_EN;
			PortB->Data = result;
			PortC->Data = ENABLES_OFF;
			PortC->Data = BLUE_EN;
			if (Row%2 == 0)
				PortB->Data = 0xFF;
			else 
				PortB->Data = result;
			PortC->Data = ENABLES_OFF;
			PortC->Data = GREEN_EN;
			if (Row%2 == 1)
				PortB->Data = 0xFF;
			else 
				PortB->Data = result;
			PortC->Data = ENABLES_OFF;
		}
		
		//CLEAR DISPLAY SELECTED 
		if (Color == 'X') {
			PortC->Data = RED_EN;
			PortB->Data = 0xFF; //all red off
			PortC->Data = ENABLES_OFF;
			PortC->Data = BLUE_EN;
			PortB->Data = 0xFF; //all blue off
			PortC->Data = ENABLES_OFF;
			PortC->Data = GREEN_EN;
			PortB->Data = 0xFF; //all green off
			PortC->Data = ENABLES_OFF;
		}
		
		PortC->Data = ROW_EN; //enable ROW_EN
		PortB->Data = ~(1<<Row); //choose row
		PortC->Data = ENABLES_OFF; //disable ROW_EN
		
		PortF->Data = 0xEF;
		
		AlertRowUpdate = false; //clear the alert
	} else {
		return;
	}
}

void initPortA(void){
	uint32_t delay;
  
  // Turn on the Clock
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
  delay = SYSCTL_RCGCGPIO_R;
  
  // Set Digital Enable
  GPIO_PORTA_DEN_R  = 0xFF; //1111 1111
  
  // Set PA7-6 as inputs
  GPIO_PORTA_DIR_R  = 0x00; //0000 0000
  
  // set pull-ups for PA7-6
  GPIO_PORTA_PUR_R = 0xC0; //1100 0000
  
  // Set Alternate Function for PA5-0
  GPIO_PORTA_AFSEL_R = 0x3F; //0011 1111
  
  // Set Port Control Register for PA5-0
  GPIO_PORTA_PCTL_R = GPIO_PCTL_PA5_SSI0TX | GPIO_PCTL_PA4_SSI0RX | GPIO_PCTL_PA3_SSI0FSS | 
											GPIO_PCTL_PA2_SSI0CLK | GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX;
}



void initPortB(void){
	uint32_t delay;
  
  // Turn on the Clock
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
  delay = SYSCTL_RCGCGPIO_R;
  
  // Set Digital Enable
  GPIO_PORTB_DEN_R  = 0xFF;
  
  // Set PB7-0 as outputs
  GPIO_PORTB_DIR_R  = 0xFF;
}

void initPortC(void)
{
  uint32_t delay;
  
  // Turn on the Clock
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2;
  delay = SYSCTL_RCGCGPIO_R;
  
  // Set Digital Enable
  GPIO_PORTC_DEN_R  = 0xFF;
  
  // Set PC7-4 as outputs
  GPIO_PORTC_DIR_R  = 0xF0;
  
  // set pull-ups for PC3-0
  GPIO_PORTC_PUR_R = 0x0F;
  
  // Set Alternate Function for PC3-0
  GPIO_PORTC_AFSEL_R = 0x0F;
  
  // Set Port Control Register for PC3-0
  GPIO_PORTC_PCTL_R = (GPIO_PCTL_PC0_TCK | GPIO_PCTL_PC1_TMS | GPIO_PCTL_PC2_TDI |GPIO_PCTL_PC3_TDO);
}

void initPortD(void){
	uint32_t delay;
  
  // Turn on the Clock
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;
  delay = SYSCTL_RCGCGPIO_R;
  
  // Set Digital Enable
  GPIO_PORTD_DEN_R  = 0xCC; //1100 1100
  
  // Set PD3-2 as inputs
  GPIO_PORTD_DIR_R  = 0x00; //0000 0000
  
  // set pull-ups for PC3-2
  GPIO_PORTD_PUR_R = 0x0C; //0000 1100
  
  // Set Alternate Function for PD7-6
  GPIO_PORTD_AFSEL_R = 0xC0; //1100 0000
  
  // Set Port Control Register for PD7-6
  GPIO_PORTD_PCTL_R = GPIO_PCTL_PD7_U2TX | GPIO_PCTL_PD6_U2RX;
}

void initPortE(void){
	uint32_t delay;
  
  // Turn on the Clock
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
  delay = SYSCTL_RCGCGPIO_R;
  
  // Set Digital Enable
  GPIO_PORTE_DEN_R  = 0x30; //0011 0000
  
  // Set PD3-2 as inputs
  GPIO_PORTE_DIR_R  = 0x00; //0000 0000
  
  // Set Alternate Function for PD7-6
  GPIO_PORTE_AFSEL_R = 0x3C; //0011 0000
	
	GPIO_PORTE_AMSEL_R = 0x0C;
  
  // Set Port Control Register for PD7-6
  GPIO_PORTE_PCTL_R = GPIO_PCTL_PE5_U5TX | GPIO_PCTL_PE4_U5RX;  
}

void initPortF(void){
	uint32_t delay;
  
  // Turn on the Clock
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
  delay = SYSCTL_RCGCGPIO_R;
  
  // Set Digital Enable
  GPIO_PORTF_DEN_R  = 0x12;
  
  // Set PD3-2 as inputs
  GPIO_PORTF_DIR_R  = 0x10;
 
	// set pull-ups for PC3-2
  GPIO_PORTF_PUR_R = 0x0C; //0000 1100
  
  // Set Port Control Register for PD7-6
  GPIO_PORTF_PCTL_R = GPIO_PCTL_PE5_U5TX | GPIO_PCTL_PE4_U5RX;
}