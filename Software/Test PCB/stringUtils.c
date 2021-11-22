/*
 * stringUtils.c
 * Routines to handle strings.
 */

/*
 * $Log: stringUtils.c,v $
 * Revision 1.1.1.1  2012/09/13 15:20:33  Dexter
 * Initial check in
 *
 * Revision 1.1.1.1  2012/05/21 21:56:53  Dexter
 * Initial check in
 *
 * Revision 1.2  2011/06/21 16:19:42  Dexter
 * Initial check in
 *
 */

#include "types.h"

/* ***********************************************
 * Some test routines
 */

/*
 * Tests whether a char can be part of a hex number.
 */
Bool CharIsHex(char theChar) {
	Bool theReturn=FALSE;

	if (('x' == theChar) || ('X' == theChar)) {
		theReturn = TRUE;
	} else if (('0' <= theChar) && (theChar <= '9')) {
		theReturn = TRUE;
	} else if (('a' <= theChar) && (theChar <= 'f')) {
		theReturn = TRUE;
	} else if (('A' <= theChar) && (theChar <= 'F')) {
		theReturn = TRUE;
	}
	return theReturn;
}

/*
 * Tests whether a char can be part of a number.
 */
Bool CharIsNumeric(char theChar) {
	Bool theReturn=FALSE;

	if (('0' <= theChar) && (theChar <= '9')) {
		theReturn = TRUE;
	}

	return theReturn;
}

/*
 * Tests whether a char is in alphabet.
 */
Bool CharIsAlpha(char theChar) {
	Bool theReturn=FALSE;

	if (('a' <= theChar) && (theChar <= 'z')) {
		theReturn = TRUE;
	} else if (('A' <= theChar) && (theChar <= 'Z')) {
		theReturn = TRUE;
	}
	return theReturn;
}

/* *************************************************
 * Routines to turn strings into values
 */

/*
 * Turn an ASCII hex string to an int. The hex can be
 * terminated by any non-hex cahracter.
 */
int HexNToInt(unsigned char *theString, int theLength) {
	int theReturn=0;
	char temp;
	int power;
	int index=0;

	/* Verify prefix */
	if ('0' == theString[0]) {
		if (('x' == theString[1]) || ('X' == theString[1])) {

			power = 1;
			for (index=theLength-1; index>=0; index--) {
				temp = theString[index];
				if (('0' <= temp) && (temp <= '9')) {
					theReturn += (temp - '0') * power;
				} else if (('a' <= temp) && (temp <= 'f')){
					theReturn += (temp - 'a' + 10) * power;
				} else if (('A' <= temp) && (temp <= 'F')){
					theReturn += (temp - 'A' + 10) * power;
				}
				power *= 16;
			}
		} 
	}
	return theReturn;
}


/*
 * Turn an ASCII hex string to an int. The hex can be
 * terminated by any non-hex character.
 */
int HexToInt(unsigned char *theString) {
	int theReturn=0;
	char temp;
	int power;
	int index=0;

	/* Verify prefix */
	if ('0' == theString[0]) {
		if (('x' == theString[1]) || ('X' == theString[1])) {

			/* find end of string */
			index = 2;
			while (CharIsHex(theString[index])) {
				index++;
			}

			power = 1;
			for (index--; index>1; index--) {
				temp = theString[index];
				if (('0' <= temp) && (temp <= '9')) {
					theReturn += (temp - '0') * power;
				} else if (('a' <= temp) && (temp <= 'f')){
					theReturn += (temp - 'a' + 10) * power;
				} else if (('A' <= temp) && (temp <= 'F')){
					theReturn += (temp - 'A' + 10) * power;
				}
				power *= 16;
			}
		} 
	}
	return theReturn;
}

/*
 * Turn an ASCII hex string to a long. The hex can be
 * terminated by any non-hex character.
 */
long HexToLong(unsigned char *theString) {
	long theReturn=0;
	char temp;
	int power;
	int index=0;

	/* Verify prefix */
	if ('0' == theString[0]) {
		if (('x' == theString[1]) || ('X' == theString[1])) {

			/* find end of string */
			index = 2;
			while (CharIsHex(theString[index])) {
				index++;
			}

			power = 1;
			for (index--; index>1; index--) {
				temp = theString[index];
				if (('0' <= temp) && (temp <= '9')) {
					theReturn += (temp - '0') * power;
				} else if (('a' <= temp) && (temp <= 'f')){
					theReturn += (temp - 'a' + 10) * power;
				} else if (('A' <= temp) && (temp <= 'F')){
					theReturn += (temp - 'A' + 10) * power;
				}
				power *= 16;
			}
		} 
	}
	return theReturn;
}


/*
 * Turn the ASCII numeric string to an int. The number is
 * terminated by any non numeric char.
 */
int StringToInt(unsigned char * theString) {
	int theReturn=0;
	int temp;
	int index=0;

	/* find end of string */
	while (CharIsNumeric(theString[index])) {
		index++;
	}

	temp = 1;
	for (index--; index>=0; index--) {
		theReturn += (theString[index] - '0') * temp;
		temp *= 10;
	}
	return theReturn;
}

