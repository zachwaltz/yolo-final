#include "final_adc.h"

/****************************************************************************
 * Initialization code for the ADC
 * with SS3, Software trigger, 125KHz
 ****************************************************************************/
void initializeADC(void)
{
	uint32_t delay; //dumym variable
	
	SYSCTL_RCGCADC_R |= SYSCTL_RCGCADC_R0; // Enable ADC
	for(delay = 0; delay < 10000; delay++); //delay for clock gating register
	
	ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN3; // Disable Sample Sequencer #3 
	ADC0_EMUX_R = ADC_EMUX_EM3_PROCESSOR; // Configure the sample sequencer so Sample Sequencer #3 (EM3) is triggered by the processor
	ADC0_SSMUX3_R &= ~ADC_SSMUX3_MUX0_M; 	// Clear the Sample Input Select for Sample Sequencer #3
	ADC0_SSCTL3_R = ADC_SSCTL3_END0 | ADC_SSCTL3_IE0; // Configure the Sample Sequencer #3 control register.  Make sure to set the 1st Sample Interrupt Enable and the End of Sequence bits
	ADC0_SAC_R = ADC_SAC_AVG_OFF; // Clear Averaging Bits
	ADC0_SAC_R = ADC_SAC_AVG_64X; // Average 64 samples
	
}

//*****************************************************************************
// Get the ADC value of a given ADC Channel. GIVEN BY INSTRUCTORS!
//*****************************************************************************
uint32_t GetADCval(uint32_t Channel)
{
	uint32_t result;

  ADC0_SSMUX3_R = Channel;      // Set the channel
  ADC0_ACTSS_R  |= ADC_ACTSS_ASEN3; // Enable SS3
  ADC0_PSSI_R = ADC_PSSI_SS3;     // Initiate SS3

  while(0 == (ADC0_RIS_R & ADC_RIS_INR3)); // Wait for END of conversion
  result = ADC0_SSFIFO3_R & 0x0FFF;     // Read the 12-bit ADC result
  ADC0_ISC_R = ADC_ISC_IN3;         // Acknowledge completion

  return result;
}