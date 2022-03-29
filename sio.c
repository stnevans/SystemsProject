/**
** @file sio.c
**
** @author Warren R. Carithers
**
** SIO module
**
** Our SIO scheme is very simple:
**
**  Input:  We maintain a buffer of incoming characters that haven't
**      yet been read by processes.  When a character comes in, if
**      there is no process waiting for it, it goes in the buffer;
**      otherwise, the first waiting process is awakeneda and it
**      gets the character.
**
**      When a process invokes readch(), if there is a character in
**      the input buffer, the process gets it; otherwise, it is
**      blocked until input appears
**
**      Communication with system calls is via two routines.
**      _sio_readc() returns the first available character (if
**      there is one), resetting the input variables if this was
**      the last character in the buffer.  If there are no
**      characters in the buffer, _sio_read() returns a -1
**      (presumably so the requesting process can be blocked).
**
**      _sio_reads() copies the contents of the input buffer into
**      a user-supplied buffer.  It returns the number of characters
**      copied.  If there are no characters available, return a -1.
**
**  Output: We maintain a buffer of outgoing characters that haven't
**      yet been sent to the device, and an indication of whether
**      or not we are in the middle of a transmit sequence.  When
**      an interrupt comes in, if there is another character to
**      send we copy it to the transmitter buffer; otherwise, we
**      end the transmit sequence.
**
**      Communication with user processes is via three functions.
**      _sio_writec() writes a single character; _sio_write()
**      writes a sized buffer full of characters; _sio_puts()
**      prints a NUL-terminated string.  If we are in the middle
**      of a transmit sequence, all characters will be added
**      to the output buffer (from where they will be sent
**      automatically); otherwise, we send the first character
**      directly, add the rest of the characters (if there are
**      any) to the output buffer, and set the "sending" flag
**      to indicate that we're expecting a transmitter interrupt.
*/

#define SP_KERNEL_SRC

#include "common.h"

#include <uart.h>
#include "x86arch.h"
#include "x86pic.h"

#include "compat.h"
#include "sio.h"

#include "queues.h"
#include "process.h"
#include "scheduler.h"
#include "kernel.h"

#include "lib.h"

/*
** PRIVATE DEFINITIONS
*/

#define BUF_SIZE    2048

/*
** PRIVATE GLOBALS
*/

    // input character buffer
static char _inbuffer[ BUF_SIZE ];
static char *_inlast;
static char *_innext;
static uint32_t _incount;

    // output character buffer
static char _outbuffer[ BUF_SIZE ];
static char *_outlast;
static char *_outnext;
static uint32_t _outcount;

    // output control flag
static int _sending;

    // interrupt register status
static uint8_t _ier;

/*
** PUBLIC GLOBAL VARIABLES
*/

// queue for read-blocked processes
queue_t READQ;

/*
** PRIVATE FUNCTIONS
*/

