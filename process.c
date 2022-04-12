/**
** @file process.c
**
** @author  CSCI-452 class of 20215
**
** Process module implementation
*/

#define SP_KERNEL_SRC

#include "common.h"

#include <x86arch.h>
#include "bootstrap.h"

#include "process.h"
#include "scheduler.h"
#include "stacks.h"
#include "cio.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// PCB management
static pcb_t *_pcb_list;

/*
** PUBLIC GLOBAL VARIABLES
*/

// next available PID
pid_t _next_pid;

// active process count
uint32_t _n_procs;

// table of active processes
pcb_t *_processes[N_PROCS];

/*
** PRIVATE FUNCTIONS
*/

/**
** _pcb_add() - allocate a slice and carve it into PCBs
**
** @param critical  Should we panic on allocation failure?
**
** @return the number of PCBs added to the pool
*/
static int _pcb_add( void ) {

    // start by carving off a slice of memory
    pcb_t *new = (pcb_t *) _km_slice_alloc();

    // NULL slice is a problem
    if( new == NULL ) {
        return( false );
    }

    // clear out the allocated space
    __memclr( new, SZ_SLICE );

    for( int i = 0; i < (SZ_SLICE / sizeof(pcb_t)); ++i ) {
        _pcb_free( &new[i] );
    }

#if TRACING_PCB
    __cio_printf( "** _pcb_add() added %d PCBs\n", SZ_SLICE / sizeof(pcb_t) );
#endif

    // all done!
    return( SZ_SLICE / sizeof(pcb_t) );
}

/*
** PUBLIC FUNCTIONS
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
void _pcb_init( void ) {

    __cio_puts( " Process:" );

    // allocate an initial slice of PCBs
    _pcb_list = NULL;
    assert( _pcb_add() );   // returns 0 on failure

    // reset the "active" variables
    _n_procs = 0;
    for( int i = 0; i < N_PROCS; ++i ) {
        _processes[i] = NULL;
    }

    // first process is init, PID 1; it's created by system initialization

    // second process is idle, PID 2; it's spawned by init()
    _next_pid = 2;

    // all done!
    __cio_puts( " done" );
}

/*
** PCB manipulation
*/

/**
** _pcb_alloc() - allocate a PCB
**
** Allocates a PCB structure and returns it to the caller.
**
** @return a pointer to the allocated PCB, or NULL
*/
pcb_t *_pcb_alloc( void ) {
    pcb_t *new;

    // see if there is an available PCB
    if( _pcb_list == NULL ) {

        // no - see if we can create some
        if( !_pcb_add() ) {
            // no!  let's just leave quietly
            return( NULL );
        }
    }

    // OK, we know that there is at least one free PCB;
    // just take the first one from the list

    new = _pcb_list;
    _pcb_list = (pcb_t *) new->context;

    // clear out the fields in this one just to be safe
    __memclr( new, sizeof(pcb_t) );

    // pass it back to the caller
    return( new );
}

/**
** _pcb_free() - return a PCB to the free list
**
** Deallocates the supplied PCB
**
** @param pcb   The PCB to be put on the free list
*/
void _pcb_free( pcb_t *pcb ) {

    // sanity check!
    if( pcb == NULL ) {
        return;
    }

    // mark it as unused and unknown (just in case)
    pcb->state = Free;
    pcb->pid = pcb->ppid = 0;

    // stick it at the front of the list
    pcb->context = (context_t *) _pcb_list;
    _pcb_list = pcb;
}

/**
** _pcb_cleanup(pcb) - reclaim a process' data structures
**
** @param pcb   The PCB to reclaim
*/
void _pcb_cleanup( pcb_t *pcb ) {

    // avoid deallocating a NULL pointer
    if( pcb == NULL ) {
        // should this be an error?
        return;
    }

    // clear the entry in the process table
    for( int i = 0; i < N_PROCS; ++i ) {
        if( _processes[i] == pcb ) {
            _processes[i] = NULL;
            --_n_procs;
            break;
        }
    }

    // release the stack(en?)
    if( pcb->stack != NULL ) {
        _stk_free( pcb->stack );
    }

    // release the PCB
    pcb->state = Free;  // just to be sure!
    // if(pcb->pg_dir){
    //     set_page_directory(get_kernel_pg_dir());
    //     delete_pg_dir(pcb->pg_dir);
    // }
    _pcb_free( pcb );
}

/*
** Debugging/tracing routines
*/

