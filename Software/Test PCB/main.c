/*
 * main.c
 * Test PCB
 * RadiantNano, Inc
 * Runs on dsPIC33CK512MP605
 * Started October 12, 2021
 * Author: Dexter Eames
 */
  
/*
 * This code is single threaded
 * The PIC is set up,
 * then goes to a polling loop,
 * the USB input is monitored for inputs,
 * if collection is turned on, the data is collected,
 * and sent out the USB port on command.
 */

/*
 * Set Device Configuration Registers
 * For now, 
 * turn off boot block security and write protect
 * turn off general segment code protect and general segment write protect
 * turn off watch dog timer
 */
/* DSPIC33CK512MP605 Configuration Bit Settings */

// FSEC
#pragma config BWRP = OFF               // Boot Segment Write-Protect bit (Boot Segment may be written)
#pragma config BSS = DISABLED           // Boot Segment Code-Protect Level bits (No Protection (other than BWRP))
#pragma config BSEN = OFF               // Boot Segment Control bit (No Boot Segment)
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = DISABLED           // General Segment Code-Protect Level bits (No Protection (other than GWRP))
#pragma config CWRP = OFF               // Configuration Segment Write-Protect bit (Configuration Segment may be written)
#pragma config CSS = DISABLED           // Configuration Segment Code-Protect Level bits (No Protection (other than CWRP))
#pragma config AIVTDIS = OFF            // Alternate Interrupt Vector Table bit (Disabled AIVT)

// FBSLIM
#pragma config BSLIM = 0x1FFF           // Boot Segment Flash Page Address Limit bits (Enter Hexadecimal value)

// FSIGN

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Source Selection (Internal Fast RC (FRC))
#pragma config IESO = ON                // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FOSC
#pragma config POSCMD = HS              // Primary Oscillator Mode Select bits (HS Crystal Oscillator Mode)
#pragma config OSCIOFNC = ON            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config FCKSM = CSECME           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are enabled)
#pragma config PLLKEN = ON              // PLL Lock Status Control (PLL lock signal will be used to disable PLL clock output if lock is lost)
#pragma config XTCFG = G1               // XT Config (8-16 MHz crystals)
#pragma config XTBST = DISABLE          // XT Boost (Default kick-start)

// FWDT
#pragma config RWDTPS = PS2147483648    // Run Mode Watchdog Timer Post Scaler select bits (1:2147483648)
#pragma config RCLKSEL = LPRC           // Watchdog Timer Clock Select bits (Always use LPRC)
#pragma config WINDIS = ON              // Watchdog Timer Window Enable bit (Watchdog Timer operates in Non-Window mode)
#pragma config WDTWIN = WIN25           // Watchdog Timer Window Select bits (WDT Window is 25% of WDT period)
#pragma config SWDTPS = PS2147483648    // Sleep Mode Watchdog Timer Post Scaler select bits (1:2147483648)
#pragma config FWDTEN = ON_SW           // Watchdog Timer Enable bit (WDT controlled via SW, use WDTCON.ON bit)

// FPOR
#pragma config BISTDIS = DISABLED       // Memory BIST Feature Disable (mBIST on reset feature disabled)

// FICD
#pragma config ICS = PGD2               // ICD Communication Channel Select bits (Communicate on PGC2 and PGD2)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)
#pragma config NOBTSWP = DISABLED       // BOOTSWP instruction disable bit (BOOTSWP instruction is disabled)

// FDMTIVTL
#pragma config DMTIVTL = 0xFFFF         // Dead Man Timer Interval low word (Enter Hexadecimal value)

// FDMTIVTH
#pragma config DMTIVTH = 0xFFFF         // Dead Man Timer Interval high word (Enter Hexadecimal value)

// FDMTCNTL
#pragma config DMTCNTL = 0xFFFF         // Lower 16 bits of 32 bit DMT instruction count time-out value (0-0xFFFF) (Enter Hexadecimal value)

// FDMTCNTH
#pragma config DMTCNTH = 0xFFFF         // Upper 16 bits of 32 bit DMT instruction count time-out value (0-0xFFFF) (Enter Hexadecimal value)