/**
** _sio_isr(vector,ecode)
**
** Interrupt handler for the SIO module.  Handles all pending
** events (as described by the SIO controller).
**
** @param vector   The interrupt vector number for this interrupt
** @param ecode    The error code associated with this interrupt
*/
static void _sio_isr( int vector, int ecode ) {
    pcb_t *pcb;
    int eir, lsr, msr;
    int ch;

#if TRACING_SIO_ISR
    __cio_puts( "SIO: int:" );
#endif
    //
    // Must process all pending events; loop until the EIR
    // says there's nothing else to do.
    //

    for(;;) {

        // get the "pending event" indicator
        eir = __inb( UA4_EIR ) & UA4_EIR_INT_PRI_MASK;

        // process this event
        switch( eir ) {

        case UA4_EIR_LINE_STATUS_INT_PENDING:
            // shouldn't happen, but just in case....
            lsr = __inb( UA4_LSR );
            __cio_printf( "** SIO line status, LSR = %02x\n", lsr );
            break;

        case UA4_EIR_RX_INT_PENDING:
#if TRACING_SIO_ISR
    __cio_puts( " RX" );
#endif
            // get the character
            ch = __inb( UA4_RXD );
            if( ch == '\r' ) {    // map CR to LF
                ch = '\n';
            }
#if TRACING_SIO_ISR
    __cio_printf( " ch %02x", ch );
#endif

            //
            // If there is a waiting process, this must be
            // the first input character; give it to that
            // process and awaken the process.
            //

            if( QLENGTH(READQ) > 0 ) {

                QDEQUE( READQ, pcb );
                assert( pcb );

                // return char via arg #2 and count in EAX
                char *buf = (char *) ARG(pcb,2);
                *buf = ch & 0xff;
                RET(pcb) = 1;
                SCHED( pcb );

            } else {

                //
                // Nobody waiting - add to the input buffer
                // if there is room, otherwise just ignore it.
                //

                if( _incount < BUF_SIZE ) {
                    *_inlast++ = ch;
                    ++_incount;
                }

            }
            break;

        case UA5_EIR_RX_FIFO_TIMEOUT_INT_PENDING:
            // shouldn't happen, but just in case....
            ch = __inb( UA4_RXD );
            __cio_printf( "** SIO FIFO timeout, RXD = %02x\n", ch );
            break;

        case UA4_EIR_TX_INT_PENDING:
#if TRACING_SIO_ISR
    __cio_puts( " TX" );
#endif
            // if there is another character, send it
            if( _sending && _outcount > 0 ) {
#if TRACING_SIO_ISR
    __cio_printf( " ch %02x", *_outnext );
#endif
                __outb( UA4_TXD, *_outnext );
                ++_outnext;
                // wrap around if necessary
                if( _outnext >= (_outbuffer + BUF_SIZE) ) {
                    _outnext = _outbuffer;
                }
                --_outcount;
#if TRACING_SIO_ISR
    __cio_printf( " (outcount %d)", _outcount );
#endif
            } else {
#if TRACING_SIO_ISR
    __cio_puts( " EOS" );
#endif
                // no more data - reset the output vars
                _outcount = 0;
                _outlast = _outnext = _outbuffer;
                _sending = 0;
                // disable TX interrupts
                _sio_disable( SIO_TX );
            }
            break;

        case UA4_EIR_NO_INT:
#if TRACING_SIO_ISR
    __cio_puts( " EOI\n" );
#endif
            // nothing to do - tell the PIC we're done
            __outb( PIC_PRI_CMD_PORT, PIC_EOI );
            return;

        case UA4_EIR_MODEM_STATUS_INT_PENDING:
            // shouldn't happen, but just in case....
            msr = __inb( UA4_MSR );
            __cio_printf( "** SIO modem status, MSR = %02x\n", msr );
            break;

        default:
            // uh-oh....
            __sprint( b256, "sio isr: eir %02x\n", ((uint32_t) eir) & 0xff );
            PANIC( 0, b256 );
        }
    
    }

    // should never reach this point!
    assert( false );
}

/*
** PUBLIC FUNCTIONS
*/

