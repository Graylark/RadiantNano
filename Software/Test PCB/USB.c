/*
 * USB.c
 * Routines to connect PC USB and dsPIC UART.
 * Simple commands from the PC are interpreted
 * and files can be moved between the two.
 */


#include "Test PCB.h"
#include "DAC104.h"
#include "MAX1932.h"
#include "PICFlash.h"
#include "stringUtils.h"
#include "timers.h"
#include "UART.h"

/* Variables */
unsigned char CommandString[12];
int	CommandStringIndex;


/* Defines */ 
#define SPACE 0x20
#define BACK_SPACE 0x08
#define DELETE 0x127


/* Functions */

/*
 * Initialize some variables
 */
void InitializeUSB(void) {

	for (CommandStringIndex=11; CommandStringIndex>=0;CommandStringIndex--) {
		CommandString[CommandStringIndex] = 0;
	}
	CommandStringIndex = 0;
}

/* 
 * Parse GetU2String() output and handle backspaces.
 * Put resulting string in CommandString and 
 * return new length.
 */
int GetCommandString(void) {
	unsigned int theLength;
	unsigned int deletions=0;
	unsigned int index;
	char theString[32];
	unsigned char * pString;
	int theReturn=0;

	SetTimer(10000);

	pString = CommandString;

	/* Handle single SPACE as special character. */
	if ((1 == charsInU2ReceiveBuffer) && (SPACE == *pU2ReceiveBufferRead)) {
		*pString++ = GetU2Char();
		*pString = 0x00;
		theReturn = 2;
	} else {
		/* Get normal command string */
		theLength = GetU2String(theString);
	
		if (theLength > 0) {
			/* Handle backspace characters */
			for (index=0; index<theLength; index++) {
				if ((BACK_SPACE == theString[index]) || (DELETE == theString[index])) {
					if (index > deletions) {
						deletions += 1;
						pString--;
					}
					deletions += 1;
				} else {
					*pString++ = theString[index];
				}			
			}
		}
		
		theReturn = theLength - deletions;
	}
	
	return theReturn;
}

/*
 * Routine to parse user input into commands.
 */
