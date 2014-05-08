#ifndef __FINAL_SYSTICK_H__
#define __FINAL_SYSTICK_H__

#include <stdint.h>
#include <stdbool.h>

//INTERRUPT HANDLERS
void SYSTICKIntHandler(void);
void TIMERAIntHandler(void);

//INIT FUNCTIONS
void initializeSysTick(uint32_t count, bool enableInterrupts);
void initializeTimerA(uint32_t count, bool enableInterrupts);
void initializeWatchdog(uint32_t count);

#endif