// FDMT
#pragma config DMTDIS = OFF             // Dead Man Timer Disable bit (Dead Man Timer is Disabled and can be enabled by software)

// FDEVOPT
#pragma config ALTI2C1 = OFF            // Alternate I2C1 Pin bit (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = OFF            // Alternate I2C2 Pin bit (I2C2 mapped to SDA2/SCL2 pins)
#pragma config ALTI2C3 = OFF            // Alternate I2C3 Pin bit (I2C3 mapped to SDA3/SCL3 pins)
#pragma config SMBEN = STANDARD         // SM Bus Enable (Standard I2C input threshold operation)
#pragma config DUPPWM = PPS             // Duplicate PWM Pin Mapping (Default PWM Functions of PPS and Fixed Pins)
#pragma config SPI2PIN = PPS            // SPI2 Pin Select bit (SPI2 uses I/O remap (PPS) pins)

// FALTREG
#pragma config CTXT1 = OFF              // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 1 bits (Not Assigned)
#pragma config CTXT2 = OFF              // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 2 bits (Not Assigned)
#pragma config CTXT3 = OFF              // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 3 bits (Not Assigned)
#pragma config CTXT4 = OFF              // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 4 bits (Not Assigned)

// FBTSEQ
#pragma config BSEQ = 0xFFF             // Relative value defining which partition will be active after device Reset; the partition containing a lower boot number will be active (Enter Hexadecimal value)
#pragma config IBSEQ = 0xFFF            // The one's complement of BSEQ; must be calculated by the user and written during device programming. (Enter Hexadecimal value)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.



#include <xc.h>
 
#include "Test PCB.h"
#include "MAX1932.h"
#include "PICFlash.h"
#include "SPI.h"
#include "stringUtils.h"
#include "timers.h"
#include "UART.h"
#include "USB.h"


/* Defines */


/* State Variables */
ProcessStateType ProcessState;


/* Variables */
volatile Bool HistogramOverflow;
Bool ADC_TriggerFromThreshold;
Bool ADC_TriggerFromDelay;
Bool ADC_TriggerFromPeak;
Bool IntegratorOn;
unsigned int ADC_TriggerDelayCount;
volatile Bool ADC_TemperatuerReady;
volatile Bool ADC_HVReady;

Bool StartMeasurement;
Bool StopMeasurement;
Bool ContinueMeasurement;
Bool MeasurementRunning;
volatile Bool MeasurementDone;

unsigned short InitializationTemperature;

unsigned short HighVoltageCentivolts;
Bool HighVoltageOn;
Bool DAC_On;
unsigned int DAC_Centivolts;

unsigned char BaudRateDivisor;

/* Buffers */
unsigned int Histogram[HISTOGRAM_SIZE];
CompensationArrayType CompensationArray[HISTOGRAM_SIZE];

/*
 * Initialize the microcontroller
 * Set up clock 
 * Set up ports
 * Set Watch Dog
 */
