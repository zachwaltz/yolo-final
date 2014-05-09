#include <stdbool.h>
#include "lm4f120h5qr.h"
#include "inc/gpio.h"
#include "final_gpio.h"
#include "final_adc.h"
#include "final_systick.h"
#include "final_spi.h"
#include "final_uart.h"

//the fewest number of neighbors that kill a living cell
#define DIENUM 3
//the most number of neighbors a cell can have to survive to the next generation
#define SURVIVEMAX 3
//the least number of neighbors a cell can have to survive to the next generation
#define SURVIVEMIN 2
//the number of neighbors a dead cell must have to be revived in the next generation
#define REVIVENUM 3

#define NUMCOLORS 8;
#define ENABLES_OFF 0x00;
#define ROW_EN (1 << 7)

extern void EnableInterrupts(void);
extern void DisableInterrupts(void);

// Arrays to be used to display with LEDs
static uint8_t NEW_LED_ARRAY[8][8] = \

    {   
			1,1,1,1, 1,1,1,1, // ROW 7
			1,1,1,1, 1,1,1,1, // ROW 6			
			1,1,1,1, 1,1,1,1, // ROW 5
			1,1,1,1, 1,1,1,1, // ROW 4
			1,1,1,1, 1,1,1,1, // ROW 3
		  1,1,1,1, 1,1,1,1, // ROW 2
			1,1,1,1, 1,1,1,1, // ROW 1
			1,1,1,1, 1,1,1,1, // ROW 0	 
    };
		
static uint8_t LED_ARRAY[8][8] = \
//I have hardcoded a couple patterns from the wikipedia page, uncomment a block to see what happens
    {  
//			//Toad
// 			1,1,1,1, 1,1,1,1, // ROW 7
// 			1,1,1,1, 0,0,0,1, // ROW 6			
// 			1,0,0,0, 0,0,1,1, // ROW 5
// 			1,0,0,1, 1,1,1,1, // ROW 4
// 			1,0,1,0, 1,0,1,1, // ROW 3
// 		  1,1,1,1, 1,1,0,1, // ROW 2
// 			1,1,1,0, 1,1,1,1, // ROW 1
// 			1,1,1,1, 1,1,1,1, // ROW 0	
			
	 			//Toad
 			1,1,1,1, 1,1,1,1, // ROW 7
 			1,1,1,1, 0,0,0,1, // ROW 6			
 			1,1,1,0, 0,0,1,1, // ROW 5
 			1,1,1,1, 1,1,1,1, // ROW 4
 			1,1,1,1, 1,1,1,1, // ROW 3
 		  1,1,1,1, 1,1,1,1, // ROW 2
 			1,1,1,1, 1,1,1,1, // ROW 1
 			1,1,1,1, 1,1,1,1, // ROW 0
			
			//Beehive (still life)
// 			1,1,1,1, 1,1,1,1, // ROW 7
// 			1,1,1,0, 0,1,1,1, // ROW 6			
// 			1,1,0,1, 1,0,1,1, // ROW 5
// 			1,1,1,0, 0,1,1,1, // ROW 4
// 			1,1,1,1, 1,1,1,1, // ROW 3
// 		  1,1,1,1, 1,1,1,1, // ROW 2
// 			1,1,1,1, 1,1,1,1, // ROW 1
// 			1,1,1,1, 1,1,1,1, // ROW 0	 
				
			//Blinker
// 			1,1,1,1, 1,1,1,1, // ROW 7
// 			1,1,1,1, 1,1,1,1, // ROW 6			
// 			1,1,1,1, 1,1,1,1, // ROW 5
// 			1,1,1,0, 0,0,1,1, // ROW 4
// 			1,1,1,1, 1,1,1,1, // ROW 3
// 		  1,1,1,1, 1,1,1,1, // ROW 2
// 			1,1,1,1, 1,1,1,1, // ROW 1
// 			1,1,1,1, 1,1,1,1, // ROW 0	 
    
			//Glider
//  			1,1,1,1, 1,1,1,1, // ROW 7
//  			1,1,1,1, 0,1,1,1, // ROW 6			
//  			1,1,1,1, 1,0,1,1, // ROW 5
//  			1,1,1,0, 0,0,1,1, // ROW 4
//  			1,1,1,1, 1,1,1,1, // ROW 3
//  	  	1,1,1,1, 1,1,1,1, // ROW 2
//  			1,1,1,1, 1,1,1,1, // ROW 1
//  			1,1,1,1, 1,1,1,1, // ROW 0	 
		//Pulsar (wraps around and breaks in 8 wide array)  	
// 			  1,1,1,1, 1,1,1,1, // ROW 7
//  			1,1,0,0, 0,1,1,1, // ROW 6			
//  			0,1,1,1, 1,0,1,1, // ROW 5
//  			0,1,1,1, 1,0,1,1, // ROW 4
//  			0,1,1,1, 1,0,1,1, // ROW 3
//  		  1,1,1,1, 1,1,1,1, // ROW 2
//  			1,1,0,0, 0,1,1,1, // ROW 1
//  			1,1,1,1, 1,1,1,1, // ROW 0	 
	};

 GPIO_PORT *PortA = (GPIO_PORT *)PORTA;
 GPIO_PORT *PortB = (GPIO_PORT *)PORTB;
 GPIO_PORT *PortC = (GPIO_PORT *)PORTC;
 GPIO_PORT *PortD = (GPIO_PORT *)PORTD;
 GPIO_PORT *PortE = (GPIO_PORT *)PORTE;
 GPIO_PORT *PortF = (GPIO_PORT *)PORTF;