/**
** _sio_init()
**
** Initialize the UART chip.
*/
void _sio_init( void ) {

    __cio_puts( " SIO:" );

    /*
    ** Initialize SIO variables.
    */

    __memclr( (void *) _inbuffer, sizeof(_inbuffer) );
    _inlast = _innext = _inbuffer;
    _incount = 0;

    __memclr( (void *) _outbuffer, sizeof(_outbuffer) );
    _outlast = _outnext = _outbuffer;
    _outcount = 0;
    _sending = 0;

    // queue of read-blocked processes
    QCREATE( _reading );

    /*
    ** Next, initialize the UART.
    **
    ** Initialize the FIFOs
    **
    ** this is a bizarre little sequence of operations
    */

    __outb( UA4_FCR, 0x20 );
    __outb( UA4_FCR, UA5_FCR_FIFO_RESET );    // 0x00
    __outb( UA4_FCR, UA5_FCR_FIFO_EN );       // 0x01
    __outb( UA4_FCR, UA5_FCR_FIFO_EN |
             UA5_FCR_RXSR );                  // 0x03
    __outb( UA4_FCR, UA5_FCR_FIFO_EN |
             UA5_FCR_RXSR |
             UA5_FCR_TXSR );                  // 0x07

    /*
    ** disable interrupts
    **
    ** note that we leave them disabled; _sio_enable() must be
    ** called to switch them back on
    */

    __outb( UA4_IER, 0 );
    _ier = 0;

    /*
    ** select bank 1 and set the data rate
    */

    __outb( UA4_LCR, UA4_LCR_BANK1 );
    __outb( UA4_LBGD_L, BAUD_LOW_BYTE( BAUD_9600 ) );
    __outb( UA4_LBGD_H, BAUD_HIGH_BYTE( BAUD_9600 ) );

    /*
    ** Select bank 0, and at the same time set the LCR for our
    ** data characteristics.
    */

    __outb( UA4_LCR, UA4_LCR_BANK0 |
             UA4_LCR_BITS_8 |
             UA4_LCR_1_STOP_BIT |
             UA4_LCR_NO_PARITY );
    
    /*
    ** Set the ISEN bit to enable the interrupt request signal,
    ** and the DTR and RTS bits to enable two-way communication.
    */

    __outb( UA4_MCR, UA4_MCR_ISEN | UA4_MCR_DTR | UA4_MCR_RTS );

    /*
    ** Install our ISR
    */

    __install_isr( INT_VEC_SERIAL_PORT_1, _sio_isr );

    /*
    ** Report that we're all set
    */

    __cio_puts( " done" );

}

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
uint8_t _sio_enable( uint8_t which ) {
    uint8_t old;

    // remember the current status

    old = _ier;

    // figure out what to enable

    if( which & SIO_TX ) {
        _ier |= UA4_IER_TX_INT_ENABLE;
    }

    if( which & SIO_RX ) {
        _ier |= UA4_IER_RX_INT_ENABLE;
    }

    // if there was a change, make it

    if( old != _ier ) {
        __outb( UA4_IER, _ier );
    }

    // return the prior settings

    return( old );
}

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
uint8_t _sio_disable( uint8_t which ) {
    uint8_t old;

    // remember the current status

    old = _ier;

    // figure out what to disable

    if( which & SIO_TX ) {
        _ier &= ~UA4_IER_TX_INT_ENABLE;
    }

    if( which & SIO_RX ) {
        _ier &= ~UA4_IER_RX_INT_ENABLE;
    }

    // if there was a change, make it

    if( old != _ier ) {
        __outb( UA4_IER, _ier );
    }

    // return the prior settings

    return( old );
}

/**
** _sio_inq_length()
**
** Get the input queue length
**
** usage:    int num = _sio_inq_length()
**
** @return the count of characters still in the input queue
*/
int _sio_inq_length( void ) {
    return( _incount );
}

/**
** _sio_readc()
**
** Get the next input character
**
** usage:    int ch = _sio_readc()
**
** @return the next character, or -1 if no character is available
*/
int _sio_readc( void ) {
    int ch;

    // assume there is no character available
    ch = -1;

    // 
    // If there is a character, return it
    //

    if( _incount > 0 ) {

        // take it out of the input buffer
        ch = ((int)(*_innext++)) & 0xff;
        --_incount;

        // reset the buffer variables if this was the last one
        if( _incount < 1 ) {
            _inlast = _innext = _inbuffer;
        }

    }

    return( ch );

}

