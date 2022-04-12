/*
** @file stacks.h
**
** @author CSCI-452 class of 20215
**
** Stack module declarations
*/

#ifndef STACKS_H_
#define STACKS_H_

/*
** General (C and/or assembly) definitions
*/

#include "common.h"

#include "kmem.h"
#include "process.h"
#include "paging.h"
#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
*/

#include "common.h"

// stack size, in bytes and words
//
// for simplicity, our stack is a multiple of the page size (4KB)

// four pages (16KB) per stack
#define STACK_PAGES      4

#define SZ_STACK        (SZ_PAGE * STACK_PAGES)
#define STACK_WORDS     (SZ_STACK / sizeof(uint32_t))

/*
** Types
*/

// the stack
//
// somewhat anticlimactic....

typedef uint32_t stack_t[STACK_WORDS];

/*
** Globals
*/

/*
** Prototypes
*/

/**
** _stk_init() - initialize the stack module
**
** Sets up the system stack (for use during interrupts)
**
** Dependencies:
**    Cannot be called before kmem is initialized
**    Must be called before interrupt handling has begun
**    Must be called before process creation has begun
*/
void _stk_init( void );

/**
** _stk_alloc() - allocate a stack
**
** @return pointer to the allocated stack, or NULL
*/
stack_t *_stk_alloc( struct page_directory * pg_dir );

/**
** _stk_free() - free a stack
**
** @param stk   The stack to be returned to the free list
*/
void _stk_free( stack_t *stk );

/**
** _stk_setup - set up the stack for a new process
**
** @param stk    - The stack to be set up
** @param entry  - Entry point for the new process
** @param args   - Argument vector to be put in place
**
** @return A pointer to the context_t on the stack, or NULL
*/
context_t *_stk_setup( stack_t *stk, uint32_t entry, char *args[] );

/*
** Debugging/tracing routines
*/

/**
** _stk_dump(msg,stk,lim)
**
** Dumps the contents of this stack to the console.  Assumes the stack
** is a multiple of four words in length.
**
** @param msg   An optional message to print before the dump
** @param stk   The stack to dump out
** @param lim   Limit on the number of words to dump (0 for all)
*/
void _stk_dump( const char *msg, register stack_t *stk, uint32_t lim );

#endif
/* SP_ASM_SRC */

#endif
