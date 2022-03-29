/**
** @file clock.h
**
** @author CSCI-452 class of 20215
**
** Clock module declarations
*/

#ifndef CLOCK_H_
#define CLOCK_H_

#include "common.h"
#include "queues.h"

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/

// tick rate of our clock
#define CLOCK_FREQUENCY     1000
#define TICKS_PER_MS        1

// conversion functions for seconds, ms, and ticks
// SEC_TO_MS is defined in common.h
#define MS_TO_TICKS(n)          ((n))
#define SEC_TO_TICKS(n)         (MS_TO_TICKS(SEC_TO_MS(n)))
#define TICKS_TO_SEC(n)         ((n) / CLOCK_FREQUENCY)
#define TICKS_TO_SEC_ROUNDED(n) (((n)+(CLOCK_FREQUENCY-1)) / CLOCK_FREQUENCY)

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
**
** Anything that should not be visible to something other than
** the C compiler should be put here.
*/

/*
** Types
*/

/*
** Globals
*/

// current system time
extern time_t _system_time;

// we own the sleep queue
extern queue_t _sleeping;

/*
** Prototypes
*/

/**
** Name:  _clk_init
**
** Initializes the clock module
**
*/
void _clk_init( void );

#endif
/* SP_ASM_SRC */

#endif
