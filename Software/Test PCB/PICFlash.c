/*
 * PICFlash.c
 * Interface to PIC internal flash memory.
 */


#include "Test PCB.h"
#include "DAC104.h"
#include "UART.h"
#include "libpic30.h"

/* Set the Settings data high in flash memory. */
#define SETTINGS_FLASH_ADDRESS 0x0F000
#define FLASH_WRITE_LATCH_PAGE 0x00FA
#define SETTINGS_FLASH_PAGE 0x0000
#define SETTINGS_RECORD_SIZE 18
#define SETTINGS_RECORD_INTS 9
#define SETTINGS_RECORD_WRITES 5

typedef struct {
	union {
		struct{
            unsigned short ADC_TriggerFromThreshold;
            unsigned short ADC_TriggerFromDelay;
            unsigned short ADC_TriggerDelayCount;
			unsigned short HVCentivolts;
            unsigned short DAC_Centivolts;
			unsigned short BaudRateDivisor;
			unsigned short UART2BRGH;
            unsigned short AmplifierDAC;
            unsigned short IntegratorDAC;
		};
		struct{
			unsigned char Data[SETTINGS_RECORD_SIZE];
		};
	};
} SettingsRecordType;


/* ************************************************************
 * The user variables are stored in one 1024 instruction page,
 * so for now we use only the low word of flash to simplify
 * the code. With three bytes per instruction, we are using
 * 3072 bytes.
 */
void SaveSettingsToFlash(void) {
	SettingsRecordType SettingsRecord;
	SettingsRecordType * pSettingsRecord;
	unsigned char * pData;
	unsigned int writeAddress;
	u_int writeData1;
	u_int writeData2;
	int TABLPAG_Save;
	unsigned short index;

	/* Set up */
	pSettingsRecord = &SettingsRecord;

	/* Store all of the Settings values in one convenient place */
    pSettingsRecord->ADC_TriggerFromThreshold = ADC_TriggerFromThreshold;
    pSettingsRecord->ADC_TriggerFromDelay = ADC_TriggerFromDelay;
    pSettingsRecord->ADC_TriggerDelayCount = ADC_TriggerDelayCount;
    pSettingsRecord->HVCentivolts = HighVoltageCentivolts;
    pSettingsRecord->DAC_Centivolts = DAC_Centivolts;
	pSettingsRecord->BaudRateDivisor = BaudRateDivisor;
	pSettingsRecord->UART2BRGH = UART2BRGH;
    pSettingsRecord->AmplifierDAC = AmplifierDAC;
    pSettingsRecord->IntegratorDAC = IntegratorDAC;


	/* Set the PIC flash page */
	TABLPAG_Save = TBLPAG;
	TBLPAG = SETTINGS_FLASH_PAGE;

	/* Erase the flash page 
	 * Might be better to use:
	 * _erase_flash(_prog_addressT dst);
	 */

	/* Set address */
	NVMADRU = SETTINGS_FLASH_PAGE;
	NVMADR = SETTINGS_FLASH_ADDRESS;
	/* Set command word */
	NVMCON = 0x4003;

	asm("DISI	#06");

	/* Start up erase */
	/* __builtin_write_NVM(); */
	asm("MOV #0x55, W0");
	asm("MOV W0, NVMKEY");
	asm("MOV #0xAA, W0");
	asm("MOV W0, NVMKEY");
	asm("BSET NVMCON, #15");
	asm("NOP");
	asm("NOP");

	/* Wait for erase to complete */
	while (1 == NVMCONbits.WR);

	/* Set up to write */
	/* Point to flash write latches */
	TBLPAG = FLASH_WRITE_LATCH_PAGE;
	/* Point to source data */
	pData = (pSettingsRecord->Data);
	/* Point to destination in flash */
	writeAddress = SETTINGS_FLASH_ADDRESS;
	NVMADR = writeAddress;
	NVMADRU = SETTINGS_FLASH_PAGE;

	/* Write the data to flash */
	for (index=0; index<SETTINGS_RECORD_WRITES; index++) {
		/* Load the flash write registers */
		writeData1.byte.low =  *pData++;
		writeData1.byte.high = *pData++;
        __builtin_tblwtl(0, writeData1.full);
		writeData2.byte.low =  *pData++;
		writeData2.byte.high = *pData++;
        __builtin_tblwtl(2, writeData2.full);
        
		/* _write_flash_word32(0, writeData1.full, writeData2.full); */
		
		 /* Set command word */
		NVMCON = 0x4001;

		/* Disable interrupts for unlock sequence. */
		 
		asm("DISI #06");

		/* Start the write */
		__builtin_write_NVM(); 
        /*
		asm("MOV #0x55, W0");
		asm("MOV W0, NVMKEY");
		asm("MOV #0xAA, W0");
		asm("MOV W0, NVMKEY");
		asm("BSET NVMCON, #15");
        */
		asm("NOP");
		asm("NOP");
		
		
		/* Wait for write */
		while (1 == NVMCONbits.WR);

		/* Increment destination write pointer */
		writeAddress += 4;
		NVMADRU = 0x0000;
		NVMADR = writeAddress;
	}

	/* Clean up */
	NVMCON = 0x0000;
	TBLPAG = TABLPAG_Save;
}

Bool ReadSettingsFromFlash(void) {
	SettingsRecordType SettingsRecord;
	SettingsRecordType * pSettingsRecord;
	unsigned char * pData;
	unsigned int readAddress;
	u_int readData;
	unsigned int data;
	int TABLPAG_Save;
	Bool theReturn=FALSE;
	unsigned short index;

	/* Set up */
	pSettingsRecord = &SettingsRecord;
	pData = (pSettingsRecord->Data);

	/* Set the PIC flash page */
	TABLPAG_Save = TBLPAG;
	TBLPAG = SETTINGS_FLASH_PAGE;

	readAddress = SETTINGS_FLASH_ADDRESS;
	for (index=0; index<SETTINGS_RECORD_INTS; index++) {
		NVMADR = readAddress;
		 data = __builtin_tblrdl(readAddress); 
		readData.full  = data;
		*pData++ = readData.byte.low;
		*pData++ = readData.byte.high;
		readAddress += 2;
	}

	/* Test whether the data is valid */
	if (0xffff != pSettingsRecord->ADC_TriggerFromThreshold) {
		/* Read all of the Settings values into their locations */
        
        ADC_TriggerFromThreshold = pSettingsRecord->ADC_TriggerFromThreshold;
        ADC_TriggerFromDelay = pSettingsRecord->ADC_TriggerFromDelay;
        ADC_TriggerDelayCount = pSettingsRecord->ADC_TriggerDelayCount;
        HighVoltageCentivolts = pSettingsRecord->HVCentivolts;
        DAC_Centivolts = pSettingsRecord->DAC_Centivolts;
		BaudRateDivisor = pSettingsRecord->BaudRateDivisor;
		UART2BRGH = pSettingsRecord->UART2BRGH;
        AmplifierDAC = pSettingsRecord->AmplifierDAC;
        SetDAC(AMPLIFIER_DAC, AmplifierDAC);
        IntegratorDAC = pSettingsRecord->IntegratorDAC;
        SetDAC(INTEGRATOR_DAC, IntegratorDAC);
		theReturn = TRUE;
	}

	TBLPAG = TABLPAG_Save;
	return theReturn;
}