/**
** _pcb_dump(msg,pcb)
**
** Dumps the contents of this PCB to the console
**
** @param msg   An optional message to print before the dump
** @param p     The PCB to dump out
*/
void _pcb_dump( const char *msg, register pcb_t *p ) {

    // first, the message (if there is one)
    if( msg ) {
        __cio_printf( "%s ", msg );
    }

    // the pointer
    __cio_printf( "@ %08x: ", (uint32_t) p );

    // if it's NULL, why did you bother calling me?
    if( p == NULL ) {
        __cio_puts( " NULL???\n" );
        return;
    }

    // now, the contents
    __cio_printf( " pids %d/%d state %d prio %d",
                  p->pid, p->ppid, p->state, p->priority );

    __cio_printf( "\n ticks %d/%d xit %d wake %08x",
                  p->ticks, p->quantum, p->exit_status, p->wakeup );

    __cio_printf( "\n context %08x stack %08x",
                  (uint32_t) p->context, (uint32_t) p->stack );
    __cio_printf( "\n pg_dir %08x",
                  (uint32_t) p->pg_dir );

    // and the filler (just to be sure)
    __cio_puts( " fill: " );
    // for( int i = 0; i < sizeof(p->pg_dir); ++i ) {
    //     __cio_printf( "%02x", p->filler[i] );
    // }

    __cio_putchar( '\n' );
}

/**
** _context_dump(msg,context)
**
** Dumps the contents of this process context to the console
**
** @param msg   An optional message to print before the dump
** @param c     The context to dump out
*/
void _context_dump( const char *msg, register context_t *c ) {

    // first, the message (if there is one)
    if( msg ) {
        __cio_printf( "%s ", msg );
    }

    // the pointer
    __cio_printf( "@ %08x: ", (uint32_t) c );

    // if it's NULL, why did you bother calling me?
    if( c == NULL ) {
        __cio_puts( " NULL???\n" );
        return;
    }

    // now, the contents
    __cio_printf( "ss %04x gs %04x fs %04x es %04x ds %04x cs %04x\n",
                  c->ss & 0xff, c->gs & 0xff, c->fs & 0xff,
                  c->es & 0xff, c->ds & 0xff, c->cs & 0xff );
    __cio_printf( "  edi %08x esi %08x ebp %08x esp %08x\n",
                             c->edi, c->esi, c->ebp, c->esp );
    __cio_printf( "  ebx %08x edx %08x ecx %08x eax %08x\n",
                  c->ebx, c->edx, c->ecx, c->eax );
    __cio_printf( "  vec %08x cod %08x eip %08x eflags %08x\n",
                  c->vector, c->code, c->eip, c->eflags );
}

/**
** _context_dump_all(msg)
**
** dump the process context for all active processes
**
** @param msg  Optional message to print
*/
void _context_dump_all( const char *msg ) {

    if( msg != NULL ) {
        __cio_printf( "%s: ", msg );
    }

    __cio_printf( "%d active processes\n", _n_procs );

    if( _n_procs < 1 ) {
        return;
    }

    int n = 0;
    for( int i = 0; i < N_PROCS; ++i ) {
        pcb_t *pcb = _processes[i];
        if( pcb != NULL && pcb->state != Free ) {
            ++n;
            __cio_printf( "%2d[%2d]: ", n, i );
            _context_dump( NULL, pcb->context );
        }
    }
}

/**
** _ptable_dump(msg,all)
**
** dump the contents of the "active processes" table
**
** @param msg  Optional message to print
** @param all  Dump all or only part of the relevant data
*/
void _ptable_dump( const char *msg, bool_t all ) {

    if( msg ) {
        __cio_printf( "%s: ", msg );
    }

    int used = 0;
    int empty = 0;

    for( int i = 0; i < N_PROCS; ++i ) {
        register pcb_t *pcb = _processes[i];
        if( pcb == NULL ) {

            // an empty slot
            ++empty;

        } else {

            // a non-empty slot
            ++used;

            // if not dumping everything, add commas if needed
            if( !all && used ) {
                __cio_putchar( ',' );
            }

            // things that are always printed
            __cio_printf( " #%d: %d/%d", i, pcb->pid, pcb->ppid );
            if( pcb->state < Free || pcb->state >= N_STATES ) {
                __cio_printf( " UNKNOWN" );
            } else {
                __cio_printf( " %s", _statestr[pcb->state] );
            }
            // do we want more info?
            if( all ) {
                __cio_printf( " stk %08x ESP %08x EIP %08x\n",
                      (uint32_t) pcb->stack, pcb->context->esp,
                      pcb->context->eip );
            }
        }
    }
    // only need this if we're doing one-line output
    if( !all ) {
        __cio_putchar( '\n' );
    }

    // sanity check - make sure we saw the correct number of table slots
    if( (used + empty) != N_PROCS ) {
        __cio_printf( "Table size %d, used %d + empty %d = %d???\n",
                      N_PROCS, used, empty, used + empty );
    }
}
