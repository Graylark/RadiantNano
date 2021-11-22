/*
 * SPI.c
 * Interface to dsPIC SPI module
 */


#include "Test PCB.h"
#include "SPI.h"

/*
 * Initialize the SPI module.
 * Using SPI1 for HV.
 */
void InitializeSPI(void) {
	/* Set up the SPI */
	/* SPI pin assignments */
	#ifdef IO_PIN_SELECT
	SPI1_MOSI_RPOR = SPI1_MOSI_Code;
	SPI1_SCK_RPOR = SPI1_SCK_Code;
	#endif

	/* The PIC is the SPI master */
	SPI1CON1Lbits.MSTEN = 1;
    
    /* The SDI pin is not used */
    SPI1CON1Lbits.DISSDI = 1;
    
    /* Set clock to 1MHz, since the MAX1932 top  speed is 2MHz */
    /* Baud rate = Fp / (2 * (SPIxBRG + 1)) */
    SPI1BRGL = 49;

	/* Use the enhanced mode with buffered data */
	SPI1CON1Lbits.ENHBUF = 1;

	/* Change data on rising edge */
	/* Clock data on falling edge */
	SPI1CON1bits.CKP = 0;
	SPI1CON1bits.CKE = 0;

}

/*
 * Clear the SPI1 input buffer
 */
void SPI1_ClearInputBuffer(void) {
	char temp;

	while(!SPI1STATLbits.SPIRBE) {
		temp = SPI1BUFL;
	}
}