extern volatile bool AlertRowUpdate;
extern volatile bool AlertADC0;
extern volatile bool AlertDebounce;
extern volatile bool AlertUpdateArray;
volatile uint16_t generationRate;
extern volatile uint8_t Row;
int8_t brightness = 0;
int8_t color = 0;
char Color = 'R';

bool buttonPressed300 = false;
bool buttonPressed301 = false;
bool buttonPressed302 = false;
bool buttonPressed303 = false;
uint32_t count300 = 0;
uint32_t count301 = 0; 
uint32_t count302 = 0;
uint32_t count303 = 0;

uint8_t redarray[8];
uint8_t bluearray[8]; 
uint8_t greenarray[8];

uint32_t delay;
char data[80];

void initBoard(void)
{
	DisableInterrupts();
	
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
	//initializeSPI(SSI0, 1, 1);
	
	EnableInterrupts();
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
			//COLOR RIGHT!
			color = (color + 1) % NUMCOLORS;
			//print the message
			sprintf(data, "COLOR: %d \n\r", color);
			uartTxPoll(UART0,data);
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
			//BRIGHTNESS UP!
			if (brightness < 10)
				brightness = brightness + 1;
			else brightness = 10;
			//print the message
			sprintf(data, "BRIGHTNESS: %d \n\r", brightness);
			uartTxPoll(UART0,data);
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
			//COLOR LEFT!
			color--;
			if (color < 0)
			{
				color = 7;
			}
			//print the message
			sprintf(data, "COLOR: %d \n\r", color);
			uartTxPoll(UART0,data);
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
			//BRIGHTNESS DOWN!
			if (brightness > 0)
				brightness = brightness - 1;
			else brightness = 0;
			//print the message
			sprintf(data, "BRIGHTNESS: %d \n\r", brightness);
			uartTxPoll(UART0,data);
		}	
		AlertDebounce = false; //clear the alert
	}	
	else{
		return;
	}		
}

