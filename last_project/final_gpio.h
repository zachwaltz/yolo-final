#ifndef __FINAL_GPIO_H__
#define __FINAL_GPIO_H__

#include <stdint.h>
#include <stdbool.h>
#include "inc/gpio.h"
#include "final_gpio.h"
#include "final_adc.h"
#include "final_systick.h"
#include "final_spi.h"
#include "final_uart.h"

//GPIO FUNCTIONS
void initBoard(void);
void initPortA(void);
void initPortB(void);
void initPortC(void);
void initPortD(void);
void initPortE(void);
void initPortF(void);

void updateDisplay(void);
void updateGenerationRate(void);
void getLCDRow(int8_t row, uint8_t *lcdData);
void examineButtons(void);
void updateArray(void);
int wrap(int8_t num);
int wrap2(int8_t num);

#endif
