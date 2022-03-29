/**
** @file sio.h
**
** @author Warren R. Carithers
**
** SIO definitions
*/

#ifndef SIO_H_
#define SIO_H_

/*
** General (C and/or assembly) definitions
*/

// sio interrupt settings

#define SIO_TX      0x01
#define SIO_RX      0x02
#define SIO_BOTH    (SIO_TX | SIO_RX)

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
*/

#include "common.h"

#include "queues.h"

#include "compat.h"

/*
** PUBLIC GLOBAL VARIABLES
*/

// queue for read-blocked processes
extern queue_t _reading;

/*
** PUBLIC FUNCTIONS
*/

/**
** _sio_init()
**
** Initialize the UART chip.
*/
void _sio_init( void );

/**
** _sio_enable()
**
** Enable SIO interrupts
**
** usage:    uint8_t old = _sio_enable( uint8_t which )
**
** @param which   Bit mask indicating which interrupt(s) to enable
**
** @return the prior IER setting
*/
uint8_t _sio_enable( uint8_t which );

/**
** _sio_disable()
**
** Disable SIO interrupts
**
** usage:    uint8_t old = _sio_disable( uint8_t which )
**
** @param which   Bit mask indicating which interrupt(s) to disable
**
** @return the prior IER setting
*/
uint8_t _sio_disable( uint8_t which );

/**
** _sio_inq_length()
**
** Get the input queue length
**
** usage:   int num = _sio_inq_length()
**
** @return the count of characters still in the input queue
*/
int _sio_inq_length( void );

/**
** _sio_readc()
**
** Get the next input character
**
** usage:   int ch = _sio_readc()
**
** @return the next character, or -1 if no character is available
*/
int _sio_readc( void );

/**
** _sio_reads()
**
** Read the entire input buffer into a user buffer of a specified size
**
** usage:    int num = _sio_reads( char *buffer, int length )
**
** @param buf     The destination buffer
** @param length  Length of the buffer
**
** @return the number of bytes copied, or 0 if no characters were available
*/
int _sio_reads( char *buffer, int length );

/**
** _sio_writec( ch )
**
** Write a character to the serial output
**
** usage:   _sio_writec( int ch )
**
** @param ch   Character to be written (in the low-order 8 bits)
*/
void _sio_writec( int ch );

/**
** _sio_write( ch )
**
** Write a buffer of characters to the serial output
**
** usage:   int num = _sio_write( const char *buffer, int length )
**
** @param buffer   Buffer containing characters to write
** @param length   Number of characters to write
**
** @return the number of characters copied into the SIO output buffer
*/
int _sio_write( const char *buffer, int length );

/**
** _sio_puts( buf )
**
** Write a NUL-terminated buffer of characters to the serial output
**
** usage:   n = _sio_puts( const char *buffer );
**
** @param buffer  The buffer containing a NUL-terminated string
**
** @return the count of bytes transferred
*/
int _sio_puts( const char *buffer );

/**
** _sio_dump( full )
**
** Dump the contents of the SIO buffers to the console
**
** usage:    _sio_dump(true) or _sio_dump(false)
**
** @param full   Boolean indicating whether or not a "full" dump
**               is being requested (which includes the contents
**               of the queues)
*/
void _sio_dump( bool_t full );

#endif
/* SP_ASM_SRC */

#endif
