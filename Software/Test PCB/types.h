/*
 * types.h
 *
 * declaration of some generic types
 */

/*
 * $Log: types.h,v $
 * Revision 1.1.1.1  2012/09/13 15:20:33  Dexter
 * Initial check in
 *
 * Revision 1.1.1.1  2012/05/21 21:56:53  Dexter
 * Initial check in
 *
 * Revision 1.2  2011/06/21 16:20:45  Dexter
 * Initial check in
 *
 */

#ifndef TYPES_DEFINED

typedef unsigned char Bool;
#define FALSE 0
#define TRUE 1

typedef unsigned char u_char;
typedef char s_char;
typedef int s_int;

typedef union {
	struct {
		unsigned bit0:1;
		unsigned bit1:1;
		unsigned bit2:1;
		unsigned bit3:1;
		unsigned bit4:1;
		unsigned bit5:1;
		unsigned bit6:1;
		unsigned bit7:1;
	};
	struct {
		unsigned full:8;
	};	
} BYTE_BITS;

typedef union {
	unsigned int full;
	struct {
		unsigned char low;
		unsigned char high;
	} byte;
} u_int;

typedef union {
	unsigned long full;
	struct {
		unsigned char low;
		unsigned char high;
		unsigned char upper;
		unsigned char top;
	} byte;
	struct {
		unsigned int low;
		unsigned int high;
	} uint;
} AddressType;

#define TYPES_DEFINED TRUE
#endif