/*
 * Turn the ASCII numeric string to an unsigned  long. 
 * The number is terminated by any non numeric char.
 */
unsigned long StringToLong(unsigned char * theString) {
	unsigned long theReturn=0;
	unsigned long temp;
	int index=0;

	/* find end of string */
	while (CharIsNumeric(theString[index])) {
		index++;
	}

	temp = 1;
	for (index--; index>=0; index--) {
		theReturn += (theString[index] - '0') * temp;
		temp *= 10;
	}
	return theReturn;
}


/*
 * Turn a signed or not signed decimal string to an int. The 
 * input is terminated by any non numeric character.
 * If no leading sign is given, a positive int is generated.
 * For now only one decimal place to the right is allowed, and
 * the int returned is multiplied by 10, so that the fraction
 * is part of the int.
 */
int SignedDecimalStringToInt(unsigned char * theString, unsigned int precision) {
	int theFraction=0;
	Bool negative=FALSE;
	unsigned int index=0;
	int theReturn=0;
    int temp;
    unsigned int fractionPrecision=0;

	if ('-' == theString[0]) {
		negative = TRUE;
		index++;
	} else if ('+' == theString[0]) {
		index++;
	}

	/* Check for no integer portion */
	if ('.' == theString[index]) {
		theFraction = StringToInt(&theString[index+1]);
	} else {
		theReturn = StringToInt(&theString[index]);
	}

	/* Look for a decimal point and get fractional part */
	if (theReturn) {
		while (CharIsNumeric(theString[index])) {
			index++;
		}
		if ('.' == theString[index]) {
			theFraction = StringToInt(&theString[index+1]);
            
            /* get precision of fraction */
			temp = theFraction;
            while (temp > 0) {
                temp /= 10;
                fractionPrecision++;
            }
            
            if (fractionPrecision > precision) {
                do {
                    theFraction /= 10;
                    fractionPrecision--;
                } while (fractionPrecision > precision);
            } else if (fractionPrecision < precision) {
                do {
                    theFraction *= 10;
                    fractionPrecision++;
                } while (fractionPrecision < precision);
            }
		}
	}

	/* Combine them */
    while (precision > 0) {
        theReturn *= 10;
        precision--;
    }
	theReturn += theFraction;
	if (negative) {
		theReturn = -theReturn;
	}

	return theReturn;
}

/*
 * Fill theArray with ints converted from ASCII hex string.
 * The hex values are separated by non-hex ASSCII, 
 * and the string is null terminated.
 * As a precaution, no more than 16 values are allowed.
 * Returns the number of values converted.
 */
int StringToValues(unsigned char *theString, int *theArray) {
	int ArrayIndex;
	int StringIndex=0;
	int HexIndex;
	int theReturn=0;

	for (ArrayIndex=0; ArrayIndex<16; ArrayIndex++) {

		HexIndex = 0;

		/* skip over any non-hex characters */
		while (!CharIsHex(theString[StringIndex])) {
			StringIndex++;
		}
	
		/* Get the hex string */
		while (CharIsHex(theString[StringIndex])) {
			HexIndex++;
			StringIndex++;
		}
		
		/* If there is a hex string,
		 * add null termination to it,
		 * then get value and put it in array
		 */
		if (HexIndex > 2) {
			theArray[ArrayIndex] = HexNToInt(&theString[StringIndex-HexIndex], HexIndex);
			theReturn++;
		}

		/* if end of string, get out */
		if (0 == theString[StringIndex]) {
			break;
		} else if (0x0D == theString[StringIndex]) {
			break;
		}

		/* Skip over non hex to next hex number*/
		StringIndex++;
	}
	return theReturn;
}

/* ********************************************************
 * Routines to convert values into strings.
 */

/*
 * Convert low nibble of byte to hex.
 */
unsigned char NibbleToHex(unsigned char theChar) {
	unsigned char theReturn;

	if ((0x0A <= theChar) &&  (theChar <= 0x0F)) {
		theReturn = theChar + 'a' - 10;	
	} else {
		theReturn = theChar + '0';
	}
	return theReturn;
}

/*
 * Convert a byte to null terminated ASCII hex string.
 */
void CharToHex(unsigned char theChar, unsigned char * theString) {
	theString[0] = '0';
	theString[1] = 'x';
	theString[2] = NibbleToHex((theChar & 0xF0)>>4);
	theString[3] = NibbleToHex((theChar & 0x0F));
	theString[4] = 0x00;
}

/*
 * Convert an int to a null terminated ASCII hex string.
 */
void IntToHex(int theInt, unsigned char * theString) {
	theString[0] = '0';
	theString[1] = 'x';
	theString[2] = NibbleToHex((theInt & 0xF000)>>12);
	theString[3] = NibbleToHex((theInt & 0x0F00)>>8);
	theString[4] = NibbleToHex((theInt & 0x00F0)>>4);
	theString[5] = NibbleToHex((theInt & 0x000F));
	theString[6] = 0x00;
}

