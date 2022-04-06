/**
** @file kdefs.h
**
** @author Numerous CSCI-452 classes
**
** Kernel-only definitions for the baseline system.
**
*/

#ifndef KDEFS_H_
#define KDEFS_H_

// The OS needs the standard system headers

#include "cio.h"
#include "kmem.h"
#include "support.h"
#include "kernel.h"

// The OS also needs the kernel library.

#include "lib.h"

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
*/

// bit patterns for modulus checking of (e.g.) sizes and addresses

#define MOD4_BITS        0x3
#define MOD4_MASK        0xfffffffc

#define MOD16_BITS       0xf
#define MOD16_MASK       0xfffffff0

/*
** Debugging and sanity-checking macros
*/

// Warning messages to the console

#define WARNING(m)  do { \
        __cio_printf( "WARN %s (%s @ %s): ", __func__, __FILE__, __LINE__ ); \
        __cio_puts( m ); \
        __cio_putchar( '\n' ); \
    } while(0)

// Panic messages to the console
// n: severity level
// m: message (condition, etc.)

#define PANIC(n,m)  do { \
        __sprint( b512, "ASSERT %s (%s @ %s), %d: %s\n", \
                  __func__, __FILE__, __LINE__, n, m ); \
        _kpanic( b512 ); \
    } while(0)

// Always-active assertions

#define assert(x)   if( !(x) ) { PANIC(0,x); }

/*
** Conditional assertions are categorized by the "sanity level"
** being used in this compilation; each only triggers a fault
** if the sanity level is at or above a specific value.  This
** allows selective enabling/disabling of debugging checks.
**
** The sanity level is set during compilation with the CPP macro
** "SANITY".  A sanity level of 0 disables these assertions.
*/

#ifndef SANITY
// default sanity check level: check everything!
#define SANITY  9999
#endif

// only provide these macros if the sanity check level is positive

#if SANITY > 0

#define assert1(x)  if( SANITY >= 1 && !(x) ) { PANIC(1,x); }
#define assert2(x)  if( SANITY >= 2 && !(x) ) { PANIC(2,x); }
#define assert3(x)  if( SANITY >= 3 && !(x) ) { PANIC(3,x); }
#define assert4(x)  if( SANITY >= 4 && !(x) ) { PANIC(4,x); }

#else

#define assert1(x)  // do nothing
#define assert2(x)  // do nothing
#define assert3(x)  // do nothing
#define assert4(x)  // do nothing

#endif

/*
** Tracing options are enabled by defining the TRACE macro in the
** Makefile.  The value of that macro is a bit mask.
*/

#ifdef TRACE

// bits for selecting desired trace
#define TRACE_PCB       0x01
#define TRACE_SYSRET    0x02
#define TRACE_EXIT      0x04
#define TRACE_STACK     0x08
#define TRACE_SIO_ISR   0x10
#define TRACE_SIO_WR    0x20
#define TRACE_SYSCALLS  0x40
#define TRACE_CONSOLE   0x80

// expressions for testing trace options
#define TRACING_PCB         ((TRACE & TRACE_PCB) != 0)
#define TRACING_SYSRET      ((TRACE & TRACE_SYSRET) != 0)
#define TRACING_EXIT        ((TRACE & TRACE_EXIT) != 0)
#define TRACING_STACK       ((TRACE & TRACE_STACK) != 0)
#define TRACING_SIO_ISR     ((TRACE & TRACE_SIO_ISR) != 0)
#define TRACING_SIO_WR      ((TRACE & TRACE_SIO_WR) != 0)
#define TRACING_SYSCALLS    ((TRACE & TRACE_SYSCALLS) != 0)
#define TRACING_CONSOLE     ((TRACE & TRACE_CONSOLE) != 0)

#else

#define TRACING_PCB         0
#define TRACING_SYSRET      0
#define TRACING_EXIT        0
#define TRACING_STACK       0
#define TRACING_SIO_ISR     0
#define TRACING_SIO_WR      0
#define TRACING_SYSCALLS    0
#define TRACING_CONSOLE     0

#endif

#endif
/* SP_ASM_SRC */

#endif
