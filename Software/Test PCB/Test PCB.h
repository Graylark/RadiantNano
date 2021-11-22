/*
 * Test PCB.h
 * public declarations for the main.c module
 * Test PCB 
 * RadiantNano
 * Author: Dexter Eames
 * Started: October 12, 2021
 */


#ifndef TEST_PCB_H

#include "xc.h"
#include "types.h"

#define DEBUG 1     /* catch error interrupts */
#define PCB_VERSION 1

/* The dsPIC33CK%12MP605 has I/O Pin select so...*/
#define IO_PIN_SELECT TRUE
#ifdef IO_PIN_SELECT

#define SPI1_MOSI_RPOR RPOR13bits.RP59R
#define SPI1_MOSI_Code 5
#define SPI1_SCK_RPOR RPOR13bits.RP58R
#define SPI1_SCK_Code 6

#define U2_Rx_RPIR RPINR19bits.U2RXR
#define U2_Rx_RP 72
#define U2_Tx_RPOR RPOR12bits.RP57R
#define U2_Tx_Code 3

#define ADC_TRIGGER_RPIR RPINR47bits.ADCTRGR
#define ADC_TRIGGER_PEAK_RP 46
#define ADC_TRIGGER_THRESHOLD_RP 47
#endif

/* Constants */

/* Mega cycles per second */
#define FMcycle	100

#define HISTOGRAM_SIZE 2048

#define NS_PER_ADC_CLOCK 20

#define MAX_SHORT 65535

/*
 * I/O Port bit definitions
 */

/* Analog Inputs */
#define Analog_Out_ADC 0
#define Analog_Out_TRISBit TRISAbits.TRISA0
#define Analog_Out_AnalogBit ANSELAbits.ANSELA0
#define Temperature_1_ADC 9
#define Temperature_1_TRISBit TRISAbits.TRISA2
#define Temperature_1_AnalogBit ANSELAbits.ANSELA2
#define HV_Monitor_ADC 12
#define HV_Monitor_TRISBit TRISCbits.TRISC0
#define HV_Monitor_AnalogBit ANSELCbits.ANSELC0

/* Analog Outputs */
#define Trigger_Threshold_TRISBit TRISAbits.TRISA3
#define Trigger_Threshold_AnalogBit ANSELAbits.ANSELA3

/* Digital Inputs */
#define Peak_Trigger_TRISBit TRISBbits.TRISB14
#define Peak_Trigger_PortBit PORTBbits.RB14
#define Threshold_Trigger_TRISBit TRISBbits.TRISB15
#define Threshold_Trigger_PortBit PORTBbits.RB15
#define UART2_RxD_TRISBit TRISDbits.TRISD8
#define UART2_RxD_PortBit PORTDbits.RD8

/* Digital Outputs */
#define Integrator_Zero_TRISBit TRISAbits.TRISA4
#define Integrator_Zero_LatchBit LATAbits.LATA4
#define IntegratorEnable_TRISBit TRISAbits.TRISA1
#define Integrator_Enable_LatchBit LATAbits.LATA1
#define CS_HV_TRISBit TRISCbits.TRISC5
#define CS_HV_LatchBit LATCbits.LATC5
#define CS_DAC_TRISBit TRISBbits.TRISB9
#define CS_DAC_LatchBit LATBbits.LATB9
#define Analog_Power_Enable_TRISBit TRISCbits.TRISC12
#define Analog_Power_Enable_LatchBit LATCbits.LATC12


/* UART I/O 
 * The USB/Controller UART uses UART2
 * U2Rx is RD8
 * U2Tx is RC9
 */

/* SPI I/O */
#define SPI_SCLK1_TRISBit TRISCbits.TRISC10
#define SPI_SCLK1_LatchBit LATCbits.LATC10
#define SPI_MOSI1_TRISBit TRISCbits.TRISC11
#define SPI_MOSI1_LatchBit LATCbits.LATC11


/* Unused I/O */
/*
 * A0   Integrator Zero
 * A1   Integrator Enable
 * A2   Temperature 1
 * A3   Trigger Threshold
 * A4   Analog Out
 * B0   Oscillator In
 * B1   Oscillator Out
 * B2   Unused
 * B3   PGD
 * B4   PBC
 * B5   Unused
 * B6   Unused
 * B7   Unused
 * B8   Unused
 * B9   DAC (Daughter)
 * B10  Unused
 * B11  Unused
 * B12  Unused
 * B13  Unused
 * B14  Peak Trigger
 * B15  Threshold Trigger
 * C0   HV Monitor
 * C1   Unused
 * C2   Unused
 * C3   Unused
 * C4   Unused
 * C5   CS HV
 * C6   Unused
 * C7   Unused
 * C8   Unused
 * C9   TxD
 * C10  SPI CLK
 * C11  SPI MOSI
 * C12  Analog Power Enable
 * C13  Unused
 * D1   Unused
 * D8   RxD
 * D10  Unused
 * D13  Unused
 *  
 */