void InitializeMicrocontroller(void) {
	/* 
	 * Set up PLL
	 * External 12MHz crystal with PLL for Fosc of 200MHz.
	 * Fosc = Fin * M / ( N1 x N2 x N3 X 2)
	 * M = PLLFBDIV
	 * N1 = PLLPRE
	 * N2 = PLLPOST1DIV
     * N3 = PLLPOST2DIV
	 * If changes are made here, change FMcycle defined in Detector.h
	 * NB: The Baud rate values in the B command depend upon
	 * FMcycle being divisible by 4, 8 and 12.
     * FMcycle = Fosc/2
	 * FMcycle = 100
	 */
    PLLFBDbits.PLLFBDIV = 100;  /* M */
	CLKDIVbits.PLLPRE = 1;      /* N1 */
	PLLDIVbits.POST1DIV = 3;    /* N2 */
    PLLDIVbits.POST2DIV = 1;    /* N3 */

	/* Switch to Primary clock with PLL */
	OSCCONH = 0x78;
	OSCCONH = 0x9A;
	/* OSCCONbits.NOSC = 0x03; */
__builtin_write_OSCCONH(0x03);

	OSCCONL = 0x46;
	OSCCONL = 0x57;
	/* OSCCONbits.OSWEN = 0x01; */
__builtin_write_OSCCONL(OSCCON | 0x01);

	/* Wait for Clock switch to occur */
	while (OSCCONbits.COSC!= 0x03);

	while (OSCCONbits.LOCK != 1); 

	/* Init Doze CPU clock to divide by 32, so the CPU can
	 * run slower (250kHz) to save power.
	 */
	CLKDIVbits.ROI = TRUE;
	CLKDIVbits.DOZE2 = 1;
	CLKDIVbits.DOZE0 = 1;

	/* Set all inputs for digital, then make selected ones
	 * analog.
	 */
	ANSELA = 0;
	ANSELB = 0;
	ANSELC = 0;
	ANSELD = 0;


	/* Set up the I/O */
	/* Analog Inputs */
    Analog_Out_TRISBit = 1;
    Analog_Out_AnalogBit = 1;
    Temperature_1_TRISBit = 1;
    Temperature_1_AnalogBit = 1;
    HV_Monitor_TRISBit = 1;
    HV_Monitor_AnalogBit = 1;
    
    /* Digital Inputs */
    Peak_Trigger_TRISBit = 1;
    Threshold_Trigger_TRISBit = 1;
    UART2_RxD_TRISBit = 1;
    
	/* Change Notification Interrupt Enables */
    
    /* Analog Outputs */
    Trigger_Threshold_TRISBit = 0;
    Trigger_Threshold_AnalogBit = 1;

	/* Digital Outputs */
    Integrator_Zero_LatchBit = TRUE;    /* Low true */
    Integrator_Zero_TRISBit = 0;
    Integrator_Enable_LatchBit = TRUE;  /* Low true */
    IntegratorEnable_TRISBit = 0;
    CS_HV_LatchBit = TRUE;
    CS_HV_TRISBit = 0;
    CS_DAC_LatchBit = TRUE;
    CS_DAC_TRISBit = 0;
    Analog_Power_Enable_LatchBit = TRUE;
    Analog_Power_Enable_TRISBit = 0;

	SPI_SCLK1_LatchBit = FALSE;
	SPI_SCLK1_TRISBit = 0;
	SPI_MOSI1_LatchBit = FALSE;
	SPI_MOSI1_TRISBit = 0;
    
	/* Turn unused I/O into outputs and drive low */
	B2_LatchBit = 0;
	B2_TRISBit = FALSE;
	B5_LatchBit = 0;
	B5_TRISBit = FALSE;
	B6_LatchBit = 0;
	B6_TRISBit = FALSE;
    B7_LatchBit = 0;
	B7_TRISBit = FALSE;
    B8_LatchBit = 0;
	B8_TRISBit = FALSE;
    /*
	B9_LatchBit = 0;  
	B9_TRISBit = FALSE; 
     */
	B10_LatchBit = 0;
	B10_TRISBit = FALSE;
	B11_LatchBit = 0;
	B11_TRISBit = FALSE;
	B12_LatchBit = 0;
	B12_TRISBit = FALSE;
	B13_LatchBit = 0;
	B13_TRISBit = FALSE;

	C1_LatchBit = 0;
	C1_TRISBit = FALSE;
	C2_LatchBit = 0;
	C2_TRISBit = FALSE;
	C4_LatchBit = 0;
	C4_TRISBit = FALSE;
	C4_LatchBit = 0;
	C4_TRISBit = FALSE;
	C6_LatchBit = 0;
	C6_TRISBit = FALSE;
	C7_LatchBit = 0;
	C7_TRISBit = FALSE;
	C8_LatchBit = 0;
	C8_TRISBit = FALSE;
	C13_LatchBit = 0;
	C13_TRISBit = FALSE;

	D1_LatchBit = 0; 
	D1_TRISBit = FALSE;
	D10_LatchBit = 0;
	D10_TRISBit = FALSE;
	D13_LatchBit = 0;
	D13_TRISBit = FALSE;

    
    /* Disable unused peripherals in dsPIC */
    PMD1bits.QEI1MD = 0;
    PMD1bits.PWMMD = 0;
    PMD1bits.I2C1MD = 0;
    PMD1bits.SPI2MD = 0;
    PMD1bits.U1MD = 0;
    PMD1bits.C1MD = 0;
    PMD1bits.C2MD = 0;
    
    PMD3bits.PMPMD = 0;
    PMD3bits.CRCMD = 0;
    PMD3bits.QEI2MD = 0;
    PMD3bits.U3MD = 0;
    PMD3bits.I2C3MD = 0;
    PMD3bits.I2C2MD = 0;
    
    PMD4bits.REFOMD = 0;
    
    PMD6bits.DMA7MD = 0;
    PMD6bits.DMA6MD = 0;
    PMD6bits.DMA5MD = 0;
    PMD6bits.DMA4MD = 0;
    PMD6bits.DMA3MD = 0;
    PMD6bits.DMA2MD = 0;
    PMD6bits.DMA1MD = 0;
    PMD6bits.DMA0MD = 0;
    PMD6bits.QEI3MD = 0;
    PMD6bits.SPI3MD = 0;
    
    PMD7bits.CMP6MD = 0;
    PMD7bits.CMP5MD = 0;
    PMD7bits.CMP4MD = 0;
    PMD7bits.CMP3MD = 0;
    PMD7bits.CMP2MD = 0;
    PMD7bits.CMP1MD = 0;
    
    PMD8bits.OPAMPMD = 0;
    PMD8bits.SENT2MD = 0;
    PMD8bits.SENT1MD = 0;
    PMD8bits.DMTMD = 0;
    PMD8bits.CLC4MD = 0;
    PMD8bits.CLC3MD = 0;
    PMD8bits.CLC2MD = 0;
    PMD8bits.CLC1MD = 0;
    PMD8bits.BIASMD = 0;
	
	/* Clear the RCON register so that reset errors can
	 * be identified.
	 */
	RCON = 0;
}


