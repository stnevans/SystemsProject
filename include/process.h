/**
** @file process.h
**
** @author CSCI-452 class of 20215
**
** Process module declarations
*/

#ifndef PROCESS_H_
#define PROCESS_H_

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
**
** Anything that should not be visible to something other than
** the C compiler should be put here.
*/

#include "common.h"

// REG(pcb,x) -- access a specific register in a process context

#define REG(pcb,x)  ((pcb)->context->x)

// RET(pcb) -- access return value register in a process context

#define RET(pcb)    ((pcb)->context->eax)

// ARG(pcb,n) -- access argument #n from the indicated process
//
// ARG(pcb,0) --> return address
// ARG(pcb,1) --> first parameter
// ARG(pcb,2) --> second parameter
// etc.
//
// ASSUMES THE STANDARD 32-BIT ABI, WITH PARAMETERS PUSHED ONTO THE
// STACK.  IF THE PARAMETER PASSING MECHANISM CHANGES, SO MUST THIS!

#define ARG(pcb,n)  ( ( (uint32_t *) (((pcb)->context) + 1) ) [(n)] )

/*
** Types
*/

// process states and priorities are defined in common.h,
// as they must be visible to user-level code

/*
** Process context structure
**
** NOTE:  the order of data members here depends on the
** register save code in isr_stubs.S!!!!
**
** This will be at the top of the user stack when we enter
** an ISR.  In the case of a system call, it will be followed
** by the return address and the system call parameters.
*/

typedef struct context {
    uint32_t ss;        // pushed by isr_save
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t vector;
    uint32_t code;
    uint32_t eip;       // pushed by the hardware
    uint32_t cs;
    uint32_t eflags;
} context_t;

// PCB needs to know what stacks look like, but the stacks.h header
// needs to know what a context_t looks like.  Bleh.
#include "stacks.h"

// the process control block
//
// fields are ordered by size to avoid padding
//
// ideally, its size should divide evenly into 1024 bytes;
// currently, 32 bytes

typedef struct pcb_s {
    // four-byte values

    // Start with these eight bytes, for easy access in assembly
    context_t *context;     // pointer to context save area on stack
    stack_t *stack;         // pointer to process stack

    time_t wakeup;          // wakeup time for this process when sleeping
    int exit_status;        // termination status, for parent's use

    pid_t pid;              // unique PID for this process
    pid_t ppid;             // PID of the parent

    // one-byte values
    state_t state;          // current state (see common.h)
    prio_t priority;        // process priority (MLQ queue level)

    uint8_t quantum;        // quantum for this process
    uint8_t ticks;          // ticks remaining in current slice

    // filler, to round us up to 32 bytes
    // adjust this as fields are added/removed/changed
    uint8_t filler[4];

} pcb_t;

/*
** Globals
*/

// next available PID
extern pid_t _next_pid;

// active process count
extern uint32_t _n_procs;

// table of active processes
extern pcb_t *_processes[N_PROCS];

/*
** Prototypes
*/

/*
** Module initialization
*/

/**
** _pcb_init() - initialize the process module
**
** Allocates an initial set of PCBs, does whatever else is
** needed to make it possible to create processes
**
** Dependencies:
**    Cannot be called before kmem is initialized
**    Must be called before any process creation can be done
*/
void _pcb_init( void );

/*
** PCB manipulation
*/

/**
** _pcb_alloc() - allocate a PCB
**
** @return pointer to a "clean" PCB, or NULL
*/
pcb_t *_pcb_alloc( void );

/**
** _pcb_free() - free a PCB
**
** @param pcb   The PCB to be returned to the free list
*/
void _pcb_free( pcb_t *pcb );

/*
** Debugging/tracing routines
*/

/**
** _pcb_cleanup(pcb) - reclaim a process' data structures
**
** Reclaim a process' data structures
**
** @param p   The PCB to reclaim
*/
void _pcb_cleanup( pcb_t *p );

/**
** _pcb_dump(msg,pcb)
**
** Dumps the contents of this PCB to the console
**
** @param msg   An optional message to print before the dump
** @param p     The PCB to dump out
*/
void _pcb_dump( const char *msg, register pcb_t *p );

/**
** _context_dump(msg,context)
**
** Dumps the contents of this process context to the console
**
** @param msg   An optional message to print before the dump
** @param c     The context to dump out
*/
void _context_dump( const char *msg, register context_t *c );

/**
** _context_dump_all(msg)
**
** dump the process context for all active processes
**
** @param msg  Optional message to print
*/
void _context_dump_all( const char *msg );

/**
** _ptable_dump(msg,all)
**
** dump the contents of the "active processes" table
**
** @param msg  Optional message to print
** @param all  Dump all or only part of the relevant data
*/
void _ptable_dump( const char *msg, bool_t all );

#endif
/* SP_ASM_SRC */

#endif