/*
 * Convert a long to a null terminated ASCII string.
 */
void LongToString(long theLong, unsigned char * theString) {
	char theChar=0;
	int index=0;
	long test=1000000000;
	Bool started=FALSE;

	while ((theLong > 0) || (test > 0)) {
		while (theLong >= test) {
			theLong -= test;
			theChar++;
			started = TRUE;
		}
		if((theChar > 0) || started) {
			theString[index++] = theChar + '0';
			theChar = 0;
		}
		test /= 10;
	}

	/* case of long=0 */
	if (!started) {
		theString[index++] ='0';
	}

	/* add null termination */
	theString[index] = 0x00;
}

/*
 * Convert an int to a null terminated ASCII string.
 */
void IntToString(unsigned int theInt, unsigned char * theString) {
	char theChar=0;
	unsigned int index=0;
	unsigned int test=10000;
	Bool started=FALSE;

	while ((theInt > 0) || (test > 0)) {
		while (theInt >= test) {
			theInt -= test;
			theChar++;
			started = TRUE;
		}
		if((theChar > 0) || started) {
			theString[index++] = theChar + '0';
			theChar = 0;
		}
		test /= 10;
	}

	/* case of int=0 */
	if (!started) {
		theString[index++] ='0';
	}

	/* add null termination */
	theString[index] = 0x00;
}

/*
 * Signed Deci value to string. Passed value theInt is turned to
 * a string, and a decimal place is added precision places from
 * the right.
 */
void DeciIntToString(int theInt, unsigned char * theString, int precision) {
	Bool negative=FALSE;
	unsigned int index=0;
	unsigned int count=0;
	unsigned int end;
	
	if (theInt < 0) {
		theInt = -theInt;
		negative = TRUE;
		theString[0] = '-';
		index++;
	}
	
	IntToString(theInt, &(theString[index]));

	/* Look for end of string */
	while (CharIsNumeric(theString[index])) {
		index++;
		count++;
	}
	end = index;

	/* add a leading zero if needed */
	if (precision >= count) {
		for (; count>0; count--) {
			index--;
			theString[index + 1] = theString[index];
		}
		theString[index] = '0';
		end++;
	}

	/* If the value is zero, we're done, else.. */
	if (theInt) {
		index = end;

		/* Move the digits and add decimal point. */
		while (precision--) {
			index--;
			theString[index + 1] = theString[index];
		}
		theString[index] = '.';
		end++;
	}

	/* Terminate the string */
	theString[end] = 0;

}

/*
 * Convert an unsigned char to a null terminated ASCII string.
 */
void CharToString(unsigned char theChar, unsigned char * theString) {
	char outChar='0';
	unsigned int index=0;
	unsigned char test=100;
	Bool started=FALSE;

	while ((theChar > 0) || (test > 0)) {
		while (theChar >= test) {
			theChar -= test;
			outChar++;
			started = TRUE;
		}
		if((outChar > 0) || started) {
			theString[index++] = outChar;
			outChar = '0';
		}
		test /= 10;
	}

	/* case of char=0 */
	if (!started) {
		theString[index++] ='0';
	}

	/* add null termination */
	theString[index] = 0x00;
}


/* ******************************************************************
 * String compare routine
 * The calling routine passes pointers to two strings and a length.
 * Returns TRUE if strings equal, FALSE otherwise.
 */
Bool StringsEqual(unsigned char *string1, unsigned char *string2, unsigned int length) {
	Bool theReturn=TRUE;
	unsigned int index;
	char char1;
	char char2;

	for(index=length; index>0; index--) {
		char1 = *string1;
		char2 = *string2;
		if(*string1++ != *string2++) {
			theReturn = FALSE;
			break;
		}
	}

	return theReturn;
}

/* ******************************************************************
 * String copy routine
 * The calling routine passes pointers to the from string and the to string,
 * and a length.
 */
void StringCopy(unsigned char *fromString, unsigned char *toString, unsigned int length) {
	unsigned int index;

	for(index=length; index>0; index--) {
		*toString++ = *fromString++;
	}
}

/*
 * Convert a BCD char to two ASCII digits and
 * insert in the passed string.
 */
void BCDToASCII(unsigned char theChar, unsigned char * theString) {
	char temp;

	temp = theChar >> 4;
	temp += '0';
	*theString++ = temp;
	temp = theChar & 0x0F;
	temp += '0';
	*theString = temp;
}	

/*
 * Convert a string of two ASCII digits to a char of BCD.
 */
unsigned char ASCIIStringToBCD(unsigned char * theString) {
	char theChar;
	unsigned char theReturn;

	theChar = *theString++;
	theChar -= '0';

	theReturn = theChar << 4;

	theChar = *theString++;
	theChar -= '0';

	theReturn |= theChar & 0x0F;

	return theReturn;
}