/*
 *	Timer for going to low power mode.
 */
Bool WakeTimeOver(void) {

	return FALSE;
}




/* ******************************************************
 * Measurement Routines
 * 
 * The dsPIC ADC can be triggered in multiple ways with 
 * this PCB in order to experiment. The PCB can generate a
 * trigger signal (Threshold Trigger) when a low threshold is 
 * crossed, and it can also generate a signal (Peak Trigger) 
 * when the response from the SiPM has flattened out.
 * It is expected that the ADC will begin sampling the signal when
 * the Threshold Trigger is generated, and the conversion will be 
 * started either when Peak Trigger is generated or after a set 
 * delay from the Threshold Trigger.
 */

/* *************************************************************
 * ADC routines
 * Set up the ADC for reading, and
 * turn it on.
 */
void SetUpADC(void) {
    
    /* Turn off module before setting configuration bits*/
    ADCON1Lbits.ADON = 0;
    
    /* Set integer output for the shared core*/
	ADCON1Hbits.FORM = 0;
    
    /* Set 10 bit operation for the shared core*/
	ADCON1Hbits.SHRRES = 2;
    
    /* Source clock divider for shared clock */
    ADCON2Lbits.SHRADCS = 0;    /* Divide Fp by 2 to get 50MHz */
    
	/* Set sampling time for shared cores to 10 conversion clocks */
	ADCON2Hbits.SHRSAMC = 8;
    
    /* Set reference voltage to AVdd, since it's the only choice */
    ADCON3Lbits.REFSEL = 0;

	/* Set the conversion clock at Fp/7 */
    /* The shared ADC core clocks are divided by 2 again */
	ADCON3Hbits.CLKSEL = 0;
    
    /* Divide Fp clock by 2 to get 50MHz ADC clock */
    /* If this is changed, update define of NS_PER_ADC_CLOCK */
    ADCON3Hbits.CLKDIV = 1;
    
    /* Enable dedicated ADC core 0 for the Analog Out signal */
    ADCON3Hbits.C0EN = 1;
    
    /* Enable the shared ADC core */
    ADCON3Hbits.SHREN = 1;
    
    /* Enable timed sampling after trigger for ADC core 0 */
    ADCON4Lbits.SAMC0EN = 1;

    /* Select ANO as input for ADC core 0 */
    ADCON4Hbits.C0CHS = 0;

    /* Set the trigger source for ADC core 0 to be PPS input ADTRG31 */
    /* Can use either peak or threshold by setting PPS RPIN to correct input */
    ADTRIG0Lbits.TRGSRC0 = 31;
    
    /* Set the trigger for the temperature ADC from software */
    ADTRIG2Lbits.TRGSRC9 = 1;
    
    /* Set the trigger for the HV Monitor ADC from software */
    ADTRIG3Lbits.TRGSRC12 = 1;
    
    /* Set default sampling time for ADC core 0 */
    /* Time is measured in 20ns clock ticks */
    ADCORE0Lbits.SAMC = 4;
    
    /* Set resolution for ADC core 0 to 12 bits */
    ADCORE0Hbits.RES = 3;
    
    /* Set the clock divider for ADC core 0 to 2 */
    /* The 100MHz Fp clock is divided by 2 to generate a 50MHz ADC clock */
    ADCORE0Hbits.ADCS = 1;
    
    /* Set ADC core 0 interrupt 1 clock before the data is ready */
    ADCORE0Hbits.EISEL = 0;
    
    /* Enable early interrupt for ADC AN0 */
    /* Do this when initializing collection */
    /* ADEIELbits.EIEN0 = 1; */
    
    /* Enable early interrupt for ADC AN9 */
    ADEIELbits.EIEN9 = 1;
    
    /* Enable early interrupt for ADC AN13 */
    ADEIELbits.EIEN13 = 1;
    
    /* Enable power to ADC core 0 */
    ADCON5Lbits.C0PWR = 1;
    
    /* Enable power to shared ADC core */
    ADCON5Lbits.SHRPWR = 1;
    
	/* Turn on the ADC */
	ADCON1Lbits.ADON = 1;
}



