/*
 * DAC104.h
 * Public declarations for the DAC104 SPI interface.
 *
 * Migrated November 22, 2021
 * Author: Dexter
 */


#include "Test PCB.h"

/* Defines */
#define SPI_CON1bits SPI1CON1bits
#define SPI_CLEAR_OVERFLOW SPI1_CLEAR_OVERFLOW
#define SPI_ENABLE SPI1_ENABLE
#define SPI_BUF SPI1BUFL
#define SPI_WAIT_XFER SPI1_WAIT_XFER
#define SPI_ClearInputBuffer SPI1_ClearInputBuffer
#define SPI_DISABLE SPI1_DISABLE
#define SPI_CKE_BIT SPI1_CKE_BIT
#define SPI_CKP_BIT SPI1_CKP_BIT

#define DAC_A_ADDRESS 0x1000
#define DAC_B_ADDRESS 0x5000
#define DAC_C_ADDRESS 0x9000
#define DAC_D_ADDRESS 0xD000

#define INTEGRATOR_DAC DAC_D_ADDRESS
#define TEST_DAC DAC_C_ADDRESS
#define AMPLIFIER_DAC DAC_A_ADDRESS

/* Variables */
extern unsigned int PhotonCounterBiasDAC[6];
extern unsigned int DAC_Values[4];

/* Functions */
void InitializeDAC104(void);
void SetDAC(unsigned short theDAC, unsigned short theValue);
void SetDAC_mV(unsigned short theDAC, unsigned short theValue);

