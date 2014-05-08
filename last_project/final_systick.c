#include <stdbool.h>
#include "lm4f120h5qr.h"
#include "final_systick.h"
#include "inc/gpio.h"

extern bool alertRowUpdate;
extern bool alertADC0;
extern bool alertDebounce;

uint32_t sysCount;
uint32_t aCount;
uint32_t interruptClear; //dummy variable used to clear timers

 /****************************************************************************
 * The SysTick Handler 
 ****************************************************************************/
void SYSTICKIntHandler(void)
{
//  interruptCount++; //this variable counts to 800 every second
//	
//	//row updates
//	if ((interruptCount >= 100/RefreshRate) && (RefreshRate != 0)){ //count to the target count, then switch rows
//		Row = (Row+1)%8; //wrap around once we hit row 7 ("The eight row")
//		AlertRowUpdate = true; //tell foreground process to update the row
//		interruptCount = 0; //reset the count and do it again!
//	}
//	
//	//alert foreground code to interrupts
//	AlertDebounce = true;
//	AlertADC0 = true;
//	
//	//clear interrupt by reading to a dummy variable
	interruptClear = NVIC_ST_CURRENT_R;
}

void TIMERAIntHandler(void)
{
	TIMER0_ICR_R |= TIMER_IMR_TATOIM;
}

/****************************************************************************
 * Initialize the SysTick timer to a given count.
 * Optionally turn on interrupts
 ****************************************************************************/
void initializeSysTick(uint32_t count, bool enableInterrupts)
{
	NVIC_ST_CTRL_R = 0; //disable SysTick timer
	NVIC_ST_RELOAD_R = count-1; //set reload to count-1
	NVIC_ST_CURRENT_R = 0; //clear the current count
	NVIC_ST_CTRL_R = (NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_CLK_SRC); //enable the timer and select clock source
	if (enableInterrupts) NVIC_ST_CTRL_R |= NVIC_ST_CTRL_INTEN; //enable interrupts
	
	sysCount = 0; //start count at zero!
}


void initializeTimerA(uint32_t count, bool enableInterrupts)
{
	uint32_t delay;
	
	//ENABLE CLOCK
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0; //enable clock
	delay = 10000;
  while( delay != 0) delay--;
	
	//DISABLE TIMER AND CLEAR CONFIGURATION
	TIMER0_CTL_R &= ~TIMER_CTL_TAEN; //disable
	TIMER0_CFG_R = 0; //clear superfluous options
	
	//SET AS PERIODIC
	TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD; //set as periodic timer
	
	//LOAD COUNT, ENABLE INTERRUPTS
	TIMER0_TAILR_R = count;
	TIMER0_IMR_R |= TIMER_IMR_TATOIM;
	
	if(enableInterrupts) TIMER0_TAMR_R |= TIMER_TAMR_TAMIE;
	
	//ENABLE TIMER A AND INTERRUPTS IN NVIC
	TIMER0_CTL_R |= TIMER_CTL_TAEN; //enable timer a
	NVIC_EN0_R |= NVIC_EN0_INT19; //enable interrupt in NVIC
	
	//START ACOUNT AT 0
	aCount = 0;
}


void initializeWatchdog(uint32_t count)
{
	uint32_t delay;
	
	SYSCTL_RCGCWD_R |= SYSCTL_RCGCWD_R0; //enable clock for peripheral
	delay = 10000;
  while( delay != 0) delay--;
	
	WATCHDOG0_LOCK_R = 0x1ACCE551; //unlock watchdog timer
	WATCHDOG0_LOAD_R = count; //load the register with desired timer load value
	WATCHDOG0_CTL_R |= WDT_CTL_INTEN | WDT_CTL_RESEN; //configure to trigger system resets, set RESEN
	//set INTEN bit to enable watchdog, enable interrupts, and lock the control register
	
	//TO CLEAR
	//write any value to the WDTICR register
}
