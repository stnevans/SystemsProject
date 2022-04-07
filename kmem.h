/**
** @file kmem.h
**
** @author Warren R. Carithers
** @author Kenneth Reek
** @author 4003-506 class of 20013
**
** Structures and functions to support dynamic memory
** allocation within the OS.
**
**      The list of free blocks is ordered by address to facilitate
**      combining freed blocks with adjacent blocks that are already
**      free.
**
**      All requests for memory are satisfied with blocks that are
**      an integral number of 4-byte words.  More memory may be
**      provided than was requested if the fragment left over after
**      the allocation would not be large enough to be useable.
*/

#ifndef KMEM_H_
#define KMEM_H_

#include "common.h"

#include "compat.h"

/*
** General (C and/or assembly) definitions
*/

// Slab and slice sizes, in bytes

#define SZ_SLICE    1024
#define SZ_SLAB     4096

// Page size, in bytes

#define SZ_PAGE     SZ_SLAB

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
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
** Name: _km_init
**
** Find what memory is present on the system and
** construct the list of free memory blocks.
**
** Dependencies:
**    Must be called before any other init routine that uses
**    dynamic storage is called.
*/
void _km_init( void );

/**
** Name:    _km_dump
**
** Dump the current contents of the freelist to the console
*/
void _km_dump( void );

/*
** Functions that manipulate free memory blocks.
*/

/**
** Name:    _km_page_alloc
**
** Allocate a page of memory from the free list.
**
** @param count  Number of contiguous pages desired
**
** @return a pointer to the beginning of the first allocated page,
**         or NULL if no memory is available
*/
void *_km_page_alloc( uint32_t count );

/**
** Name:    _km_page_free
**
** Returns a memory block to the list of available blocks,
** combining it with adjacent blocks if they're present.
**
** CRITICAL ASSUMPTION:  multi-page blocks will be freed one page
** at a time!
**
** @param block   Pointer to the page to be returned to the free list
*/
void _km_page_free( void *block );

/**
** Name:    _km_slice_alloc
**
** Dynamically allocates a slice (1/4 of a page).  If no
** memory is available, we panic.
**
** @return a pointer to the allocated slice
*/
void *_km_slice_alloc( void );

/**
** Name:    _km_slice_free
**
** Returns a slice to the list of available slices.
**
** We make no attempt to merge slices, as they are independent
** blocks of memory (unlike pages).
**
** @param block  Pointer to the slice (1/4 page) to be freed
*/
void _km_slice_free( void *block );

bool_t km_is_init(void);

#endif
/* SP_ASM_SRC */

#endif
