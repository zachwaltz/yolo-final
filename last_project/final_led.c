#include "final_gpio.h"
#include "final_led.h"

extern volatile uint8_t count;

extern GPIO_PORT *PortA;
extern GPIO_PORT *PortB;
extern GPIO_PORT *PortC;
extern GPIO_PORT *PortD;
extern GPIO_PORT *PortE;
extern GPIO_PORT *PortF;

#define ENABLES_OFF 0x00;
#define ROW_EN (1 << 7)

extern volatile bool AlertRowUpdate;
extern volatile bool AlertADC0;
extern volatile bool AlertDebounce;
extern volatile bool AlertUpdateArray;
extern volatile uint8_t Row;


// Look up table for 
static const uint8_t LED_LUT[16][8] = \
{
    {   // Zero
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON &            LED_0_ON, // ROW 1
        LED_2_ON &            LED_0_ON, // ROW 2
        LED_2_ON &            LED_0_ON, // ROW 3
        LED_2_ON &            LED_0_ON, // ROW 4
        LED_2_ON &            LED_0_ON, // ROW 5
        LED_2_ON &            LED_0_ON, // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   // One
                              LED_0_ON, // ROW 0
                              LED_0_ON, // ROW 1
                              LED_0_ON, // ROW 2
                              LED_0_ON, // ROW 3
                              LED_0_ON, // ROW 4
                              LED_0_ON, // ROW 5
                              LED_0_ON, // ROW 6
                              LED_0_ON, // ROW 7
    },
    {   // Two
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON                      , // ROW 1
        LED_2_ON                      , // ROW 2
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 3
                              LED_0_ON, // ROW 4
                              LED_0_ON, // ROW 5
                              LED_0_ON, // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   // Three
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
                              LED_0_ON, // ROW 1
                              LED_0_ON, // ROW 2
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 3
                              LED_0_ON, // ROW 4
                              LED_0_ON, // ROW 5
                              LED_0_ON, // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   // Four
                              LED_0_ON, // ROW 0
                              LED_0_ON, // ROW 1
                              LED_0_ON, // ROW 2
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 3
        LED_2_ON &            LED_0_ON, // ROW 4
        LED_2_ON &            LED_0_ON, // ROW 5
        LED_2_ON &            LED_0_ON, // ROW 6
        LED_2_ON &            LED_0_ON, // ROW 7
    },
    {   // Five
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
                              LED_0_ON, // ROW 1
                              LED_0_ON, // ROW 2
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 3
        LED_2_ON                      , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   // Six
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON &            LED_0_ON, // ROW 1
        LED_2_ON &            LED_0_ON, // ROW 2
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 3
        LED_2_ON                      , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
     {   // Seven
                              LED_0_ON, // ROW 0
                              LED_0_ON, // ROW 1
                              LED_0_ON, // ROW 2
                              LED_0_ON, // ROW 3
                              LED_0_ON, // ROW 4
                              LED_0_ON, // ROW 5
                              LED_0_ON, // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
     },
    {   //  Eight
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON &            LED_0_ON, // ROW 1
        LED_2_ON &            LED_0_ON, // ROW 2
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 3
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 4
        LED_2_ON &            LED_0_ON, // ROW 5
        LED_2_ON &            LED_0_ON, // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   //  Nine
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
                              LED_0_ON, // ROW 1
                              LED_0_ON, // ROW 2
                              LED_0_ON, // ROW 3
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 4
        LED_2_ON &            LED_0_ON, // ROW 5
        LED_2_ON &            LED_0_ON, // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   //  10
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON                      , // ROW 1
        LED_2_ON                      , // ROW 2
        LED_2_ON & LED_1_ON           , // ROW 3
        LED_2_ON & LED_1_ON           , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   //  11
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON                      , // ROW 1
        LED_2_ON                      , // ROW 2
        LED_2_ON & LED_1_ON           , // ROW 3
        LED_2_ON & LED_1_ON           , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   //  12
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON                      , // ROW 1
        LED_2_ON                      , // ROW 2
        LED_2_ON & LED_1_ON           , // ROW 3
        LED_2_ON & LED_1_ON           , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   //  13
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON                      , // ROW 1
        LED_2_ON                      , // ROW 2
        LED_2_ON & LED_1_ON           , // ROW 3
        LED_2_ON & LED_1_ON           , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   //  14
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON                      , // ROW 1
        LED_2_ON                      , // ROW 2
        LED_2_ON & LED_1_ON           , // ROW 3
        LED_2_ON & LED_1_ON           , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   //  15
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON                      , // ROW 1
        LED_2_ON                      , // ROW 2
        LED_2_ON & LED_1_ON           , // ROW 3
        LED_2_ON & LED_1_ON           , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
};

//*****************************************************************************
// Convert a decimal number to BCD.  GIVEN TO US BY INSTRUCTOR!
//*****************************************************************************
static bool decimalToBcd(int8_t decimal, uint8_t *tens, uint8_t *ones)
{
    char msg[2];
    if(decimal > 99)
        return false;
    
    sprintf(msg, "%02d", decimal);
		
    *tens = msg[0] - 0x30;
    *ones = msg[1] - 0x30;
    
    return true;
}

//*****************************************************************************
// Returns 8-bit data written to the active row.
// Arguments
//    hertz:    current refresh rate
//    row:      active row
//    lcdData:  data to write out to the latches.
// WRITTEN BY US!
//*****************************************************************************
bool getLCDRowCount(int8_t count, int8_t row, uint8_t *lcdData)
{
	uint8_t ones, tens;
	if(decimalToBcd((count&0x00FF), &tens, &ones)){ //convert
		ones = LED_LUT[ones][row]; //find the current row to display for the ones digit
		tens = LED_LUT[tens][row]; //same for the tens digit
		tens = tens << 4; //shift the tens digit to the left side of the display (over 4 spots)
		ones &= 0x0F; //clear the top bits of the ones digit so they don't interfere with the tens digit
		*lcdData = tens | ones; //piece it together - voila!
		return true;
	}
	return false;
}

//*****************************************************************************
// The ISR sets AlertRowUpdate to true if the display should be updated.
// The routine will call getLCDRow() (led_chars.c) to determine  the 8-bit 
// data value that will be written out to PORTB.
//
// If AlertRowUpdate is false, simply return
//
//*****************************************************************************
void updateCount(void)
{
	uint8_t result;
	static uint32_t counting = 0;
	
  if (AlertRowUpdate){
		getLCDRowCount(count, Row, &result); //get the data and send it to PortB. active low at this point, yo.

		PortF->Data |= ~OUT_EN_B; //disable all outputs |= 0x10
		
		PortC->Data = BLU_EN; //enable BLU_EN
		PortB->Data = result; //turn on relevant blue (active low LEDs!)
		PortC->Data = ENABLES_OFF;
		PortC->Data = RED_EN;
		PortB->Data = 0xFF; //turn off all red
		PortC->Data = ENABLES_OFF;
		PortC->Data = GRN_EN;
		PortB->Data = 0xFF; //turn off all green
		PortC->Data = ENABLES_OFF;
		
		PortC->Data = ROW_EN; //enable ROW_EN
		PortB->Data = ~(1<<Row); //choose row
		PortC->Data = ENABLES_OFF; //disable ROW_EN
		
		PortF->Data = 0xEF;
		
		AlertRowUpdate = false; //clear the alert
	} else {
		return;
	}
}