void ParseUserInput(void) {   
    char theChar;
	unsigned char theString[12];
	unsigned int theInt;
	unsigned long theLong;
	char * pChar;
	unsigned short index;

	/* Build user input string */
	SetTimer(10000);
	CommandStringIndex = GetCommandString();
	
	/* Got \r, so parse string */
	switch (CommandString[0]) {
		case 'a':
		break;
        
		case 'A':
                /* Status message*/
                PutU2QString("Temperature: ");
                theInt = GetTemperature();
                IntToString(theInt, theString);
                PutU2String(theString);
                PutU2QString("C\n\r");

                PutU2QString("SiPM bias voltage set: ");
                DeciIntToString(HighVoltageCentivolts, theString, 2);
                PutU2String(theString);
                if (HighVoltageOn) {
                    PutU2QString("  - on, Actual: ");
                } else {
                    PutU2QString(" - off, Actual:");
                }
                theInt = GetHighVoltage();
                DeciIntToString(theInt, theString, 2);
                PutU2String(theString);
                PutU2QString("\n\r");
                
                if (ADC_TriggerFromPeak) {
                    PutU2QString("Trigger from peak detect\n\r");
                }
                if (ADC_TriggerFromDelay) {
                    PutU2QString("Trigger from delay after threshold detect\n\r");
                }
                PutU2QString("Trigger delay: ");
                theInt = ADC_TriggerDelayCount * NS_PER_ADC_CLOCK;
                IntToString(theInt, theString);
                PutU2String(theString);
                PutU2QString("ns, (");
                IntToString(ADC_TriggerDelayCount, theString);
                PutU2String(theString);
                PutU2QString(" clock cycles)\n\r");
                
                PutU2QString("Trigger threshold voltage: ");
                DeciIntToString(DAC_Centivolts, theString, 2);
                PutU2String(theString);
                PutU2QString("V, DAC ");
                if (DAC_On) {
                    PutU2QString("on\n\r");
                } else {
                    PutU2QString("off \n\r");
                }
                
                PutU2QString("Integrator ");
                if (IntegratorOn) {
                    PutU2QString("on\n\r");
                } else {
                    PutU2QString("off\n\r");
                }
                
                PutU2QString("Amplifier DAC value: ");
                IntToString(DAC_Values[0], theString);
                PutU2String(theString);
                PutU2QString("\n\rIntegrator DAC value: ");
                IntToString(DAC_Values[3], theString);
                PutU2String(theString);
                PutU2QString("\n\r");
                
                PutU2Char(0x00);
		break;

		case 'B':
			/* Set Baud rate value */
			PutU2QString("Set Baud rate\n\r");
			PutU2QString("1 -> 1MBaud\n\r");
			PutU2QString("3 -> 3MBaud\n\r");
			PutU2QString("L -> 38.4kBaud\n\r");
			PutU2QString("Enter selection: ");
			CommandStringIndex = GetCommandString();
			if (CommandStringIndex > 1) {

				switch (CommandString[0]) {
				case'1':
					U2MODEbits.BRGH = TRUE;
					BaudRateDivisor = (FMcycle/4) - 1;
					U2BRG = BaudRateDivisor;
					U2MODEbits.BRGH = TRUE;
					UART2BRGH = TRUE;
				break;
				/* This case does not work for FMcycle = 60
				case'2':
					U2MODEbits.BRGH = TRUE;
					BaudRateDivisor = (FMcycle/8) - 1;
					BaudRateDivisor = 5;
					U2BRG = BaudRateDivisor;
					U2MODEbits.BRGH = TRUE;
				break;
				*/
				case'3':
					U2MODEbits.BRGH = TRUE;
					BaudRateDivisor = (FMcycle/12) - 1;
					U2BRG = BaudRateDivisor;
					U2MODEbits.BRGH = TRUE;
					UART2BRGH = TRUE;
				break;
				case'L':
					U2MODEbits.BRGH = FALSE;
					theLong = ((FMcycle * 1000000)/(38400 * 16)) -1;
					BaudRateDivisor = (unsigned char)theLong;
					U2BRG = BaudRateDivisor;
					UART2BRGH = FALSE;
				break;
				default:
					PutU2QString("Invalid entry\n\r");
				break;
				}
			}
			PutU2Char(0x00);
		break;


		case 'b':
		break;
        
        case 'd':
            /* Set delay from threshold trigger to ADC trigger in 20ns steps */
            if ('+' == CommandString[1]) {
                PutU2QString("Enable ADC trigger from delay\n\r");
                ADC_TriggerFromDelay = TRUE;
            } else if ('-' == CommandString[1]) {
                PutU2QString("Disable ADC trigger from delay\n\r");
                ADC_TriggerFromDelay = FALSE;
            } else {
                PutU2QString("Enter ADC trigger delay count (");
                theInt = NS_PER_ADC_CLOCK;
                IntToString(theInt, theString);
                PutU2String(theString);
                PutU2QString("ns per count)\n\r");
                CommandStringIndex = GetCommandString();
                theInt = StringToInt(CommandString);
                if (theInt > 25) {
                    theInt = 25;
                } else if (theInt < 10) { 
                    theInt = 10;
                }
                ADC_TriggerDelayCount = theInt;
                IntToString((theInt * NS_PER_ADC_CLOCK), theString);
                PutU2QString("Delay ");
                PutU2String(theString);
                PutU2QString("ns\n\r");
            }
        break;
        
        case'D':
            /* DAC commands */
             if ('+' == CommandString[1]) {
                PutU2QString("Turn on DAC\n\r");
                DAC_On = TRUE;
                DACCTRL1Lbits.DACON = TRUE;
            } else if ('-' == CommandString[1]) {
                PutU2QString("Turn off DAC\n\r");
                DAC_On = FALSE;
                DACCTRL1Lbits.DACON = FALSE;
            } else if ('A' == CommandString[1]) {
                /* Set Amplifier DAC */  
                PutU2QString("Enter amplifier DAC value (0 - 1023):");
                CommandStringIndex = GetCommandString();
                theInt = StringToInt(CommandString);
                if (theInt > 1023) {
                    theInt = 1023;
                }
                AmplifierDAC = theInt;
                IntToString(theInt, theString);
                PutU2String(theString);
                PutU2QString("\n\r");
                SetDAC(AMPLIFIER_DAC, theInt);
            }else if ('I' == CommandString[1]) {
                /* Set Integrator DAC */  
                PutU2QString("Enter integrator DAC value (0 - 1023):");
                CommandStringIndex = GetCommandString();
                theInt = StringToInt(CommandString);
                if (theInt > 1023) {
                    theInt = 1023;
                }
                IntegratorDAC = theInt;
                IntToString(theInt, theString);
                PutU2String(theString);
                PutU2QString("\n\r");
                SetDAC(INTEGRATOR_DAC, theInt);
            } else if ('T' == CommandString[1]) {
                PutU2QString("Test DAC\n\r");
                SetDAC(TEST_DAC, 512);
                DAC_On = TRUE;
                DACCTRL1Lbits.DACON = TRUE;
            }else {
                PutU2QString("Enter DAC mV (50 - 950):");
                CommandStringIndex = GetCommandString();
                theInt = StringToInt(CommandString);
                if (theInt > 950) {
                    theInt = 950;
                } else if (theInt < 50) { 
                    theInt = 50;
                }
                IntToString(theInt, theString);
                PutU2String(theString);
                PutU2QString("mV\n\r");
                DAC_Centivolts = theInt/10;
                SetDACVoltage();
            }
        break;

		case 'E':
		break;

		case 'f':
		break;

		case 'F':
			/* Save or restore Settings values in flash */
			if ('S' == CommandString[1]) {
				PutU2QString("Saving settings to flash\n\r");
				SaveSettingsToFlash();
			} else if ('R' == CommandString[1]) {
				PutU2QString("Retrieving settings from flash\n\r");
				ReadSettingsFromFlash();
			} else {
				PutU2QString("FS saves settings, FR retrieves settings\n\r");
			}	
			PutU2Char(0x00);	
		break;

		case 'G':
		break;

		case 'H':
			/* Set high voltage */
			if ('?' == CommandString[1]) {
				PutU2QString("SiPM high voltage set: ");
                DeciIntToString(HighVoltageCentivolts, theString, 2);
                PutU2String(theString);
                PutU2QString(", actual: ");
                theInt = GetHighVoltage();
                DeciIntToString(theInt, theString, 2);
                PutU2String(theString);
                PutU2QString("\n\r");
			} else if ('+' == CommandString[1]) {
                SetHighVoltage(HighVoltageCentivolts);
                PutU2QString("Turn on high voltage\n\r");
            } else if ('-' == CommandString[1]) {
                SetHighVoltage(0);
                PutU2QString("Turn off high voltage\n\r");
            }else {
                PutU2QString("Enter power supply voltage (20.0 - 50.0)");
                CommandStringIndex = GetCommandString();
                theInt = SignedDecimalStringToInt(CommandString, 2);
                if (theInt > HV_MAX_CENTIVOLT) {
                    theInt = HV_MAX_CENTIVOLT;
                } else if (theInt < HV_MIN_CENTIVOLT) { 
                    theInt = HV_MIN_CENTIVOLT;
                }
                HighVoltageCentivolts = theInt;
                DeciIntToString(theInt, theString, 2);
                PutU2String(theString);
                PutU2QString("\n\r");
			}
			PutU2Char(0x00);
		break;

		case 'I':
            /* Integrator (low true)*/
            if ('+' == CommandString[1]) {
                Integrator_Enable_LatchBit = 0;
                IntegratorOn = TRUE;
                PutU2QString("Turn on integrator\n\r");
            } else if ('-' == CommandString[1]) {
                Integrator_Enable_LatchBit = 1;
                IntegratorOn = FALSE;
                PutU2QString("Turn off integrator\n\r");
            } else if ('Z' == CommandString[1]) {
                Integrator_Zero_LatchBit = 0;
                for(theInt= 50; theInt>0; theInt-- ) {
                    theLong = theInt;
                }
                Integrator_Zero_LatchBit = theLong;
                PutU2QString("Integrator zero\n\r");
            }
            
		break;

		case 'J':
		break;

		case 'K':
		break;

		case 'k':
			/* Timer test */
			PutU2QString("1 ");
			Wait(1000);
			PutU2QString("2 ");
			Wait(1000);
			PutU2QString("3 ");
			Wait(1000);
			PutU2QString("4\n\r");
			PutU2Char(0x00);
		break;

		case 'L':
		break;

		case 'm':
		break;

		case 'M':
		break;

		case 'O':
		break;

		case 'P':
		break;

		case 'Q':
		break;

		case 'r':
			/* DEBUG: show RCON register */
			theChar = RCON;
			CharToHex(theChar, theString);
			PutU2String(theString);
			PutU2QString("\n\r");
		break;

		case 'R':
		break;

		case 's':
			/* Stop measurement */
            PutU2QString("Stopping collection\n\r");
			StopMeasurement = TRUE;
			PutU2Char(0x00);
		break;

		case 'S':
			/* Start the measurement */
            if ('C' == CommandString[1]) {
                /* continue collection without zeroing out histogram */
                ContinueMeasurement = TRUE;
                PutU2QString("Continuing collection\n\r");
            } else {
                PutU2QString("Starting collection\n\r");
            }
			StartMeasurement = TRUE;
			PutU2Char(0x00);
		break;			

		case 't':
			/* Get the temperature */
			PutU2QString("PCB temperature: ");
    		theInt = GetTemperature();        
			IntToString(theInt, theString);
			PutU2String(theString);
            PutU2QString("C\n\r");
			PutU2Char(0x00);
		break;
        
        
        case 'T':
            /* ADC triggering */
            if ('D' == CommandString[1]) {
                /* set trigger from delay */
                ADC_TriggerFromDelay = TRUE;
                ADC_TriggerFromThreshold =  TRUE;
                ADC_TriggerFromPeak = FALSE;
                PutU2QString("Trigger from delay after threshold detect\n\r");
                ADC_TRIGGER_RPIR = ADC_TRIGGER_THRESHOLD_RP;
                PutU2QString("Trigger delay: ");
                theInt = ADC_TriggerDelayCount * NS_PER_ADC_CLOCK;
                IntToString(theInt, theString);
                PutU2String(theString);
                PutU2QString("ns, (");
                IntToString(ADC_TriggerDelayCount, theString);
                PutU2String(theString);
                PutU2QString(" clock cycles)\n\r");
            } else if ('P' == CommandString[1]) {
                 /* set trigger from peak detect */
                ADC_TriggerFromDelay = FALSE;
                ADC_TriggerFromPeak = TRUE;
                ADC_TRIGGER_RPIR = ADC_TRIGGER_PEAK_RP;
                PutU2QString("Trigger from peak detect\n\r");
            } else if ('T' == CommandString[1]) {
                /* set threshold voltage in centivolts */
                PutU2QString("Enter trigger threshold voltage: (0.08 - 1.55V");
                CommandStringIndex = GetCommandString();
                theInt = SignedDecimalStringToInt(CommandString, 2);
                if (theInt > 155) {
                    theInt = 155;
                } else if (theInt < 8) { 
                    theInt = 8;
                }
                DAC_Centivolts = theInt;
                DeciIntToString(theInt, theString, 2);
                PutU2String(theString);
                PutU2QString("\n\r");
            }
        break;

		case 'u':
		break;

		case 'U':
            /* Upload the histogram */
            SendHistogram();
		break;

		case 'V':
		break;

		case 'v':
			/* build version */
            PutU2QString("Build date 11/9/2021 8:00PM\n\r");
#if PCB_VERSION == 1
            PutU2QString(", Hardware revision 1\n\r");
#endif
		break;

		case 'W':
		break;

		case 'Y':
		break;


		case 'Z':
			/* Zero the histogram */
            for (index=0; index<HISTOGRAM_SIZE; index++) {
                Histogram[index] = 0;
            }

			PutU2QString("Histogram cleared\n\r");
		break;

		case SPACE:
		break;
	
		case '?':
			/* Help command */
			PutU2QString("Commands:\n\r");
			PutU2QString("A  - Show full status\n\r");
			PutU2QString("B  - Set Baud rate\n\r");
            PutU2QString("D  - Set DAC output\n\r");
            PutU2QString("d  - Set ADC trigger delay\n\r");
            PutU2QString("d+ - Enable ADC trigger delay\n\r");
            PutU2QString("d- - Disable ADC trigger delay\n\r");
			PutU2QString("FS - Save settings to flash\n\r");
			PutU2QString("FR - Restore settings from flash\n\r");
			PutU2QString("H  - Set SiPM high voltage\n\r");
			PutU2QString("H? - Show SiPM high voltage\n\r");
            PutU2QString("H+ - Turn on high voltage\n\r");
            PutU2QString("H- - Turn off high voltage\n\r");
            PutU2QString("I+ - Turn on integrator\n\r");
            PutU2QString("I- - Turn off integrator\n\r");
            PutU2QString("IZ - Integrator zero\n\r");
			PutU2QString("k  - Timer test\n\r");
			PutU2QString("r  - Show rcon register\n\r");
			PutU2QString("S  - Start measuring\n\r");
			PutU2QString("s  - Stop measuring, turn off HV\n\r");
			PutU2QString("t  - Get board temperature\n\r");
            PutU2QString("TD - Set ADC trigger from delay after threshold detect\n\r");
            PutU2QString("TP - Set ADC trigger from peak detect\n\r");
            PutU2QString("TT - Set ADC trigger threshold voltage\n\r");
            PutU2QString("U  - Upload histogram via USB\n\r");
			PutU2QString("v  - Show build version\n\r");
			PutU2QString("Z  - Zero histogram\n\r");
			PutU2Char(0x00);
		break;

		case '\n':
		case '\r':
			PutU2QString("DB\n\r");
			PutU2Char(0x00);
		break;

		default:
			PutU2QString("Unknown Command: ");
			PutU2Char(CommandString[0]);
			PutU2QString("\n\r");
			pChar = receiveFromU2Buffer;
			while (pChar < pU2ReceiveBufferWrite) {
				CharToHex(*pChar++, theString);
				PutU2String(theString);
				PutU2Char(' ');
			}
			PutU2QString("\n\r");
			PutU2Char(0x00);
			CommandStringIndex = 0;
		break;
	}

}