/*
 * Get temperature ADC reading and return it as degrees C.
 * The ADC must already be set up for 10 bit operation.
 */
unsigned short GetTemperature(void) {
	unsigned long temperature;

    /* The STLM20W87F has a -11.69mV/C slope and
     * best fit linear function near 25C of:
     * -11.69mV/C * T + 1.8663V
     * so,
     * (ADC/4096) * 3.3 = 1.8663V -(11.69mV/C) * T
     * T * 11.69mV/C = 1.8663 - (ADC/4096) * 3.3
     * T = 159.65C - ((ADC/4096) * 3.3)/.01169C
     *   = 159.65 - ADC * 0.0689
     */
    
    /* Enable the interrupt */
    ADIELbits.IE9 = TRUE;
    IFS6bits.ADCAN9IF = 0;
    IEC6bits.ADCAN9IE = TRUE;
    
    /* Set up for conversion */
    ADCON3Lbits.SHRSAMP = 1;
    ADCON3Lbits.CNVCHSEL = 9;
    Wait(1);
    ADCON3Lbits.SHRSAMP = 0;
    ADC_TemperatuerReady = FALSE;
    
    /* Trigger conversion */
    ADCON3Lbits.CNVRTCH = 1;
    
    /* Wait for conversion */
    while (!ADC_TemperatuerReady) {
        
    }
    temperature = ADCBUF9;
    temperature *= 69;
    temperature = 159650 - temperature;
    temperature /= 1000;

	return temperature;
}

/*
 * Read the HV monitor and convert it into
 * centivolts. The monitor reduces the HV with
 * a resistor divider, so
 * HV = ((1M + 52.3k)/52.3k) * (ADC * 3.3) / 1024
 *    = 20.1 * (ADC * 3.3) / 4096
 *    = 0.016 * ADC
 */
