#include "final_spi.h"

// *******************************************
// Configure SPI
// *******************************************
void initializeSPI( uint32_t base, uint8_t phase, uint8_t polarity)
{
  uint32_t delay;
  SPI_PERIPH *myPeriph = (SPI_PERIPH *)base;

  // Turn on the Clock Gating Register
  SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R0;
  delay = SYSCTL_RCGCSSI_R;

  // Disable the SSI interface
  myPeriph->SSICR1 &= ~SSI_CR1_SSE  ;                   // ###04###

  // Enable Master Mode (0 is master)
  myPeriph->SSICR1 &= ~SSI_CR1_MS;                   // ###05###
  
  // Assume that we hvae a 80MHz clock and want a 2MHz SPI clock
  // FSSIClk = FSysClk / (CPSDVSR * (1 + SCR));
	// 2* 10^6  = (80 * 10^6 )/(CPSDVSR * (1+ SCR));
  myPeriph->SSICPSR = 0x08;                  // ###06###
  myPeriph->SSICR0  = (0x04<<8);             // ###07###
  
  // Clear the phse and polarity bits
  myPeriph->SSICR0  &=  ~(SSI_CR0_SPH | SSI_CR0_SPO);
  
  if (phase == 1)
      myPeriph->SSICR0  |= SSI_CR0_SPH;
  
  if (polarity ==1)
      myPeriph->SSICR0  |= SSI_CR0_SPO;

  // Freescale SPI Mode with 8-Bit data (See line 2226 of lm4f120h5qr.h)
  myPeriph->SSICR0  |= SSI_CR0_DSS_8;  // ###08###
	myPeriph->SSICR0 &= ~(SSI_CR0_FRF_M); //clear frame select bits
	myPeriph->SSICR0 |= SSI_CR0_FRF_MOTO; //set Freescale mode

  //Enable SSI
  myPeriph->SSICR1 |= SSI_CR1_SSE;                   // ###09###
}