/**
** _sio_reads(buf,length)
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

int _sio_reads( char *buf, int length ) {
    char *ptr = buf;
    int copied = 0;

    // if there are no characters, just return 0

    if( _incount < 1 ) {
        return( 0 );
    }

    //
    // We have characters.  Copy as many of them into the user
    // buffer as will fit.
    //

    while( _incount > 0 && copied < length ) {
        *ptr++ = *_innext++ & 0xff;
        if( _innext > (_inbuffer + BUF_SIZE) ) {
            _innext = _inbuffer;
        }
        --_incount;
        ++copied;
    }

    // reset the input buffer if necessary

    if( _incount < 1 ) {
        _inlast = _innext = _inbuffer;
    }

    // return the copy count

    return( copied );
}


/**
** _sio_writec( ch )
**
** Write a character to the serial output
**
** usage:    _sio_writec( int ch )
**
** @param ch   Character to be written (in the low-order 8 bits)
*/
void _sio_writec( int ch ){


    //
    // Must do LF -> CRLF mapping
    //

    if( ch == '\n' ) {
        _sio_writec( '\r' );
    }

    //
    // If we're currently transmitting, just add this to the buffer
    //

    if( _sending ) {
        *_outlast++ = ch;
        ++_outcount;
        return;
    }

    //
    // Not sending - must prime the pump
    //

    _sending = 1;
    __outb( UA4_TXD, ch );

    // Also must enable transmitter interrupts

    _sio_enable( SIO_TX );

}

/**
** _sio_write( buffer, length )
**
** Write a buffer of characters to the serial output
**
** usage:    int num = _sio_write( const char *buffer, int length )
**
** @param buffer   Buffer containing characters to write
** @param length   Number of characters to write
**
** @return the number of characters copied into the SIO output buffer
*/
int _sio_write( const char *buffer, int length ) {
    int first = *buffer;
    const char *ptr = buffer;
    int copied = 0;

    //
    // If we are currently sending, we want to append all
    // the characters to the output buffer; else, we want
    // to append all but the first character, and then use
    // _sio_writec() to send the first one out.
    //

    if( !_sending ) {
        ptr += 1;
        copied++;
    }

    while( copied < length && _outcount < BUF_SIZE ) {
        *_outlast++ = *ptr++;
        // wrap around if necessary
        if( _outlast >= (_outbuffer + BUF_SIZE) ) {
            _outlast = _outbuffer;
        }
        ++_outcount;
        ++copied;
    }

    //
    // We use _sio_writec() to send out the first character,
    // as it will correctly set all the other necessary
    // variables for us.
    //

    if( !_sending ) {
        _sio_writec( first );
    }

    // Return the transfer count


    return( copied );

}

/**
** _sio_puts( buf )
**
** Write a NUL-terminated buffer of characters to the serial output
**
** usage:    int num = _sio_puts( const char *buffer )
**
** @param buffer  The buffer containing a NUL-terminated string
**
** @return the count of bytes transferred
*/
int _sio_puts( const char *buffer ) {
    int n;  // must be outside the loop so we can return it

    for( n = 0; *buffer; ++n ) {
        _sio_writec( *buffer++ );
    }

    return( n );
}

/**
** _sio_Dump( full )
**
** dump the contents of the SIO buffers to the console
**
** usage:    _sio_dump(true) or _sio_dump(false)
**
** @param full   Boolean indicating whether or not a "full" dump
**               is being requested (which includes the contents
**               of the queues)
*/

void _sio_dump( bool_t full ) {
    int n;
    char *ptr;

    // dump basic info into the status region

    __cio_printf_at( 48, 0,
        "SIO: IER %02x (%c%c%c) in %d ot %d",
            ((uint32_t)_ier) & 0xff, _sending ? '*' : '.',
            (_ier & UA4_IER_TX_INT_ENABLE) ? 'T' : 't',
            (_ier & UA4_IER_RX_INT_ENABLE) ? 'R' : 'r',
            _incount, _outcount );

    // if we're not doing a full dump, stop now

    if( !full ) {
        return;
    }

    // also want the queue contents, but we'll
    // dump them into the scrolling region

    if( _incount ) {
        __cio_puts( "SIO input queue: \"" );
        ptr = _innext; 
        for( n = 0; n < _incount; ++n ) {
            __put_char_or_code( *ptr++ );
        }
        __cio_puts( "\"\n" );
    }

    if( _outcount ) {
        __cio_puts( "SIO output queue: \"" );
        __cio_puts( " ot: \"" );
        ptr = _outnext; 
        for( n = 0; n < _outcount; ++n )  {
            __put_char_or_code( *ptr++ );
        }
        __cio_puts( "\"\n" );
    }
}
