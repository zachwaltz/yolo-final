#include "final_adc.h"

/****************************************************************************
 * Initialization code for the ADC
 * with SS3, Software trigger, 125KHz
 ****************************************************************************/
void initializeADC(void)
{
	uint32_t delay; //used in class code, not ours
	GPIO_PORT *PortA = (GPIO_PORT *)PORTA;

	// Enable Port E
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
	delay = SYSCTL_RCGCGPIO_R;
	
	// Set the direction as an input
	PortE->Direction = 0x00;
	// Set the alternate function (not GPIO)
	PortE->AlternateFunctionSelect = 0x0C;
	// Disable the Digital Enable
	PortE->DigitalEnable = 0x00;
	// Enable Analog
	PortE->AnalogSelectMode = 0x0C;

	// Enable ADC
	SYSCTL_RCGCADC_R |= SYSCTL_RCGCADC_R0;
	for(delay = 0; delay < 10000; delay++); //delay for clock gating register
	// (Step 1 from Data Sheet)
	// Disable Sample Sequencer #3 
	ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN3;

	// ((Step 2 from Data Sheet)
	// Configure the sample sequencer so Sample Sequencer #3 (EM3) 
	// is triggered by the processor
	ADC0_EMUX_R = ADC_EMUX_EM3_PROCESSOR;

	// (Step 3 from Data Sheet)
	// Clear the Sample Input Select for Sample Sequencer #3
	ADC0_SSMUX3_R &= ~ADC_SSMUX3_MUX0_M;

	// (Step 4 from Data Sheet)
	// Configure the Sample Sequencer #3 control register.  Make sure to set
	// the 1st Sample Interrupt Enable and the End of Sequence bits
	ADC0_SSCTL3_R = ADC_SSCTL3_END0 | ADC_SSCTL3_IE0;

	// Not shown in the data sheet.  See SAC register
	// Clear Averaging Bits
	ADC0_SAC_R = ADC_SAC_AVG_OFF;

	// Not shown in the data sheet.  See SAC register
	// Average 64 samples
	ADC0_SAC_R = ADC_SAC_AVG_64X;
	
	// Do NOT enable the Sequencer after this.  This is done in GetADCval
	
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