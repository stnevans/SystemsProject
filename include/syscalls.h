/**
** @file syscalls.h
**
** @author CSCI-452 class of 20215
**
** System call declarations
*/

#ifndef SYSCALLS_H_
#define SYSCALLS_H_

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/

#include "common.h"

// system call codes
//
// these are used in the user-level C library stub functions
#define SYS_exit        0
#define SYS_fork        1
#define SYS_execp       2
#define SYS_kill        3
#define SYS_wait        4
#define SYS_sleep       5
#define SYS_read        6
#define SYS_write       7
#define SYS_sysstat     8
#define SYS_getpid      9
#define SYS_getppid     10
#define SYS_gettime     11
#define SYS_getprio     12

// UPDATE THIS DEFINITION IF MORE SYSCALLS ARE ADDED!
#define N_SYSCALLS      13

// dummy system call code for testing our ISR
#define SYS_bogus       0xbad

// interrupt vector entry for system calls
#define INT_VEC_SYSCALL   0x80

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

/*
** Prototypes
*/

/**
** Name:  _sys_init
**
** Syscall module initialization routine
*/
void _sys_init( void );

/**
** _perform_exit - do the real work for exit() and some kill() calls
**
** @param victim  Pointer to the PCB for the exiting process
*/
void _perform_exit( pcb_t *victim );

#endif
/* SP_ASM_SRC */

#endif
