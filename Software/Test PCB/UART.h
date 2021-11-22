/*
 * UART.h
 * Public declarations for UART.c
 */

#include "types.h"

/* #defines */
#define XOn 0x11
#define XOff 0x13

#define U1_IN_USE FALSE
#define U2_IN_USE TRUE
#define U3_IN_USE FALSE

/* Global variables */
#if U1_IN_USE
#define U1_RECEIVE_BUFFER_SIZE 256
#define U1_TRANSMIT_BUFFER_SIZE 256
extern char receiveFromU1Buffer[U1_RECEIVE_BUFFER_SIZE];
extern char transmitToU1Buffer[U1_TRANSMIT_BUFFER_SIZE];
extern char * pU1ReceiveBufferRead;
extern char * pU1ReceiveBufferWrite;
extern char * pU1TransmitBufferRead;
extern char * pU1TransmitBufferWrite;
extern volatile int charsInU1ReceiveBuffer;
extern volatile int charsInU1TransmitBuffer;
extern unsigned int U1TxFIFO_Count;
extern volatile Bool U1FlowOn;
#else
#define U1_RECEIVE_BUFFER_SIZE 0
#define U1_TRANSMIT_BUFFER_SIZE 0
#endif

#if U2_IN_USE
#define U2_RECEIVE_BUFFER_SIZE 256
#define U2_TRANSMIT_BUFFER_SIZE 256
extern char receiveFromU2Buffer[U2_RECEIVE_BUFFER_SIZE];
extern char transmitToU2Buffer[U2_TRANSMIT_BUFFER_SIZE];
extern char * pU2ReceiveBufferRead;
extern char * pU2ReceiveBufferWrite;
extern char * pU2TransmitBufferRead;
extern char * pU2TransmitBufferWrite;
extern volatile int charsInU2ReceiveBuffer;
extern volatile int charsInU2TransmitBuffer;
extern unsigned int U2TxFIFO_Count;
extern volatile Bool U2FlowOn;
extern unsigned char UART2BRGH;
#else
#define U2_RECEIVE_BUFFER_SIZE 0
#define U2_TRANSMIT_BUFFER_SIZE 0
#endif

#if U3_IN_USE

#else
#define U3_RECEIVE_BUFFER_SIZE 0
#define U3_TRANSMIT_BUFFER_SIZE 0
#endif


/* Variables */


/* Public functions */
extern void InitializeUARTs(void);
#if U1_IN_USE
void PutU1Char(char theChar);
void PutU1String(char * theString);
char GetU1Char(void);
int GetU1String(char *theString);
#endif
#if U2_IN_USE
void PutU2Char(char theChar);
void PutU2String(unsigned char * theString);
void PutU2QString(char * theString);
void PutU2Data(unsigned char * theString, unsigned int length, unsigned char * theMask);
char GetU2Char(void);
int GetU2String(char *theString);
#endif

