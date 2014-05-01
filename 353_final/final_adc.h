#ifndef __FINAL_ADC_H__
#define __FINAL_ADC_H__

#include <stdint.h>
#include <stdbool.h>
#include "inc/gpio.h"

#define RIGHT_POT       0
#define LEFT_POT        1

void initializeADC(void);
uint32_t GetADCval(uint32_t Channel);

#endif