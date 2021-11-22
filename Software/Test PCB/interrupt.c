/*
 * interrupt.c
 * Interrupt handlers.
 */

/*
 * Note:
 * Names for interrupt service routines are listed in:
 * Table 7-2 of the dsPIC datasheet
 */

#include "Test PCB.h"
#include "SPI.h"
#include "timers.h"
#include "UART.h"


/* ************************
 * U1 Interrupts
 */
#if U1_IN_USE
/* U1 Receive Interrupt */
void  __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) {

	*pU1ReceiveBufferWrite++ = U1RXREG;
	IFS0bits.U1RXIF = 0;
	charsInU1ReceiveBuffer++;

	/* flow control */
	if (charsInU1ReceiveBuffer > (U1_RECEIVE_BUFFER_SIZE - 8)) {
		PutU1Char(XOff);
		U1FlowOn = FALSE;
	}

	/* circularize the buffer */
	if (pU1ReceiveBufferWrite >= receiveFromU1Buffer
			+ U1_RECEIVE_BUFFER_SIZE) {
		pU1ReceiveBufferWrite = receiveFromU1Buffer;
	}
}

/* U1 Transmit Interrupt */
void  __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
	IFS0bits.U1TXIF = 0;
	if (U1TxFIFO_Count) {
		U1TxFIFO_Count--;
	}
	if ((U1TxFIFO_Count < 3) && (charsInU1TransmitBuffer)) {
		U1TXREG = *pU1TransmitBufferRead++;
		U1STAbits.UTXEN = TRUE;
		charsInU1TransmitBuffer--;
		U1TxFIFO_Count++;
	
		/* circularize the buffer */
		if (pU1TransmitBufferRead >= transmitToU1Buffer
				+ U1_TRANSMIT_BUFFER_SIZE) {
			pU1TransmitBufferRead = transmitToU1Buffer;
		}
	}
}
#endif

#if U2_IN_USE
/* *****************************
 * U2 Interrupts
 */

/* U2 Receive Interrupt */
void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void) {

	*pU2ReceiveBufferWrite++ = U2RXREG;
	IFS1bits.U2RXIF = 0;
	charsInU2ReceiveBuffer++;

	/* flow control */
	if (charsInU2ReceiveBuffer > (U2_RECEIVE_BUFFER_SIZE - 8)) {
		PutU2Char(XOff);
		U2FlowOn = FALSE;
	}

	/* circularize the buffer */
	if (pU2ReceiveBufferWrite >= receiveFromU2Buffer
			+ U2_RECEIVE_BUFFER_SIZE) {
		pU2ReceiveBufferWrite = receiveFromU2Buffer;
	}
}

/* U2 Transmit Interrupt */
void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void) {
	IFS1bits.U2TXIF = 0;
	if (U2TxFIFO_Count) {
		U2TxFIFO_Count--;
	}
	if ((U2TxFIFO_Count < 3) && charsInU2TransmitBuffer) {
		U2TXREG = *pU2TransmitBufferRead++;
		U2MODEbits.UTXEN = TRUE;
		charsInU2TransmitBuffer--;
		U2TxFIFO_Count++;
	
		/* circularize the buffer */
		if (pU2TransmitBufferRead >= transmitToU2Buffer
				+ U2_TRANSMIT_BUFFER_SIZE) {
			pU2TransmitBufferRead = transmitToU2Buffer;
		}
	}
}
#endif

/* ***********************************
 * SPI1 Interrupt 
 */
void __attribute__((interrupt, no_auto_psv)) _SPI2Interrupt(void) {

	IFS0bits.SPI1RXIF = 0;
    IFS0bits.SPI1TXIF = 0;

}

/* ***************************************
 * ADC Interrupts
 */

/* ADC AN0 Interrupt */
void __attribute__((interrupt, no_auto_psv)) _ADCAN0Interrupt(void) {
    unsigned short theShort;
    
    IFS5bits.ADCAN0IF = 0;
    
    /* Zero the integration circuit. It's low true. */
    Integrator_Zero_LatchBit = 0;
    
    /* Get the value */
    theShort = ADCBUF0;
    
    if (theShort > 2047){
        theShort = 2047;
    }
    if (MAX_SHORT == Histogram[theShort]) {
        HistogramOverflow = TRUE;
    } else {
        Histogram[theShort]++;
    }
    
    /* Release the zeroing on the integration circuit. */
    Integrator_Zero_LatchBit = 1;
    
    
    
}

/* ADC AN9 Interrupt */
void __attribute__((interrupt, no_auto_psv)) _ADCAN9Interrupt(void) {
    IFS6bits.ADCAN9IF = 0;
    IEC6bits.ADCAN9IE = 0;
    
    ADC_TemperatuerReady = TRUE;
}

/* ADC AN12 Interrupt */
void __attribute__((interrupt, no_auto_psv)) _ADCAN12Interrupt(void) {
    IFS6bits.ADCAN12IF = 0;
    IEC6bits.ADCAN12IE = 0;
    if (ADSTATLbits.AN12RDY) {
        ADSTATLbits.AN12RDY = 0;
    }
    ADC_HVReady = TRUE;
}


/* ***********************************
 * Timer1 Interrupt
 * Timer1 is the 1ms tick timer.
 */
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {

	IFS0bits.T1IF = 0;
    
    clockMilliseconds++;

    if (1000 <= clockMilliseconds) {
        clockMilliseconds = 0;
        clockSeconds++;
    }
}

/* *********************************************
 * Change Notification Register A Interrupt
 */
void __attribute__((interrupt, no_auto_psv)) _CNAInterrupt(void) {

	IFS0bits.CNAIF = 0;
}

/* *********************************************
 * Change Notification Register B Interrupt
 * These interrupts are not used.
 */
void __attribute__((interrupt, no_auto_psv)) _CNBInterrupt(void) {

	IFS0bits.CNBIF = 0;
    if (1 == Peak_Trigger_PortBit) {
        if (ADC_TriggerFromPeak) {
            /* Start Analog Out ADC */
            
        }
        
    } else if (1 == Threshold_Trigger_PortBit) {
        if (ADC_TriggerFromThreshold) {
            /* Start Analog Out ADC */
            /* This may happen through ADTRIG31 PPS input */
        }
    }
}

/* ***************************************
 * Catch these errors in debug
 */
void __attribute__((interrupt, no_auto_psv)) _U2EInterrupt(void){
	IFS3bits.U2EIF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _DefaultInterrupt(void){
#if DEBUG
	while (1) ;
#else
	asm("RESET");
#endif
}

void __attribute__((interrupt, no_auto_psv)) _OscillatorFail(void){
#if DEBUG
	while (1) ;
#else
	asm("RESET");
#endif
}

void __attribute__((interrupt, no_auto_psv)) _AddressError(void){
#if DEBUG
	while (1) ;
#else
	asm("RESET");
#endif
}

void __attribute__((interrupt, no_auto_psv)) _StackError(void){
#if DEBUG
	while (1) ;
#else
	asm("RESET");
#endif
}

void __attribute__((interrupt, no_auto_psv)) _MathError(void){
#if DEBUG
	while (1) ;
#else
	asm("RESET");
#endif
}

