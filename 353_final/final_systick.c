#include <stdbool.h>
#include "lm4f120h5qr.h"
#include "final_systick.h"
#include "inc/gpio.h"

 /****************************************************************************
 * The SysTick Handler 
 ****************************************************************************/
void SYSTICKIntHandler(void)
{
  interruptCount++; //this variable counts to 800 every second
	
	//row updates
	if ((interruptCount >= 100/RefreshRate) && (RefreshRate != 0)){ //count to the target count, then switch rows
		Row = (Row+1)%8; //wrap around once we hit row 7 ("The eight row")
		AlertRowUpdate = true; //tell foreground process to update the row
		interruptCount = 0; //reset the count and do it again!
	}
	
	//alert foreground code to interrupts
	AlertDebounce = true;
	AlertADC0 = true;
	
	//clear interrupt by reading to a dummy variable
	interruptClear = NVIC_ST_CURRENT_R;
}


/****************************************************************************
 * Initialize the SysTick timer to a given count.
 * Optionally turn on interrupts
 ****************************************************************************/
void initializeSysTick(uint32_t count, bool enableInterrupts)
{
	//disable SysTick timer
	NVIC_ST_CTRL_R = 0;
	//set reload to count-1
	NVIC_ST_RELOAD_R = count-1;
	//clear the current count
	NVIC_ST_CURRENT_R = 0;
	//enable the timer and select clock source
	NVIC_ST_CTRL_R = (NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_CLK_SRC);
	//enable interrupts
	if (enableInterrupts) NVIC_ST_CTRL_R |= NVIC_ST_CTRL_INTEN;
	
	interruptCount = 0; //start count at zero!
}