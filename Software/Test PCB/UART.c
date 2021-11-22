/*
 * UART.c
 * Routines to handle getting and putting char to the UARTs.
 */

/*
 * UART 1 is unused
 * UART 2 connects to USB and debug port
 * UART # is unused
 */



#include "Test PCB.h"
#include "timers.h"
#include "UART.h"

/* Variables */
unsigned char UART2BRGH;

/*
 * Buffers, pointers, and counters for the UARTs
 *
 * #define U1_RECEIVE_BUFFER_SIZE 64
 * #define U1_TRANSMIT_BUFFER_SIZE 64
 * #define U2_RECEIVE_BUFFER_SIZE 0
 * #define U2_TRANSMIT_BUFFER_SIZE 0
 */
#if U1_IN_USE
char receiveFromU1Buffer[U1_RECEIVE_BUFFER_SIZE];
char transmitToU1Buffer[U1_TRANSMIT_BUFFER_SIZE];
char * pU1ReceiveBufferRead;
char * pU1ReceiveBufferWrite;
char * pU1TransmitBufferRead;
char * pU1TransmitBufferWrite;
volatile int charsInU1ReceiveBuffer;
volatile int charsInU1TransmitBuffer;
int unsigned U1TxFIFO_Count;
volatile Bool U1FlowOn;
#endif

#if U2_IN_USE
char receiveFromU2Buffer[U2_RECEIVE_BUFFER_SIZE];
char transmitToU2Buffer[U2_TRANSMIT_BUFFER_SIZE];
char * pU2ReceiveBufferRead;
char * pU2ReceiveBufferWrite;
char * pU2TransmitBufferRead;
char * pU2TransmitBufferWrite;
volatile int charsInU2ReceiveBuffer;
volatile int charsInU2TransmitBuffer;
int unsigned U2TxFIFO_Count;
volatile Bool U2FlowOn;
#endif

/* *************************************************
 * UART initialization
 * The baud rate for the UARTs is calculated as
 * With BRGH = 0, BR = Fcyc / (16 * (BRG + 1))
 * With BRGH = 1, BR = Fcyc / (4 * (BRG + 1))
 * Fcyc = Fosc/2
 * With 8MHz clock,
 * BRG		Baud
 * 51		19230
 * 25		38461
 */
void InitializeUARTs(void) {
	unsigned int index;
	char temp;

	/* 
	 * Init UART 1 
	 * Not in use
	 * 38.4K, 8 bits, no parity, 1 stop bit
	 * With BRGH =1, U1BRG = (Fcyc/Baud*4)-1
	 * With BRGH =0, U1BRG = (Fcyc/Baud*16)-1
	 */

#if U1_IN_USE
	/* set I/O pin selects */
	#ifdef IO_PIN_SELECT
	U1RXReg = U1RXRP;
	U1TXReg = U1TXCode;
	#endif

	U1MODEbits.UARTEN = TRUE;
	//U1MODEbits.WAKE = TRUE;
	U1MODEbits.BRGH = TRUE;
	//U1MODEbits.LPBACK = TRUE;
	//U1BRG = 51;		//19.2kB @ Fcyc = 4MHz
	U1BRG = 25;			//38.4kB @ Fcyc = 4MHz

	/* Initialize the U1 buffer set */
	for (index=0; index<U1_RECEIVE_BUFFER_SIZE; index++)
		receiveFromU1Buffer[index] = 0;
	for (index=0; index<U1_TRANSMIT_BUFFER_SIZE; index++)
		transmitToU1Buffer[index] = 0;
	pU1ReceiveBufferRead = receiveFromU1Buffer;
	pU1ReceiveBufferWrite = receiveFromU1Buffer;
	pU1TransmitBufferRead = transmitToU1Buffer;
	pU1TransmitBufferWrite = transmitToU1Buffer;
	charsInU1ReceiveBuffer = 0;
	charsInU1TransmitBuffer = 0;

	/* Let the UART settle, then remove any junk
	 * from the receive buffer.
	 */
	Wait(1);
	while (U1STAbits.URXDA)
		temp = U1RXREG;

	/* Enable UART1 interrupts */
	IFS0bits.U1RXIF = FALSE;
	IFS0bits.U1TXIF = FALSE;
	IEC0bits.U1RXIE = TRUE;
	IEC0bits.U1TXIE = TRUE;

	/* Flow control */
	U1FlowOn = TRUE;
	U1TxFIFO_Count = 0;
#else /* U1_IN_USE FALSE */
	U1MODEbits.UARTEN = FALSE;
#endif

	/* Init UART 2
	 * Talks to port, either USB or debug
	 * 8 bits, no parity, 1 stop bit
	 */
#if U2_IN_USE
	/* set I/O pin selects */
	#ifdef IO_PIN_SELECT
	U2_Rx_RPIR = U2_Rx_RP;
	U2_Tx_RPOR = U2_Tx_Code;
	#endif

	/* set control bits */
	U2MODEbits.UARTEN = TRUE;
    U2MODEbits.URXEN = TRUE;
	//U2MODEbits.WAKE = TRUE;
	//U2MODEbits.LPBACK = TRUE;

	/* set Baud rate */
#if DEBUG
	U2BRG = 161;			//38.4kB @ Fcyc 100MHz
	U2MODEbits.BRGH = FALSE;
#else
	//BaudRateDivisor = (FMcycle/12) - 1;
	U2BRG = BaudRateDivisor;
	U2MODEbits.BRGH = UART2BRGH;
#endif

	/* Initialize the U2 buffer set */
	for (index=0; index<U2_TRANSMIT_BUFFER_SIZE; index++)
		transmitToU2Buffer[index] = 0x5c;
	for (index=0; index<U2_RECEIVE_BUFFER_SIZE-1; index++)
		receiveFromU2Buffer[index] = 0x5a;
	pU2ReceiveBufferRead = receiveFromU2Buffer;
	pU2ReceiveBufferWrite = receiveFromU2Buffer;
	pU2TransmitBufferRead = transmitToU2Buffer;
	pU2TransmitBufferWrite = transmitToU2Buffer;
	charsInU2ReceiveBuffer = 0;
	charsInU2TransmitBuffer = 0;

	/* Empty any junk from the receive buffer */
	while (0 == U2STAHbits.URXBE)
		temp = U2RXREG;

	/* Enable UART2 interrupts */
	IFS1bits.U2RXIF = FALSE;
	IFS1bits.U2TXIF = FALSE;
	IEC1bits.U2RXIE = TRUE;
	IEC1bits.U2TXIE = TRUE;
	//IEC4bits.U2ERIE = TRUE;

	/* Flow control */
	U2FlowOn = TRUE;
	U2TxFIFO_Count = 0;
#else /* U2_IN_USE FALSE */
	U2MODEbits.UARTEN = FALSE;
#endif
}