#define B2_TRISBit TRISBbits.TRISB2
#define B2_LatchBit LATBbits.LATB2
#define B5_TRISBit TRISBbits.TRISB5
#define B5_LatchBit LATBbits.LATB5
#define B6_TRISBit TRISBbits.TRISB6
#define B6_LatchBit LATBbits.LATB6
#define B7_TRISBit TRISBbits.TRISB7
#define B7_LatchBit LATBbits.LATB7
#define B8_TRISBit TRISBbits.TRISB8
#define B8_LatchBit LATBbits.LATB8
/* DAC Daughter 
#define B9_TRISBit TRISBbits.TRISB9
#define B9_LatchBit LATBbits.LATB9
 * */
#define B10_TRISBit TRISBbits.TRISB10
#define B10_LatchBit LATBbits.LATB10
#define B11_TRISBit TRISBbits.TRISB11
#define B11_LatchBit LATBbits.LATB11
#define B12_TRISBit TRISBbits.TRISB12
#define B12_LatchBit LATBbits.LATB12
#define B13_TRISBit TRISBbits.TRISB13
#define B13_LatchBit LATBbits.LATB13

#define C1_TRISBit TRISCbits.TRISC1
#define C1_LatchBit LATCbits.LATC1
#define C2_TRISBit TRISCbits.TRISC2
#define C2_LatchBit LATCbits.LATC2
#define C3_TRISBit TRISCbits.TRISC3
#define C3_LatchBit LATCbits.LATC3
#define C4_TRISBit TRISCbits.TRISC4
#define C4_LatchBit LATCbits.LATC4
#define C6_TRISBit TRISCbits.TRISC6
#define C6_LatchBit LATCbits.LATC6
#define C7_TRISBit TRISCbits.TRISC7
#define C7_LatchBit LATCbits.LATC7
#define C8_TRISBit TRISCbits.TRISC8
#define C8_LatchBit LATCbits.LATC8
#define C13_TRISBit TRISCbits.TRISC13
#define C13_LatchBit LATCbits.LATC13

#define D1_TRISBit TRISDbits.TRISD1
#define D1_LatchBit LATDbits.LATD1
#define D10_TRISBit TRISDbits.TRISD10
#define D10_LatchBit LATDbits.LATD10
#define D13_TRISBit TRISDbits.TRISD13
#define D13_LatchBit LATDbits.LATD13


/* Type definitions */
typedef enum {
	Process_Idle,
	Process_InitializeMeasurement,
	Process_Measuring,
	Process_StopMeasuring
} ProcessStateType;

/* To calibrate the ADC we map the ADC histogram into a linear histogram.
 * The ADC bin width is the voltage range that falls within a single ADC value.
 * The ADC bins widths are non uniform, and a single ADC bin can map into 1, 2 or 3 
 * output bins. The Compensation array is created by piping a linear voltage ramp 
 * into the system and collecting data until the ADC bins have about 10,000 counts
 * each. 
 * 
 */
typedef struct {
    unsigned char End_1;
    unsigned char End_2;
    unsigned char End_3;
   // unsigned short Bin_1;
   // unsigned short Bin_2;
   // unsigned short Bin_3;
} CompensationArrayType;

/* Global variables */
extern ProcessStateType ProcessState;

extern unsigned int Histogram[HISTOGRAM_SIZE];
extern volatile Bool HistogramOverflow;

extern Bool ADC_TriggerFromThreshold;
extern Bool ADC_TriggerFromDelay;
extern Bool ADC_TriggerFromPeak;
extern Bool IntegratorOn;
extern unsigned int ADC_TriggerDelayCount;
extern volatile Bool ADC_TemperatuerReady;
extern volatile Bool ADC_HVReady;

extern Bool StartMeasurement;
extern Bool StopMeasurement;
extern Bool ContinueMeasurement;
extern Bool MeasurementRunning;
extern volatile Bool MeasurementDone;

extern unsigned short HighVoltageCentivolts;
extern Bool HighVoltageOn;
extern Bool DAC_On;
extern unsigned int DAC_Centivolts;

extern unsigned char BaudRateDivisor;


/* Functions */
void SetDACVoltage(void);
void InitializeMeasurement(void);
void HaltMeasurement(void);
void SendHistogram(void);
unsigned short GetTemperature(void);
unsigned int GetHighVoltage(void);

#define TEST_PCB_H
#endif
