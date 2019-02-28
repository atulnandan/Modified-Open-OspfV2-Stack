
#include <stdarg.h>
#include <string.h>

/* 
 * Machine-dependent and compiler-dependent constants and macros.
 * Defined here for a Intel architecture, and a compiler whose
 * "ints" are 32-bits and "shorts" 16.
 */

// Definition of fixed-length types

typedef unsigned char byte;	// Byte (8-bit unsigned)
typedef	unsigned short uns16;	// 16-bit unsigned
typedef	unsigned uns32;		// 32-bit unsigned
typedef char int8;		// 8-bit signed
typedef	short int16;		// 16-bit signed
typedef	int int32;		// 32-bit signed

typedef unsigned long word;	// Generic pointer

// Constant for fletcher checksum calculation
const int MODX = 4102;

// Machine-dependencies in the socket interface
typedef unsigned int socklen;	// length arg to accept(), etc.
#define PHYBINDADDR "127.255.255.255"

// Convert fields between network and machine byte-order
// This involves byte-swapping on an Intel architecture
	
// Convert a 32-bit unsigned quantity from network to host order

inline uns32 ntoh32(uns32 value)

{
    uns32 swval;

    swval = (value << 24);
    swval |= (value << 8) & 0x00ff0000L;
    swval |= (value >> 8) & 0x0000ff00L;
    swval |= (value >> 24);
    return(swval);
}

// Convert a 32-bit quantity from host to network order

inline uns32 hton32(uns32 value)

{
    return(ntoh32(value));
}

// Convert a 16-bit unsigned quantity from network to host order

inline uns16 ntoh16(uns16 value)

{
    uns16 swval;

    swval = (value << 8);
    swval |= (value >> 8) & 0xff;
    return(swval);
}

// Convert a 16-bit quantity from host to network order

inline uns16 hton16(uns16 value)

{
    return(ntoh16(value));
}


/* Standard utility functions
 * These have been defined in the standard Linux
 * header files above.
 *
 * void	bcopy(const void *from, void *to, size_t);	// Byte copy
 * int bcmp(const void *from, const void *to, size_t);	// Byte compare
 * void	bzero(void *ptr, size_t);		// Zero memory
 * char *strncpy(char *dest, const char *src, size_t n);
 */
