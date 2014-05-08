#ifndef __FINAL_SPI_H__
#define __FINAL_SPI_H__

#include <stdint.h>
#include <stdbool.h>

#define SSI0 0x40008000
#define SSI1 0x40009000
#define SSI2 0x4000A000
#define SSI3 0x4000B000

//SPI FUNCTIONS
void initializeSPI( uint32_t base, uint8_t phase, uint8_t polarity);

typedef struct {
  volatile uint32_t    SSICR0;        // + 0x000
  volatile uint32_t    SSICR1;        // + 0x004          
  volatile uint32_t    SSIDR;         // + 0x008
  volatile uint32_t    SSISR;         // + 0x00C
  volatile uint32_t    SSICPSR;       // + 0x010
  volatile uint32_t    SSIIM;         // + 0x014
  volatile uint32_t    SSIRIS;        // + 0x018
  volatile uint32_t    SSIMIS;        // + 0x01C
  volatile uint32_t    SSIICR;        // + 0x020
  volatile uint32_t    SSIDMACTL;     // + 0x024
} SPI_PERIPH; 

#endif
