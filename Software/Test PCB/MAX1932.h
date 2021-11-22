/*
 * MAX1932.h
 * Public declarations for the MAX1932 SPI interface.
 */

/*
 * $Log: MAX1932.h,v $
 * Revision 1.1.1.1  2012/09/13 15:20:33  Dexter
 * Initial check in
 *
 */

/* Defines */
#define HV_MIN_CENTIVOLT 2000
#define HV_MAX_CENTIVOLT 5000



extern void SetHighVoltage(unsigned int theValue);