unsigned int GetHighVoltage(void) {
    unsigned long theVoltage;
    
    /* Enable the interrupt */
    ADIELbits.IE12 = TRUE;
    IFS6bits.ADCAN12IF = 0;
    IEC6bits.ADCAN12IE = TRUE;
    
     /* Set up for conversion */
    ADCON3Lbits.SHRSAMP = 1;
    ADCON3Lbits.CNVCHSEL = 12;
    Wait(1);
    ADCON3Lbits.SHRSAMP = 0;
    ADC_HVReady = FALSE;
    
    /* Trigger conversion */
    ADCON3Lbits.CNVRTCH = 1;
    
    /* Wait for conversion */
    while (!ADC_HVReady) {
        //ADC_HVReady = FALSE;
    }
    
    /* Set up for conversion */
    ADCON3Lbits.CNVCHSEL = 13;
    
    /* Trigger conversion */
    ADCON3Lbits.CNVRTCH = 1;
    
    while (!ADC_HVReady) {
        
    }
    theVoltage = ADCBUF12;
    theVoltage *= 16;
    theVoltage /= 10;
    
    return theVoltage;
}

/* *********************************************************
 * DAC routines
 */
void SetUpDAC(void) {
    
    /* Keep DAC off while setting up */
    DACCTRL1Lbits.DACON = FALSE;
    
    /* Fpllo for clock */
    DACCTRL1Lbits.CLKSEL = 3;
    
    /* Transition mode duration bits */
    DACCTRL2Lbits.TMODTIME = 8;
    
    /* Time from start of transition until steady state filter is enabled */
    DACCTRL2Hbits.SSTIME = 10;
    
    /* Enable DAC 1 */
    DAC1CONLbits.DACEN = TRUE;
    
    /* Connect DAC 1 to DACOUT1 I/O pin */
    DAC1CONLbits.DACOEN = TRUE;
    
    /* Enable the filter */
    DAC1CONLbits.FLTREN = TRUE;
    
    DAC_On = FALSE;
    
}

/* Set the voltage out of the DAC 
 * The DAC can be set in the range of 0.05 to 0.95 of Vdd,
 * or DAC values of 205 (0xCD) to 3890 (0xF32) of 4095.
 * Vdac = DACDAT * Vdd / 4095
 *      = DACDAT * 3.3 / 4095
 * The voltage passed to the function is in centivolts, so
 * (Vdac * 4095) / 330 = DACDAT
 * The circuit for threshold detection has a resistor divider that
 * cuts the voltage in half, so the actual threshold is
 * Vthresh = Vdac/2
 */
void SetDACVoltage(void) {
    unsigned long theLong;
    unsigned int DACDAT;
    
    DACCTRL1Lbits.DACON = FALSE;
    
    theLong = DAC_Centivolts;
    theLong *= 4095;
    theLong *= 2;
    theLong /= 330;
    if (theLong < 205) {
        theLong = 205;
    } else if (theLong > 3890) {
        theLong = 3890;
    }
    DACDAT = theLong;
    DAC1DATHbits.DACDAT = DACDAT;
    
    DACCTRL1Lbits.DACON = TRUE; 
}


/*
 * Set up for running measurement.
 */
void InitializeMeasurement(void) {
    unsigned int index;
    
    /* Set trigger line to ADC */
    if (ADC_TriggerFromPeak) {
        ADC_TRIGGER_RPIR = ADC_TRIGGER_PEAK_RP;
    } else if (ADC_TriggerFromThreshold) {
        ADC_TRIGGER_RPIR = ADC_TRIGGER_THRESHOLD_RP;
    }

	/* Turn on the high voltage */
	SetHighVoltage(HighVoltageCentivolts);
    
    /* Turn on the DAC */
    SetDACVoltage();

	/* Record the temperature */
   	InitializationTemperature = GetTemperature(); 

    if (ContinueMeasurement) {
        /* continuing, so don't zero histogram */
        ContinueMeasurement = FALSE;
    } else {
        /* Zero out the histogram */
        for (index=0; index<HISTOGRAM_SIZE; index++) {
            Histogram[index] = 0;
        }
    }

	/* Wait for high voltage to stabilize. */
	Wait(20);
    
    /* Enable ADC AN0 interrupts */
    ADEIELbits.EIEN0 = 1;
}


