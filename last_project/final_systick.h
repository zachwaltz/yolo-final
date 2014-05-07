#ifndef __FINAL_SYSTICK_H__
#define __FINAL_SYSTICK_H__

#include <stdint.h>
#include <stdbool.h>

//INTERRUPT HANDLERS
void SYSTICKIntHandler(void);
void TIMERAIntHandler(void);
void UART0IntHandler(void);
void UART2IntHandler(void);
void UART5IntHandler(void);

//INIT FUNCTIONS
void initializeSysTick(uint32_t count, bool enableInterrupts);
void initializeTimerA(uint32_t count, bool enableInterrupts);
void initializeWatchdog(uint32_t count);

#endif
