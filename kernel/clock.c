/**
** @file clock.c
**
** @author CSCI-452 class of 20215
**
** Clock module  implementation
*/

#define SP_KERNEL_SRC

#include "x86arch.h"
#include "x86pic.h"
#include "x86pit.h"

#include "common.h"

#include "clock.h"
#include "process.h"
#include "queues.h"
#include "scheduler.h"
#include "sio.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// pinwheel control variables
static uint32_t _pinwheel;   // pinwheel counter
static uint32_t _pindex;     // index into pinwheel string

/*
** PUBLIC GLOBAL VARIABLES
*/

// current system time
time_t _system_time;

// we own the sleep queue
queue_t _sleeping;

/*
** PRIVATE FUNCTIONS
*/

/**
** Name:  _cmp_wakeup
**
** Ordering function for the sleep queue
**
** @param v1    First key value to examine
** @param v2    Second key value to examine
**
** @return Relationship between the key values:
**      < 0   v1 < v2
**      = 0   v1 == v2
**      > 0   v1 > v2
*/
static int _cmp_wakeup( const key_t v1, const key_t v2 ) {

    // QUESTION:  does it make more sense to put this function
    // somewhere else with a name like "_cmp_uint32", which would
    // make it more generic and thus usable for other ordered
    // queues which sort in ascending order by uint32_t values?

    if( v1 < v2 )
        return( -1 );
    else if( v1 == v2 )
        return( 0 );
    else
        return( 1 );
}

/**
** Name:  _clk_isr
**
** The ISR for the clock
**
** @param vector    Vector number for the clock interrupt
** @param code      Error code (0 for this interrupt)
*/
static void _clk_isr( int vector, int code ) {

    // spin the pinwheel

    ++_pinwheel;
    if( _pinwheel == (CLOCK_FREQUENCY / 10) ) {
        _pinwheel = 0;
        ++_pindex;
        __cio_putchar_at( 0, 0, "|/-\\"[ _pindex & 3 ] );
    }

#if defined(STATUS)
    // Periodically, dump the queue lengths and the SIO status (along
    // with the SIO buffers, if non-empty).
    //
    // Define the symbol STATUS with a value equal to the desired
    // reporting frequency, in seconds.

    uint32_t counts[ N_STATES ];

    if( (_system_time % SEC_TO_TICKS(STATUS)) == 0 ) {
        int32_t n = _pcount( counts );
        __cio_printf_at( 2, 0,
            "%3d procs: n/%d r/%d R/%d s/%d b/%d w/%d k/%d z/%d  RQ[%d,%d,%d]",
            n, counts[New],      counts[Ready],   counts[Running],
               counts[Sleeping], counts[Blocked], counts[Waiting],
               counts[Killed],   counts[Zombie],
            _queue_length(_ready[System]),  _queue_length(_ready[User]),
            _queue_length(_ready[Deferred])
        );
        // _sio_dump( true );
        // _ptable_dump( "Ptbl", false );
    }
#endif

    // time marches on!
    ++_system_time;

    // wake up any sleeping processes whose time has come
    //
    // we give them preference over the current process
    // (when it is scheduled again)

    do {

        // peek at the first member of the queue
        key_t key = _queue_kpeek( _sleeping );

        // the key value is the earliest wakeup time for any process
        // on the queue; if that's greater than the current time, or
        // if the queue is empty (indicated by a key value of 0),
        // there's nobody to awaken

        if( key == 0 || key > _system_time ) {
            break;
        }

        // ok, we need to wake someone up
        pcb_t *pcb = NULL;
        status_t status = _queue_remove( _sleeping, (void **) &pcb );
        assert1( status == E_SUCCESS && pcb != NULL );

        // schedule this process
        _schedule( pcb );

    } while( 1 );

    // check the current process to see if its time slice has expired
    _current->ticks -= 1;

    if( _current->ticks < 1 ) {
        // yes!  put it back on the ready queue
        _schedule( _current );
        // pick a new "current" process
        _dispatch();
    }

    // tell the PIC we're done
    __outb( PIC_PRI_CMD_PORT, PIC_EOI );
}

/*
** PUBLIC FUNCTIONS
*/

/**
** Name:  _clk_init
**
** Initializes the clock module
**
*/
void _clk_init( void ) {

    __cio_puts( " Clock:" );

    // start the pinwheel
    _pinwheel = (CLOCK_FREQUENCY / 10) - 1;
    _pindex = 0;

    // return to the dawn of time
    _system_time = 0;

    // configure the clock
    uint32_t divisor = TIMER_FREQUENCY / CLOCK_FREQUENCY;
    __outb( TIMER_CONTROL_PORT, TIMER_0_LOAD | TIMER_0_SQUARE );
    __outb( TIMER_0_PORT, divisor & 0xff );        // LSB of divisor
    __outb( TIMER_0_PORT, (divisor >> 8) & 0xff ); // MSB of divisor

    // create the sleep queue
    _sleeping = _queue_create( _cmp_wakeup );
    assert( _sleeping != NULL );

    // register the second-stage ISR
    __install_isr( INT_VEC_TIMER, _clk_isr );

    // report that we're all set
    __cio_puts( " done" );
}
