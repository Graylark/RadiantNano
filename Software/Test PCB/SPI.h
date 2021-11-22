/*
 * SPI.h
 * Public declarations for SPI.c
 */

#include "Test PCB.h"


 /* Define some macros to standardize interface */
#define SPI1_ENABLE 	SPI1CON1Lbits.SPIEN = 1
#define SPI1_DISABLE SPI1CON1Lbits.SPIEN = 0
#define SPI1_CLEAR_OVERFLOW  	SPI1STATLbits.SPIROV = 0
#define SPI1_WAIT_XFER 	while (SPI1STATLbits.SPIBUSY) {} /* while (!SPI1STATbits.SRMPT) {} */
#define SPI_XMIT_OKAY	while (SPI1STATLbits.SPITBF) {}
#define SPI1_CKE_BIT SPI1CON1bits.CKE
#define SPI1_CKP_BIT SPI1CON1bits.CKP


extern void InitializeSPI(void);
extern void SPI1_ClearInputBuffer(void);
extern void StartSPI(void);
extern void PutSPI_Byte(unsigned char theChar);
extern void PutSPI_Data(unsigned char * theString, unsigned int count, unsigned char * theMask);
extern void StopSPI(void);

