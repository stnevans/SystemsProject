/*
** @file scheduler.c
**
** @author CSCI-452 class of 20215
**
** Scheduler implementation
*/

#define    SP_KERNEL_SRC

#include "common.h"
#include "syscalls.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

/*
** PUBLIC GLOBAL VARIABLES
*/

// the ready queue:  a MLQ with one level per priority value
queue_t _ready[N_PRIOS];

// the current user process
pcb_t *_current;

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
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
void _sched_init( void ) {

    __cio_puts( " Sched:" );
    
    // allocate the ready queues
    for( int i = 0; i < N_PRIOS; ++i ) {
        _ready[i] = _queue_create( NULL );
        // at this point, allocation failure is terminal
        assert( _ready[i] != NULL );
    }
    
    // reset the "current process" pointer
    _current = NULL;
    
    __cio_puts( " done" );
}

/**
** _schedule() - add a process to the ready queue
**
** Enques the supplied process according to its priority value
**
** @param pcb   The process to be scheduled
*/
void _schedule( pcb_t *pcb ) {

    // can't enque nothing
    assert1( pcb != NULL );

    // if this process has been terminated, clean it up
    if( pcb->state == Killed ) {
        _perform_exit( pcb );
        return;
    }

    // bad priority value causes a fault
    assert1( pcb->priority < N_PRIOS );
    
    // mark the process as ready to execute
    pcb->state = Ready;

    // add it to the appropriate queue
    int status = _queue_add( _ready[pcb->priority], pcb, 0 );

    // failure is not an option!
    assert( status == E_SUCCESS );
}

/**
** _dispatch() - select a new "current" process
**
** Selects the highest-priority process available
*/
void _dispatch( void ) {
    pcb_t *pcb;
    int n;
    
    do {

        // find a ready queue that has an available process
        for( n = 0; n < N_PRIOS; ++n ) {
            if( _queue_length(_ready[n]) > 0 ) {
                break;
            }
        }

        // this should never happen - if nothing else, the
        // idle process should be on the "Deferred" queue
        assert( n < N_PRIOS );

        // OK, we found a queue; pull the first process from it
        status_t status = _queue_remove( _ready[n], (void **) &pcb );

        // failure to deque means something serious has gone wrong
        assert( status == E_SUCCESS );

        // if this process has been terminated, clean it up, then
        // loop and pick another process; otherwise, leave the loop
        if( pcb->state == Killed ) {
            _perform_exit( pcb );
        } else {
            // we have a winner!
            break;
        }

    } while( 1 );

    // set its state and remaining quantum
    pcb->state = Running;
    pcb->ticks = pcb->quantum;

    // make this the current process
    _current = pcb;
}
