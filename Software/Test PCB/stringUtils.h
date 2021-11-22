/*
 * stringUtils.h
 * Public declarations for stringUtils.c
 */

/*
 * $Log: stringUtils.h,v $
 * Revision 1.1.1.1  2012/09/13 15:20:33  Dexter
 * Initial check in
 *
 * Revision 1.1.1.1  2012/05/21 21:56:53  Dexter
 * Initial check in
 *
 * Revision 1.2  2011/06/21 16:19:58  Dexter
 * Initial check in
 *
 */

#include "types.h"

extern Bool CharIsNumeric(char theChar);
extern Bool CharIsAlpha(char theChar);
extern int HexToInt(unsigned char *theString);
extern long HexToLong(unsigned char *theString);
extern int StringToInt(unsigned char * theString);
extern unsigned long StringToLong(unsigned char * theString);
int SignedDecimalStringToInt(unsigned char * theString, unsigned int precision);
extern int StringToValues(unsigned char *theString, int *theArray);
extern void CharToHex(unsigned char theChar, unsigned char * theString);
extern void IntToHex(int theInt, unsigned char * theString);
extern void LongToString(long theLong, unsigned char * theString);
extern void IntToString(int theInt, unsigned char * theString);
extern void DeciIntToString(int theInt, unsigned char * theString, int precision);
extern void CharToString(unsigned char theChar, unsigned char * theString);
extern Bool StringsEqual(unsigned char *string1, unsigned char *string2, unsigned int length);
extern void StringCopy(unsigned char *fromString, unsigned char *toString, unsigned int length);

extern void BCDToASCII(unsigned char theChar, unsigned char * theString);
extern unsigned char ASCIIStringToBCD(unsigned char * theString);