#if U1_IN_USE
/* *************************************************
 * UART 1 routines 
 */

/*
 * Send a char out UART 1, handing all counters
 * and pointers.
 */
void PutU1Char(char theChar) {
	while (U1_TRANSMIT_BUFFER_SIZE <= charsInU1TransmitBuffer){
		/* wait here to let buffer empty via interrupts */
	}

	/* Turn off the interrupt */
	IEC0bits.U1TXIE = FALSE;

	/* To make debug easy, always put char in
	 * buffer so we can read it later 
	 */
	*pU1TransmitBufferWrite++ = theChar;
	charsInU1TransmitBuffer++;

	/* and circularize write the buffer */
	if (pU1TransmitBufferWrite >= transmitToU1Buffer 
			+ U1_TRANSMIT_BUFFER_SIZE) {
		pU1TransmitBufferWrite = transmitToU1Buffer;
	}
	
	/* if the UART is empty, put the char there too 
	 * Bug in rev 4 of PIC silicon, UTXBF not working, so
	 * we keep track of FIFO count.
	 * if ((0 == U1STAbits.UTXBF) && (1 == charsInU1TransmitBuffer)) {
	 */
	if ((U1TxFIFO_Count < 3) && (1 == charsInU1TransmitBuffer)) {
		charsInU1TransmitBuffer--;
		pU1TransmitBufferRead++;
		U1TXREG = theChar;
		U1STAbits.UTXEN = TRUE;
		U1TxFIFO_Count++;

		/* circularize the read buffer */
		if (pU1TransmitBufferRead >= transmitToU1Buffer 
				+ U1_TRANSMIT_BUFFER_SIZE) {
			pU1TransmitBufferRead = transmitToU1Buffer;
		}
	}

	/* Turn the interrupt back on */
	IEC0bits.U1TXIE = TRUE;
}

/*
 * Send a null terminated string out UART 1.
 */
void PutU1String(char * theString) {
	while (*theString) {
		PutU1Char(*theString++);
	}
}


/*
 * Get one char from the UART 1 receive buffer
 * handling all pointers and counters.
 */
char GetU1Char(void) {
	char theReturn;
	char theChar;

	if (charsInU1ReceiveBuffer > 0) {
		theChar = *pU1ReceiveBufferRead++;
		charsInU1ReceiveBuffer--;

		/* Flow control */
		if (!U1FlowOn && charsInU1ReceiveBuffer < U1_RECEIVE_BUFFER_SIZE/2) {
			PutU1Char(XOn);
			U1FlowOn = TRUE;
		}

		/* circularize the buffer */
		if (pU1ReceiveBufferRead >= receiveFromU1Buffer
				+ U1_RECEIVE_BUFFER_SIZE) {
			pU1ReceiveBufferRead = receiveFromU1Buffer;
		}

		theReturn = theChar;
	} else {
		theReturn = 0x00;
	}

	return theReturn;
}

/*
 * Get a string terminated with carriage return. 
 * Calling routine passes a pointer to buffer to 
 * receive the string. Routine adds null termination
 * to the received string.
 * The routine returns the number of characters found.
 */
