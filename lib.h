/**
** @file lib.h
**
** @author Warren R. Carithers
**
** Library of kernel support functions
**
** This module implements a simple collection of support functions
** similar to the standard C library.
*/

#ifndef LIB_H_
#define LIB_H_

#include "common.h"

#include "compat.h"

/**
** __put_char_or_code( ch )
**
** prints the character on the console, unless it is a non-printing
** character, in which case its hex code is printed
*/
void __put_char_or_code( int ch );

/**
** Name:	__bound
**
** Description:	This function confines an argument within specified bounds.
**
** @param min    Lower bound
** @param value  Value to be constrained
** @param max    Upper bound
**
** @return The constrained value
*/
unsigned int __bound( unsigned int min, unsigned int value,
                        unsigned int max );

/**
** Name:	__memset
**
** Description:	initialize all bytes of a block of memory to a specific value
**
** @param buf    The buffer to initialize
** @param len    Buffer size (in bytes)
** @param value  Initialization value
*/
void __memset( void *buf, register unsigned int len,
               register unsigned int value );

/**
** Name:	__memclr
**
** Description:	Initialize all bytes of a block of memory to zero
**
** @param buf    The buffer to initialize
** @param len    Buffer size (in bytes)
*/
void __memclr( void *buf, register unsigned int len );

/**
** Name:	__memcpy
**
** Description:	Copy a block from one place to another
**
** May not correctly deal with overlapping buffers
**
** @param dst   Destination buffer
** @param src   Source buffer
** @param len   Buffer size (in bytes)
*/
void __memcpy( void *dst, register const void *src,
               register unsigned int len );

/**
** Name:        __strlen
**
** Description: Calculate the length of a C-style string.
**
** @param str  The string to examine
**
** @return The length of the string
*/
unsigned int __strlen( register const char *str );

/**
** Name:        __strcmp
**
** Description: Compare two strings
**
** @param s1   The first string to examine
** @param s2   The second string to examine
**
** @return < 0 if s1 < s2; 0 if equal; > 0 if s1 > s2
*/
int __strcmp( register const char *s1, register const char *s2 );

/**
** Name:        __strcpy
**
** Description: Copy a string into a destination buffer
**
** May not correctly deal with overlapping buffers
**
** @param dst   The destination buffer
** @param src   The source buffer
**
** @return The destination buffer
*/
char *__strcpy( register char *dst, register const char *src );

/**
** Name:	__strcat
**
** Description: Append one string to another
**
** @param dst   The destination buffer
** @param src   The source buffer
**
** @return The destination buffer
**
** NOTE:  May not correctly deal with overlapping buffers.  Assumes
** dst is large enough to hold the resulting string.
*/
char *__strcat( register char *dst, register const char *src );

/**
** Name:        __pad
**
** Description: Generate a padding string
**
** @param dst     Pointer to where the padding should begin
** @param extra   How many padding bytes to add
** @param padchar What character to pad with
**
** @return Pointer to the first byte after the padding
*/
char *__pad( char *dst, int extra, int padchar );

/**
** Name:        __padstr
**
** Description: Add padding characters to a string
**
** @param dst        The destination buffer
** @param str        The string to be padded
** @param len        The string length, or -1
** @param width      The desired final length of the string
** @param leftadjust Should the string be left-justified?
** @param padchar    What character to pad with
**
** @return Pointer to the first byte after the padded string
*/
char *__padstr( char *dst, char *str, int len, int width,
                   int leftadjust, int padchar );

/**
** Name:	__sprint
**
** Description: Formatted output into a string buffer
**
** @param dst   The destination buffer
** @param fmt   The format string
**
** The format string parameter is followed by zero or more additional
** parameters which are interpreted according to the format string.
**
** NOTE:  assumes the buffer is large enough to hold the result string
**
** NOTE:  relies heavily on the x86 convention that parameters
** are pushed onto the stack in reverse order as 32-bit values.
*/
void __sprint( char *dst, char *fmt, ... );

/**
** Conversion functions
*/

/**
** Name:	__cvtdec0
**
** Description:	Convert an integer value into a decimal character string.
**
** @param buf    Result buffer
** @param value  Value to be converted
**
** @return The result buffer
*/
char *__cvtdec0( char *buf, int value );

/**
** Name:	__cvtdec
**
** Description:	Convert an integer value into a decimal character string.
**
** @param buf    Result buffer
** @param value  Value to be converted
**
** @return The result buffer
*/
int __cvtdec( char *buf, int value );

extern char __hexdigits[];

/**
** Name:	__cvthex
**
** Description:	Convert an integer value into a hex character string.
**
** @param buf    Result buffer
** @param value  Value to be converted
**
** @return The result buffer
*/
int __cvthex( char *buf, int value );

/**
** Name:	__cvtoct
**
** Description:	Convert an integer value into an octal character string.
**
** @param buf    Result buffer
** @param value  Value to be converted
**
** @return The result buffer
*/
int __cvtoct( char *buf, int value );

/**
** Name:	__inb, __inw, __inl
**
** Description:	Read one byte, word, or longword from an input port
**
** @param port   The port from which to read
**
** @return The data from that port
*/
int __inb( int port );
int __inw( int port );
int __inl( int port );

/**
** Name:	__outb, __outw, __outl
**
** Description:	Write one byte, word, or longword to an output port
**
** @param port   The port to be written to
** @param data   The data to write to that port
*/
void __outb( int port, int value );
void __outw( int port, int value );
void __outl( int port, int value );

/**
** Name:	__get_flags
**
** Description:	Get the current processor flags
**
** @return The EFLAGS register after entry to this function
*/
unsigned int __get_flags( void );

/**
** Name:	__pause
**
** Description:	Pause until something happens
*/
void __pause( void );

/**
** __get_ra:
**
** Description: Get the return address for the calling function
**              (i.e., where whoever called us will go back to)
**
** @return The address the calling routine will return to as a uint32_t
*/
uint32_t __get_ra( void );

/**
** _pcount - count the number of active processes in each state
**
** @param counts  Array into which the counts are placed
** @return The number of active processes
*/
int _pcount( register uint32_t *counts );

/**
** _kpanic - kernel-level panic routine
**
** usage:  _kpanic( msg )
**
** Prefix routine for __panic() - can be expanded to do other things
** (e.g., printing a stack traceback)
**
** @param msg  String containing a relevant message to be printed
*/
void _kpanic( char *msg );

#endif
