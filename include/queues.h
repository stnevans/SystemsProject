/**
** @file queues.h
**
** @author CSCI-452 class of 20215
**
** Queue module declarations
*/

#ifndef QUEUES_H_
#define QUEUES_H_

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/

// initial number of queues to create
#define N_QUEUES    5

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
** Our queue structure.  The queue is an opaque type, so the outside
** world only sees it as a "thing".  The basic queue_t type is a
** pointer to the internal queue structure.
*/

typedef struct q_s *queue_t;

// Key type (for ordering queues)
typedef uint32_t key_t;

/*
** Globals
*/

/*
** Prototypes
*/

/**
** _queue_create() - allocate a queue
**
** Allocates a queue structure and returns it to the caller.
**
** @param order   The ordering function to be used, or NULL
**
** @return a pointer to the allocated queue, or NULL
*/
queue_t _queue_create( int (*order)(const key_t, const key_t) );

/**
** _queue_delete() - return a queue to the free list
**
** Deallocates the supplied queue
**
** @param q   The queue to be put on the free list
*/
void _queue_delete( queue_t q );

/**
** _queue_length() - return the count of elements in a queue
**
** @param q   The queue to be checked
*/
uint_t _queue_length( queue_t q );

/**
** _queue_add() - add an element to a queue
**
** @param q     The queue to be manipulated
** @param data  The data to be added
** @param key   The key value to be used when ordering the queue
**
** @return the status of the insertion attempt
*/
status_t _queue_add( queue_t q, void *data, key_t key );

/**
** _queue_remove() - remove an element from a queue
**
** @param q     The queue to be manipulated
** @param data  (output) The data removed from the queue
**
** @return the status of the removal attempt
*/
status_t _queue_remove( queue_t q, void **data );

/**
** _queue_remove_specific() - remove a specific entry from a queue
**
** @param q     The queue to be manipulated
** @param data  The value to be removed from the queue
**
** @return the removed data value, or NULL
*/
void *_queue_remove_specific( queue_t q, void *data );

/**
** _queue_peek() - peek at the first element in a queue
**
** @param q   The queue to be checked
**
** @return the data pointer from the first node in the queue, or NULL
**         if the queue is empty (note: an empty queue is not
**         distinguishable from one whose first entry has a data
**         value of 0 using this function!).
*/
void *_queue_peek( queue_t q );

/**
** _queue_kpeek() - peek at the key from the first element in a queue
**
** @param q   The queue to be checked
**
** @return The key from the first node in the queue, or 0
**         if the queue is empty (note: an empty queue is
**         not distinguishable from one whose first entry
**         has a key value of 0 using this function!).
*/
key_t _queue_kpeek( queue_t q );

/*
** Debugging/tracing routines
*/

/**
** _queue_dump(msg,que)
**
** dump the contents of the specified queue to the console
**
** @param msg  Optional message to print
** @param q    Queue to dump
*/
void _queue_dump( const char *msg, queue_t q );

/*
** Module initialization
*/

/**
** _queue_init() - initialize the queue module
**
** Allocates the initial set of qnodes and set of queues.
**
** Dependencies:
**    Cannot be called before kmem is initialized
**    Must be called before any queue manipulation can be done
*/
void _queue_init( void );

#endif
/* SP_ASM_SRC */

#endif
