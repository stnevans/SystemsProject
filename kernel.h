/**
** @file kernel.h
**
** @author Numerous CSCI-452 classes
**
** Miscellaneous OS routines
*/

#ifndef KERNEL_H_
#define KERNEL_H_

#include "common.h"
#include "x86arch.h"

#include "process.h"
#include "queues.h"

/*
** General (C and/or assembly) definitions
*/

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
*/

// default contents of EFLAGS register for new processes
#define DEFAULT_EFLAGS  (EFLAGS_MB1 | EFLAGS_IF)

// PID for the init process
#define PID_INIT        1

/*
** Types
*/

/*
** Globals
*/

// character buffers, usable throughout the OS
// not guaranteed to retain their contents across an exception return
extern char b256[256];
extern char b512[512];

// Other system variables (see kernel.c for possible names)

// A separate stack for the OS itself
// (NOTE: this assumes the OS is not reentrant!)
extern stack_t *_system_stack;
extern uint32_t *_system_esp;

// PCB for the init process
extern pcb_t *_init_pcb;

// table of state name strings
extern const char *_statestr[];

// table of priority name strings
extern const char *_priostr[];

/*
** Prototypes
*/

/**
** _kinit - system initialization routine
**
** Called by the startup code immediately before returning into the
** first user process.
*/
void _kinit( void );

#ifdef CONSOLE_SHELL
/**
** _kshell - extremely simple shell for handling console input
**
** Called whenever we want to take input from the console and
** act upon it (e.g., for debugging the kernel)
**
** @param ch   The character that should be processed first
*/
void _kshell( int ch );

#endif

#endif
/* SP_ASM_SRC */

#endif
