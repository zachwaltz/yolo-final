#ifndef __FINAL_UART_H__
#define __FINAL_UART_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include "final_gpio.h"
#include "final_adc.h"
#include "final_systick.h"
#include "final_spi.h"

#define UART0 0x4000C000
#define UART1 0x4000D000
#define UART2 0x4000E000
#define UART3 0x4000F000
#define UART4 0x40010000
#define UART5 0x40011000
#define UART6 0x40012000
#define UART7 0x40013000

void initUART0(void);
void initUART2(void);
void initUART5(void);

typedef struct {
  volatile uint32_t    Data;                    // + 0x000
  volatile uint32_t    RxStatus;                // + 0x004
  volatile uint32_t    Unused0[4];          
  volatile uint32_t    Flag;                    // + 0x018
  volatile uint32_t    Unused1;          
  volatile uint32_t    IrDALowPower;            // + 0x020
  volatile uint32_t    IntegerBaudRateDiv;      // + 0x024
  volatile uint32_t    FracBaudRateDiv;         // + 0x028
  volatile uint32_t    LineControl;             // + 0x02C
  volatile uint32_t    UARTControl;             // + 0x030
  volatile uint32_t    IntFIFOLevelSel;         // + 0x034
  volatile uint32_t    IntMask;                 // + 0x038 
  volatile uint32_t    RawIntStatus;            // + 0x03C
  volatile uint32_t    MaskedIntStatus;         // + 0x040
  volatile uint32_t    IntClear;                // + 0x044
} UART_PERIPH;

#endif