int GetU1String(char *theString) {
	char theChar;
	Bool theReturn=0;

	while (!TimerExpired()) {
		if (charsInU1ReceiveBuffer > 0) {
			theChar = GetU1Char();
			*theString++ = theChar;
			theReturn++;
			if (0x0A == theChar) {
				break;
			}
			if (0x0D == theChar) {
				break;
			}
		}
	}
	*theString = 0x00;
	return theReturn;
}
#endif

#if U2_IN_USE
/* *******************************************************
 * UART 2 routines 
 */

/*
 * Send one char out UART 2, handling
 * all counters and pointers.
 */
void PutU2Char(char theChar) {
	while (U2_TRANSMIT_BUFFER_SIZE <= charsInU2TransmitBuffer){
		/* wait here to let buffer empty */
	}

	/* Turn off the interrupt */
	IEC1bits.U2TXIE = FALSE;

	/* Put the char in the buffer to make debug easier */
	*pU2TransmitBufferWrite++ = theChar;
	charsInU2TransmitBuffer++;

	/* and circularize the buffer */
	if (pU2TransmitBufferWrite >= transmitToU2Buffer 
			+ U2_TRANSMIT_BUFFER_SIZE) {
		pU2TransmitBufferWrite = transmitToU2Buffer;
	}

	/* if the UART is empty, put the char there too 
	 * Bug in rev 4 of PIC silicon, UTXBF not working, so
	 * we keep track of FIFO count.
	 * 	if ((0 == U2STAbits.UTXBF) && (1 == charsInU2TransmitBuffer)) {
	 */
	if ((U2TxFIFO_Count < 3) && (1 == charsInU2TransmitBuffer)) {
		pU2TransmitBufferRead++;
		charsInU2TransmitBuffer--;
		U2TXREG = theChar;
		U2MODEbits.UTXEN = TRUE;
		U2TxFIFO_Count++;

		/* and circularized the read pointer */
		if (pU2TransmitBufferRead >= transmitToU2Buffer 
				+ U2_TRANSMIT_BUFFER_SIZE) {
			pU2TransmitBufferRead = transmitToU2Buffer;
		}
	}

	/* Turn the interrupt back on */
	IEC1bits.U2TXIE = TRUE;
}

/*
 * Send a null terminated string out UART 2.
 */
void PutU2String(unsigned char * theString) {
	while (*theString) {
		PutU2Char(*theString++);
	}
}

void PutU2QString(char * theString) {
    while (*theString) {
		PutU2Char((unsigned char)*theString++);
	}
}

/*
 * Send data that may include zero out UART 2.
 * Bypass the circular buffer to speed up data
 * transmission.
 */
void PutU2Data(unsigned char * theString, unsigned int length, unsigned char * theMask) {
    unsigned char theChar;
    
	IEC1bits.U2TXIE = FALSE;

	U2MODEbits.UTXEN = TRUE;
	while (length--) {     
        while (FALSE == *theMask++) {
            theString++;
        }
        theChar = *theString++;
        if (255 == theChar) {
            U2TXREG = 254;
        } else {
            U2TXREG = theChar;
        }
	}
	IEC1bits.U2TXIE = TRUE;
}



/*
 * Get a char from the UART 2 receive buffer,
 * handling all of the counters and pointers.
 */
char GetU2Char(void) {
	char theReturn;
	char theChar;

	if (charsInU2ReceiveBuffer > 0) {

		/* Turn off the interrupt */
		IEC1bits.U2RXIE = FALSE;

		theChar = *pU2ReceiveBufferRead++;
		charsInU2ReceiveBuffer--;

		/* Flow control */
		if (!U2FlowOn && charsInU2ReceiveBuffer < U2_RECEIVE_BUFFER_SIZE/2) {
			PutU2Char(XOn);
			U2FlowOn = TRUE;
		}

		/* Turn the interrupt back on */
		IEC1bits.U2RXIE = TRUE;

		//circularize the buffer
		if (pU2ReceiveBufferRead >= receiveFromU2Buffer
				+ U2_RECEIVE_BUFFER_SIZE) {
			pU2ReceiveBufferRead = receiveFromU2Buffer;
		}

		/* Add another millisecond so the process doesn't time out
		 * while we're getting characters.
		 */
		AddTime(&timerSeconds, &timerMilliseconds, 1);

		theReturn = theChar;
	} else
		theReturn = 0x00;

	return theReturn;
}

/*
 * Get a string terminated with carriage return. 
 * Calling routine passes a pointer to buffer to 
 * receive the string. Routine adds null termination
 * to the received string.
 * The routine returns TRUE if the carriage return is
 * found, FALSE if it times out before getting the 
 * carriage return.
 */
int GetU2String(char *theString) {
	char theChar;
	Bool theReturn=0;

	while (!TimerExpired()) {
		if (charsInU2ReceiveBuffer > 0) {
			theChar = GetU2Char();
			*theString++ = theChar;
			theReturn++;
			if (0x0A == theChar) {
				break;
			}
			if (0x0D == theChar) {
				break;
			}
		}
	}
	*theString = 0x00;
	theReturn++;
	return theReturn;
}
#endif

