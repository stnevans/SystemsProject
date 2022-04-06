/**
** @file ulibs.S
**
** @author Numerous CSCI-452 classes
**
** assembly-language user-level library functions
*/

#define	SP_ASM_SRC

// get the system call codes

#include "syscalls.h"

/**
** System call stubs
**
** All have the same structure:
**
**      move a code into EAX
**      generate the interrupt
**      return to the caller
**
** As these are simple "leaf" routines, we don't use
** the standard enter/leave method to set up a stack
** frame - that takes time, and we don't really need it.
*/

#define	SYSCALL(name) \
	.globl	name			; \
name:					; \
	movl	$SYS_##name, %eax	; \
	int	$INT_VEC_SYSCALL	; \
	ret

/*
** "real" system calls
*/

SYSCALL(exit)
SYSCALL(fork)
SYSCALL(execp)
SYSCALL(kill)
SYSCALL(wait)
SYSCALL(sleep)
SYSCALL(read)
SYSCALL(write)
SYSCALL(sysstat)
SYSCALL(getpid)
SYSCALL(getppid)
SYSCALL(gettime)
SYSCALL(getprio)

/*
** This is a bogus system call; it's here so that we can test
** our handling of out-of-range syscall codes in the syscall ISR.
*/
SYSCALL(bogus)

/*
** Other library functions
*/

/**
** exit_helper() - dummy "startup" function
**
** calls exit(%eax) - serves as the "return to" code for main()
** functions, in case they don't call exit() themselves
*/

        .globl  exit_helper
exit_helper:
        pushl   %eax    // use whatever was in EAX as the status
        call    exit    // terminate this process
