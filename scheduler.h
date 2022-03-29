/*
** @file scheduler.h
**
** @author CSCI-452 class of 20215
**
** Scheduler module declarations
*/

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/

#include "common.h"

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
**
** Anything that should not be visible to something other than
** the C compiler should be put here.
*/

// standard process quantum
#define Q_DEFAULT       5

/*
** Types
*/

/*
** Globals
*/

// the ready queue:  a MLQ with one level per priority value
extern queue_t _ready[N_PRIOS];

// the current user process
extern pcb_t *_current;

/*
** Prototypes
*/

/**
** _sched_init() - initialize the scheduler module
**
** Allocates the ready queues and resets the "current process" pointer
**
** Dependencies:
**    Cannot be called before queues are initialized
**    Must be called before any process scheduling can be done
*/
void _sched_init( void );

/**
** _schedule() - add a process to the ready queue
**
** Enques the supplied process according to its priority value
**
** @param pcb   The process to be scheduled
*/
void _schedule( pcb_t *pcb );

/**
** _dispatch() - select a new "current" process
**
** Selects the highest-priority process available
*/
void _dispatch( void );

#endif
/* SP_ASM_SRC */

#endif