////*****************************************************************************
//// The ISR sets AlertRowUpdate to true if the display should be updated.
//// The routine will call getLCDRow() (led_chars.c) to determine  the 8-bit 
//// data value that will be written out to PORTB.
////
//// If AlertRowUpdate is false, simply return
////
////*****************************************************************************
void updateDisplay(void)
{
	uint8_t result;
	static uint8_t brightnesscount = 0;
	
  if (AlertRowUpdate){
		brightnesscount++;
		if (brightnesscount > brightness){
			if (brightnesscount > 10) brightnesscount = 0;
				PortF->Data |= ~OUT_EN_B; //disable all outputs |= 0x10
				PortC->Data = BLU_EN; //enable BLU_EN
				PortB->Data = 0xFF; //turn on relevant blue (active low LEDs!)
				PortC->Data = ENABLES_OFF;
				PortC->Data = RED_EN;
				PortB->Data = 0xFF; //turn off all red
				PortC->Data = ENABLES_OFF;
				PortC->Data = GRN_EN;
				PortB->Data = 0xFF; //turn off all green
				PortC->Data = ENABLES_OFF;
				PortF->Data = 0xEF;
				AlertRowUpdate = false;
			return;
		} else {
			getLCDRow(Row, &result); //get the data and send it to PortB. active low at this point, yo.

			PortF->Data |= ~OUT_EN_B; //disable all outputs |= 0x10
			
			//BLUE COLOR SELECTED
			if (color == 0) {
				PortC->Data = BLU_EN; //enable BLU_EN
				PortB->Data = result;//turn on relevant blue (active low LEDs!)
				PortC->Data = ENABLES_OFF;
				PortC->Data = RED_EN;
				PortB->Data = 0xFF; //turn off all red
				PortC->Data = ENABLES_OFF;
				PortC->Data = GRN_EN;
				PortB->Data = 0xFF; //turn off all green
				PortC->Data = ENABLES_OFF;
			}
			
			//BLUE COLOR SELECTED
			if (color == 1) {
				PortC->Data = BLU_EN; //enable BLU_EN
				PortB->Data = 0xFF; //turn on relevant blue (active low LEDs!)
				PortC->Data = ENABLES_OFF;
				PortC->Data = RED_EN;
				PortB->Data = result; //turn off all red
				PortC->Data = ENABLES_OFF;
				PortC->Data = GRN_EN;
				PortB->Data = 0xFF; //turn off all green
				PortC->Data = ENABLES_OFF;
			}
			
			//BLUE COLOR SELECTED
			if (color == 2) {
				PortC->Data = BLU_EN; //enable BLU_EN
				PortB->Data = 0xFF; //turn on relevant blue (active low LEDs!)
				PortC->Data = ENABLES_OFF;
				PortC->Data = RED_EN;
				PortB->Data = 0xFF; //turn off all red
				PortC->Data = ENABLES_OFF;
				PortC->Data = GRN_EN;
				PortB->Data = result; //turn off all green
				PortC->Data = ENABLES_OFF;
			}
			
			//BLUE COLOR SELECTED
			if (color == 3) {
				PortC->Data = BLU_EN; //enable BLU_EN
				PortB->Data = result; //turn on relevant blue (active low LEDs!)
				PortC->Data = ENABLES_OFF;
				PortC->Data = RED_EN;
				PortB->Data = result; //turn off all red
				PortC->Data = ENABLES_OFF;
				PortC->Data = GRN_EN;
				PortB->Data = 0xFF; //turn off all green
				PortC->Data = ENABLES_OFF;
			}
			
			//BLUE COLOR SELECTED
			if (color == 4) {
				PortC->Data = BLU_EN; //enable BLU_EN
				PortB->Data = 0xFF; //turn on relevant blue (active low LEDs!)
				PortC->Data = ENABLES_OFF;
				PortC->Data = RED_EN;
				PortB->Data = result; //turn off all red
				PortC->Data = ENABLES_OFF;
				PortC->Data = GRN_EN;
				PortB->Data = result; //turn off all green
				PortC->Data = ENABLES_OFF;
			}
			
			//BLUE COLOR SELECTED
			if (color == 5) {
				PortC->Data = BLU_EN; //enable BLU_EN
				PortB->Data = result; //turn on relevant blue (active low LEDs!)
				PortC->Data = ENABLES_OFF;
				PortC->Data = RED_EN;
				PortB->Data = 0xFF; //turn off all red
				PortC->Data = ENABLES_OFF;
				PortC->Data = GRN_EN;
				PortB->Data = result; //turn off all green
				PortC->Data = ENABLES_OFF;
			}
			
			//BLUE COLOR SELECTED
			if (color == 6) {
				PortC->Data = BLU_EN; //enable BLU_EN
				PortB->Data = result; //turn on relevant blue (active low LEDs!)
				PortC->Data = ENABLES_OFF;
				PortC->Data = RED_EN;
				PortB->Data = result; //turn off all red
				PortC->Data = ENABLES_OFF;
				PortC->Data = GRN_EN;
				PortB->Data = result; //turn off all green
				PortC->Data = ENABLES_OFF;
			}
			
			//BLUE COLOR SELECTED
			if (color == 7) {
				PortC->Data = BLU_EN; //enable BLU_EN
				PortB->Data = ~result; //turn on relevant blue (active low LEDs!)
				PortC->Data = ENABLES_OFF;
				PortC->Data = RED_EN;
				PortB->Data = ~result; //turn off all red
				PortC->Data = ENABLES_OFF;
				PortC->Data = GRN_EN;
				PortB->Data = ~result; //turn off all green
				PortC->Data = ENABLES_OFF;
			}
			
			PortC->Data = ROW_EN; //enable ROW_EN
			PortB->Data = ~(1<<Row); //choose row
			PortC->Data = ENABLES_OFF; //disable ROW_EN
			
			PortF->Data = 0xEF;
			
			AlertRowUpdate = false; //clear the alert
		}}
	  else {
		return;
	}
}

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
	//GPIO_PORTD_LOCK_R = 0x4C4F434B; //unlock register
}

