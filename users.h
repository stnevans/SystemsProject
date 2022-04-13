/**
** @file users.h
**
** @author CSCI-452 class of 20215
**
** "Userland" configuration information
*/

#ifndef USERS_H_
#define USERS_H_

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/

// Maximum number of command-line arguments that can be passed
// to a process.  Not a true limit; used to dimension the argv[]
// arrays in the various user functions, for convenience.

#define MAX_ARGS        7

// delay loop counts

#define DELAY_LONG      100000000
#define DELAY_MED       4500000
#define DELAY_SHORT     2500000

#define DELAY_STD       DELAY_SHORT

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
*/

// convenience macros

// a delay loop

#define DELAY(n)    do { \
        for(int _dlc = 0; _dlc < (DELAY_##n); ++_dlc) continue; \
    } while(0)

// command-line argument macros
//
// each takes a command name and one or more command-line
// arguments (typically, either string literals or arrays of
// char), and fills in the 'args' array slots accordingly, with
// a NULL pointer at the end

#define	ARGS0(p,a)         char *argv_##p[] = { a, NULL };
#define	ARGS1(p,a,b)       char *argv_##p[] = { a, b, NULL };
#define	ARGS2(p,a,b,c)     char *argv_##p[] = { a, b, c, NULL };
#define	ARGS3(p,a,b,c,d)   char *argv_##p[] = { a, b, c, d, NULL };
#define	ARGS4(p,a,b,c,d,e) char *argv_##p[] = { a, b, c, d, e, NULL };

//
// System call matrix
//
// System calls in this system:   exit, fork, exec, kill, wait, sleep,
//  read, write, sysstat, getpid, getppid, gettime
//
// There is also a "bogus" system call which attempts to use an invalid
// system call code; this should be caught by the syscall handler and
// the process should be terminated.
//
// These are the system calls which are used in each of the user-level
// main functions.  Some main functions only invoke certain system calls
// when given particular command-line arguments (e.g., main6).
//
// Note that some system calls are nested inside library functions - e.g.,
// spawn() performs fork() and exec(), cwrite() performs write(), etc.
//
//                        baseline system calls in use
//  fcn   exit fork exec kill wait sleep read write stat pid ppid time bogus
// -----  ---- ---- ---- ---- ---- ----- ---- ----- ---- --- ---- ---- -----
// main1    X    .    .    .     .    .    .     X    .   .    .    .    .
// main2    .    .    .    .     .    .    .     X    .   .    .    .    .
// main3    X    .    .    .     .    X    .     X    .   .    .    .    .
// main4    X    X    X    .     .    X    .     X    .   X    .    .    .
// main5    X    X    X    .     .    .    .     X    .   .    .    .    .
// main6    X    X    X    X     X    X    .     X    .   X    .    .    .
//
// userH    X    X    X    .     .    X    .     X    .   .    .    .    .
// userI    X    X    X    X     .    X    .     X    .   X    .    .    .
// userJ    X    X    X    .     .    .    .     X    .   X    .    .    .
// userP    X    .    .    .     .    X    .     X    X   .    .    X    .
// userQ    X    .    .    .     .    .    .     X    .   .    .    .    X
// userR    X    .    .    .     .    X    X     X    .   .    .    .    .
// userS    X    .    .    .     .    X    .     X    .   .    .    .    .
// userV    X    .    .    .     .    X    .     X    .   .    .    .    .
// userW    X    .    .    .     .    X    .     X    .   X    .    X    .
// userX    X    .    .    .     .    .    .     X    .   .    .    .    .
// userY    X    .    .    .     .    X    .     X    .   .    .    .    .
// userZ    X    .    .    .     .    X    .     X    .   X    X    .    .

/*
** User process controls.
**
** To spawn a specific user process from the initial process, uncomment
** its entry in this list.
*/

//
// Generally, most of these will exit with a status of 0.  If a process
// returns from its main function when it shouldn't (e.g., if it had
// called exit() but continued to run), it will usually return a status
// of 42.
//
#define SPAWN_A
#define SPAWN_B
#define SPAWN_C
#define SPAWN_D
#define SPAWN_E
#define SPAWN_F
#define SPAWN_G
#define SPAWN_H
#define SPAWN_I
#define SPAWN_J
#define SPAWN_K
#define SPAWN_L
#define SPAWN_M
#define SPAWN_N
// no user O
#define SPAWN_P
#define SPAWN_Q
#define SPAWN_R
#define SPAWN_S
#define SPAWN_T
#define SPAWN_U
#define SPAWN_V

//
// Users W-Z are spawned from other processes; they
// should never be spawned directly by init().
//

/*
** Prototypes for externally-visible routines
*/

/**
** init - initial user process
**
** Spawns the other user processes, then loops forever calling wait()
**
** Invoked as:  init
*/
int32_t init( int argc, char *argv[] );

/**
** idle - the idle process
**
** Reports itself, then loops forever delaying and printing a character.
**
** Invoked as:  idle
*/
int32_t idle( int argc, char *argv[] );

#endif
/* SP_ASM_SRC */

#endif
