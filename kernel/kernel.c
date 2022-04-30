/**
** @file kernel.c
**
** @author Numerous CSCI-452 classes
**
** Miscellaneous OS support routines.
*/

#define SP_KERNEL_SRC

#include "common.h"

#include "kernel.h"
#include "kmem.h"
#include "queues.h"
#include "clock.h"
#include "process.h"
#include "bootstrap.h"
#include "syscalls.h"
#include "cio.h"
#include "sio.h"
#include "scheduler.h"
#include "support.h"
#include "paging.h"
#include "filesystem.h"

// need addresses of some user functions
#include "users.h"

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

// character buffers, usable throughout the OS
// not guaranteed to retain their contents across an exception return
char b256[256];
char b512[512];

// Other kernel variables that could be defined here:
//
//     system time
//     pointer to the current process
//     information about the initial process
//         pid, PCB pointer
//     information about the idle process (if there is one)
//         pid, PCB pointer
//     information about active processes
//         static array of PCBs, active count, next available PID
//     queue variables
//     OS stack & stack pointer
//

// A separate stack for the OS itself
// (NOTE:  this assumes the OS is not reentrant!)
stack_t *_system_stack;
uint32_t *_system_esp;

// PCB for the init process
pcb_t *_init_pcb;

// table of state name strings
const char *_statestr[] = {
    [ Free     ] = "Free",
    [ New      ] = "New",
    [ Ready    ] = "Ready",
    [ Running  ] = "Running",
    [ Sleeping ] = "Sleeping",
    [ Blocked  ] = "Blocked",
    [ Waiting  ] = "Waiting",
    [ Killed   ] = "Killed",
    [ Zombie   ] = "Zombie"
};

// table of priority name strings
const char *_priostr[] = {
    [ System   ] = "System",
    [ User     ] = "User",
    [ Deferred ] = "Deferred"
};

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/**
** _kinit - system initialization routine
**
** Called by the startup code immediately before returning into the
** first user process.
*/
void _kinit( void ) {

    /*
    ** BOILERPLATE CODE - taken from basic framework
    **
    ** Initialize interrupt stuff.
    */

    __init_interrupts();  // IDT and PIC initialization

    /*
    ** Console I/O system.
    **
    ** Does not depend on the other kernel modules, so we can
    ** initialize it before we initialize the kernel memory
    ** and queue modules.
    */

#if defined(CONSOLE_SHELL)
    __cio_init( _kshell );
#else
    __cio_init( NULL );    // no console callback routine
#endif

#ifdef TRACE_CX
    // define a scrolling region in the top 7 lines of the screen
    __cio_setscroll( 0, 7, 99, 99 );
    // clear the top line
    __cio_puts_at( 0, 0, "*                                                                               " );
    // separator
    __cio_puts_at( 0, 6, "================================================================================" );
#endif

    /*
    ** TERM-SPECIFIC CODE STARTS HERE
    */

    /*
    ** Initialize various OS modules
    **
    ** Other modules (clock, SIO, syscall, etc.) are expected to
    ** install their own ISRs in their initialization routines.
    */

    __cio_puts( "System initialization starting.\n" );
    __cio_puts( "-------------------------------\n" );

    __cio_puts( "Modules:" );

    // call the module initialization functions, being
    // careful to follow any module precedence requirements
    //
    // classic order:  kmem; queue; everything else

    _km_init();     // MUST BE FIRST. Initializes the memory AND paging

    // other module initialization calls here
    _queue_init();  // MUST BE SECOND
    _pcb_init();
    _stk_init();
    _sys_init();
    _sched_init();
    _clk_init();
    _sio_init();

    __cio_puts("\nFile System set up starting.\n");
    f32_t *new_fs = make_Filesystem();
    __cio_puts("\nFile System set up complete.\n");
    

    __cio_puts( "\nModule initialization complete.\n" );
    __cio_puts( "-------------------------------\n" );
    __delay( 100 );  // about 2.5 seconds

    /*
    ** Other tasks typically performed here:
    **
    **  Enabling any I/O devices (e.g., SIO xmit/rcv)
    */

    // create the initial user process
    pcb_t *new = _pcb_alloc();
    assert( new != NULL );
    // _current = new;
    new->stack = _stk_alloc(NULL);
    assert( new->stack != NULL );

    // fill in the necessary fields
    new->pid = new->ppid = PID_INIT;
    new->state = New;
    new->quantum = Q_DEFAULT;
    new->priority = System;

    // command-line arguments
    char *args[2] = { "init", NULL };

    // set up the stack
    new->context = _stk_setup( new->stack, (uint32_t) init, args );
    new->pg_dir = copy_pg_dir(get_current_pg_dir());
    // add to the process table
    _processes[0] = new;
    _n_procs = 1;
    
    // add it to the ready queue and then give it the CPU
    _schedule( new );
    _dispatch();

    /*
    ** END OF TERM-SPECIFIC CODE
    **
    ** Finally, report that we're all done.
    */

    __cio_puts( "System initialization complete.\n" );
    __cio_puts( "-------------------------------\n" );
}

#ifdef CONSOLE_SHELL
/**
** _kshell - extremely simple shell for handling console input
**
** Called whenever we want to take input from the console and
** act upon it (e.g., for debugging the kernel)
**
** @param ch   The character that should be processed first
*/
void _kshell( int ch ) {

    // clear the input buffer
    (void) __cio_getchar();

    switch( ch ) {

    case 'x':   // FALL THROUGH
    case EOT:
        break;

    case '\r': // ignore CR and LF
    case '\n':
        break;

    case 'q':  // dump the queues
        // code to dump out any/all queues
        _queue_dump( "Sleep queue", _sleeping );
        _queue_dump( "Read queue", _reading );
        _queue_dump( "Ready queue[System]", _ready[System] );
        _queue_dump( "Ready queue[User]", _ready[User] );
        _queue_dump( "Ready queue[Deferred]", _ready[Deferred] );
        break;

    case 'a':  // dump the active table
        _ptable_dump( "\nActive processes", false );
        break;

    case 'p':  // dump the active table and all PCBs
        _ptable_dump( "\nActive processes", true );
        break;

    case 'c':  // dump context info for all active PCBs
        _context_dump_all( "\nContext dump" );
        break;

    case 's':  // dump stack info for all active PCBS
        __cio_puts( "\nActive stacks (w/5-sec. delays):\n" );
        for( int i = 0; i < N_PROCS; ++i ) {
            pcb_t *pcb = _processes[i];
            if( pcb != NULL && pcb->state != Free ) {
                __cio_printf( "pid %5d: ", pcb->pid );
                __cio_printf( "EIP %08x, ", pcb->context->eip );
                _stk_dump( NULL, pcb->stack, 12 );
            }
        }
        break;
 
    default:
        __cio_printf( "shell: unknown request '0x%02x'\n", ch );
        // FALL THROUGH

    case 'h':  // help message
        __cio_puts( "\nCommands:\n" );
        __cio_puts( "   a  -- dump the active table\n" );
        __cio_puts( "   c  -- dump contexts for active processes\n" );
        __cio_puts( "   h  -- this message\n" );
        __cio_puts( "   p  -- dump the active table and all PCBs\n" );
        __cio_puts( "   q  -- dump the queues\n" );
        __cio_puts( "   s  -- dump stacks for active processes\n" );
        __cio_puts( "   x  -- exit\n" );
        break;
    }

    // clear the input buffer
    (void) __cio_getchar();
}
#endif