// PORTE CONFIGURATION
// ADC1 (2), ADC0 (3), UART5_RX (4), UART5_TX (5)
void initPortE(void){  
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
  delay = SYSCTL_RCGCGPIO_R;
  
	GPIO_PORTE_LOCK_R = 0x4C4F434B; //unlock register
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

//GAME CODE!!

void getLCDRow(int8_t row, uint8_t *lcdData)
{
		uint8_t currRow;
		
		currRow =  (LED_ARRAY[row][7] << 7)|(LED_ARRAY[row][6] << 6)|(LED_ARRAY[row][5] << 5)|
		(LED_ARRAY[row][4] << 4)|(LED_ARRAY[row][3] << 3)|(LED_ARRAY[row][2] << 2)|(LED_ARRAY[row][1] << 1)|LED_ARRAY[row][0];
		*lcdData = currRow;
		return;
}

void updateGenerationRate(void)
{
	if(AlertADC0){
		generationRate = (GetADCval(1)*150/4095) + 100;
	}		
	else{
		return;
	}
}
	//*****************************************************************************
// Creates new array 
//*****************************************************************************


void updateArray(){
	if (AlertUpdateArray){
    int8_t i;
		int8_t j;
		int8_t count;
		for(i = 0; i < 8; i++){
			for(j = 0; j < 8; j++){
				count = 0;
				//count living cells (0's) surrounding current cell
				if(LED_ARRAY[wrap(i+1)][wrap2(j-1)] == 0){
					count++;
				}
				if(LED_ARRAY[wrap(i+1)][wrap2(j)] == 0){
					count++;
				}
				if(LED_ARRAY[wrap(i+1)][wrap2(j+1)] == 0){
					count++;
				}
				if(LED_ARRAY[wrap(i)][wrap2(j-1)] == 0){
					count++;
				}
				if(LED_ARRAY[wrap(i)][wrap2(j+1)] == 0){
					count++;
				}
				if(LED_ARRAY[wrap(i-1)][wrap2(j-1)] == 0){
					count++;
				}
				if(LED_ARRAY[wrap(i-1)][wrap2(j)] == 0){
					count++;
				}
				if(LED_ARRAY[wrap(i)-1][wrap2(j+1)] == 0){
					count++;
				}
				//Logic to determine what state the cell will be in based on number of neighbours
				
				//Any live cell with fewer than two live neighbours dies, as if caused by under-population.
				//Any live cell with more than three live neighbours dies, as if by overcrowding.
 				if(((count > DIENUM)) && (LED_ARRAY[i][j] == 0)){
 					NEW_LED_ARRAY[i][j] = 1;
 				}
				
				else if(((count < 2)) && (LED_ARRAY[i][j] == 0)){
 					NEW_LED_ARRAY[i][j] = 1;
 				}
 				//Any live cell with two or three live neighbours lives on to the next generation.
 				else if(((count >= SURVIVEMIN )&& (count <= SURVIVEMAX)) && (LED_ARRAY[i][j] == 0) ){
 					NEW_LED_ARRAY[i][j] = 0;
 				}
 				//Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
 				else if((count == REVIVENUM) && (LED_ARRAY[i][j] == 1) ){
 					NEW_LED_ARRAY[i][j] = 0;
 				}
				else{
					NEW_LED_ARRAY[i][j] = LED_ARRAY[i][j];
				}
			}
		}

		for(i = 0; i < 8; i++){
			for(j = 0; j < 8; j++){
				LED_ARRAY[i][j] = NEW_LED_ARRAY[i][j];
			}
		}
		AlertUpdateArray = false;
		return;
	} else {
		return;
	}
}

int wrap(int8_t num)
{
	if(num > 7){
		return num = 0;
	}
	else if( num < 0){
		return num = 7;
	}
	else{
		return num;
	}
}
//will need a separate wrap function for vert/horizontal 
//if we display across both boards at once
int wrap2(int8_t num)
{
	if(num > 7){
		return num = 0;
	}
	else if( num < 0){
		return num = 7;
	}
	else{
		return num;
	}
}
