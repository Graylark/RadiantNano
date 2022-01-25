/*
 * DAC104.c
 * Routines for the DAC104 SPI interface.
 *
 * Migrated November 22, 2021
 * Author: Dexter
 */

#include "DAC104.h"
#include "Test PCB.h"
#include "SPI.h"

unsigned int DAC_Values[4];

void SendDACValue(unsigned int theValue) {
	u_int theInt;
	char temp;

	theInt.full = theValue;

	/* Set Clock polarity */
	SPI_CKE_BIT = 0;
    SPI_CKP_BIT = 0;

	/* Clear overflow flag */
	SPI_CLEAR_OVERFLOW;

	/* Enable SPI for transfers */
	SPI_ENABLE;

	/* Set CE bit low */
	CS_DAC_LatchBit = FALSE;

	/* Send Value to Output */
	temp = theInt.byte.high;
	SPI_BUF = temp;
	temp = theInt.byte.low;
	SPI_BUF = temp;

	/* wait for the command to transfer */
	SPI_WAIT_XFER;

	/* Clear input buffer */
	SPI_ClearInputBuffer();
    
    /* The transfer flag is failing, so just wait */
    for (temp=0; temp<5; temp++) {
        asm("NOP");
    }
    
        
	/* Clear the CE bit */
	CS_DAC_LatchBit = TRUE;

	/* Turn SPI off */
	SPI_DISABLE;
}

/*
 * Initialize the DAC
 * Set to Write-Through Mode
 */
void InitializeDAC104(void) {
	

    /* Set the amplifier bias to initial value */
    SetDAC(INTEGRATOR_DAC, 15);

}

/* 
 * Set a DAC output in mV.
 * The DAC reference is 3.3V and it is an 10 bit DAC.
 * The value is the output in mV.
 *		theValue = 3300 * (DAC/1023) mV
 *		DAC = (theValue * 1023) / 3300
 */
void SetDAC_mV(unsigned short theDAC, unsigned short theValue) {
	unsigned short theShort;
	unsigned long theLong;

	theLong = theValue;
    theLong *= 1023;
	theLong /= 3300;
	theShort = theLong;

	if ((theLong%10) > 4) {
		theShort++;
	}

	/* point to channel and put in value */
	theShort <<=  2;
	theShort |= theDAC;

	SendDACValue(theShort);
}


/* 
 * Set a DAC output directly.
 * The value is the DAC value.
 */
void SetDAC(unsigned short theDAC, unsigned short theValue) {
	unsigned short theShort;
    switch (theDAC) {
        case AMPLIFIER_DAC:
            DAC_Values[0] = theValue;
            break;
        case TEST_DAC:
            DAC_Values[2] = theValue;
            break;
        case INTEGRATOR_DAC:
            DAC_Values[3] = theValue;
            break;
        default:
            break;
    }
	theShort = theValue;

	/* point to channel and put in value */
	theShort <<=  2;
	theShort |= theDAC;

	SendDACValue(theShort);
}