/* 
 * Send the measurements to the USB port
 */
void SendHistogram(void) {
    unsigned int index;
    unsigned char theString[12];
    
    PutU2QString("keV,counts\n\r");
    
    for (index=0; index<HISTOGRAM_SIZE; index++){
        IntToString(index, theString);
        PutU2String(theString);
        PutU2QString(", ");
        IntToString(Histogram[index], theString);
        PutU2String(theString);
        PutU2QString("\r");
    }
}

/*
 * Turn off power and reset circuits
 */
void HaltMeasurement(void) {
    
    /* Disable ADC AN0 interrupts */
    ADEIELbits.EIEN0 = 0;
    
	/* Turn off the High Voltage power supply */
	SetHighVoltage(0x00);
	
}

/* ******************************************************************
 * Main loop
 * Do initializations
 * then loop while monitoring the user input.
 * 
 */
int main(void){
    unsigned char theString[12];
    unsigned long theLong;
    
	/* Initialize the PIC modules */
	InitializeMicrocontroller();
	InitializeTimers();
	InitializeUSB();
	InitializeSPI();
	SetUpADC();
    SetUpDAC();

	/* Initialize the HV supply */
	HaltMeasurement();

	/* Initialize variables */
    HistogramOverflow = FALSE;
    StartMeasurement = FALSE;
	StopMeasurement = FALSE;
    ContinueMeasurement = FALSE;
	MeasurementRunning = FALSE;
	MeasurementDone = FALSE;
    ADC_TemperatuerReady = FALSE;
    ADC_HVReady = FALSE;
    IntegratorOn = FALSE;
	ProcessState = Process_Idle;

	/* Get user settings from flash */
	if (!ReadSettingsFromFlash())
	{
		/* If flash not valid put in defaults */
        ADC_TriggerFromPeak = TRUE;
        ADC_TriggerFromDelay = FALSE;
        ADC_TriggerFromThreshold = FALSE;
        
        /* NS_PER_ADC_CLOCK * ADC_TriggerDelayCount delay from threshold trigger to ADC trigger */
		ADC_TriggerDelayCount = 16;
        
        /* SiPM bias voltage 25V */
        HighVoltageCentivolts = 2500; 
        
        /* Set the threshold voltage in centivolts */
        DAC_Centivolts = 10;

        /* UART Baud rate 38400 */
        theLong = ((FMcycle * 1000000)/(38400 * 16)) - 1;
		BaudRateDivisor = (unsigned char)theLong;
		UART2BRGH = FALSE;
	}
    

    /* Needed to wait for the Baud rate to be set */
	InitializeUARTs();

	/* Send wake up message to USB 
     * We need to wait because the UART has just woken up.
     * The two newlines are used because the second char at
     * startup is lost somehow...
     */
    Wait(2);
	PutU2QString("\n\rTest PCB running\n\r");
    PutU2QString("SiPM bias set to ");
    DeciIntToString(HighVoltageCentivolts, theString, 2);
    PutU2String(theString);
    PutU2QString("V\n\r");

	/* Top level loop */
	while (1) {
		if (charsInU2ReceiveBuffer){
			ParseUserInput();
		} 
		switch (ProcessState) {
			case Process_Idle:
				if (StartMeasurement) {
					ProcessState = Process_InitializeMeasurement;
				}
				if (StopMeasurement) {
					ProcessState = Process_StopMeasuring;
				}	
			break;

			case Process_InitializeMeasurement:
				InitializeMeasurement();
				StartMeasurement = FALSE;
				MeasurementRunning = TRUE;
				ProcessState = Process_Measuring;
			break;

			case Process_Measuring:
				if (StopMeasurement) {
					ProcessState = Process_StopMeasuring;
				}				
			break;

			case Process_StopMeasuring:
                StopMeasurement = FALSE;
                MeasurementRunning = FALSE;
				HaltMeasurement();
				ProcessState = Process_Idle;
			break;
        }
    }
    /* Never get here */
	return 0;
}

