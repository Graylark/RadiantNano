/*
 * MAX1932.c
 * Routines for the MAX1932 SPI interface.
 */

#include "MAX1932.h"
#include "Test PCB.h"
#include "SPI.h"

/* #defines */
#define SPI_CON1bits SPI1CON1bits
#define SPI_CLEAR_OVERFLOW SPI1_CLEAR_OVERFLOW
#define SPI_ENABLE SPI1_ENABLE
#define SPI_BUF SPI1BUFL
#define SPI_WAIT_XFER SPI1_WAIT_XFER
#define SPI_ClearInputBuffer SPI1_ClearInputBuffer
#define SPI_DISABLE SPI1_DISABLE



/*
 * Use SPI to set the High Voltage.
 * The conversion factor for DAC code from output voltage
 * DAC = 440 - (volts * 8.65)
 * 
 */
void SetHighVoltage(unsigned int centivolts) {
	unsigned char theChar;
	unsigned long theLong;
    char temp;

	if (0 == centivolts) {
		HighVoltageOn = FALSE;
		theChar = 0x00;
	} else {
		HighVoltageOn = TRUE;
        /* Limit to legal range */
		if (centivolts > HV_MAX_CENTIVOLT) {
			centivolts = HV_MAX_CENTIVOLT;
		} else if (centivolts < HV_MIN_CENTIVOLT) { 
			centivolts = HV_MIN_CENTIVOLT;
		}
        /* Convert centivolts to DAC value */
		theLong = centivolts;
		theLong *= 865;
		theLong /= 10000;
		theChar = 440 - theLong;
        if (theChar > 255) {
            theChar = 255;
        } else if (theChar < 0) {
            theChar = 1;
        }
	}

	/* Set Clock polarity */
	SPI_CON1bits.CKE = 1;
    SPI_CON1bits.CKP = 0;

	/* Clear overflow flag */
	SPI_CLEAR_OVERFLOW;

	/* Enable SPI for transfers */
	SPI_ENABLE;

	/* Set CS bit low */
	CS_HV_LatchBit = FALSE;

	/* Send Value to Output */
	SPI_BUF = theChar;

	/* wait for the command to transfer */
	SPI_WAIT_XFER;
    
   /* The transfer flag is failing, so just wait */
    for (temp=0; temp<6; temp++) {
        asm("NOP");
    }

    /* Clear input buffer */
	SPI_ClearInputBuffer();

	/* Clear the CS bit */
	CS_HV_LatchBit = TRUE;

	/* Turn SPI off */
	SPI_DISABLE;
}
