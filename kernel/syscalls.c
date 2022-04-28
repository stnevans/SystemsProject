/**
** @file syscalls.c
**
** @author CSCI-452 class of 20215
**
** System call implementations
*/

#define SP_KERNEL_SRC

#include "common.h"

#include "x86arch.h"
#include "x86pic.h"
#include "uart.h"

#include "support.h"
#include "bootstrap.h"

#include "syscalls.h"
#include "scheduler.h"
#include "process.h"
#include "stacks.h"
#include "clock.h"
#include "cio.h"
#include "sio.h"
#include "paging.h"
#include "elf_loader.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// The system call jump table
//
// Initialized by _sys_init() to ensure that the code::function mappings
// are correct even if the code values should happen to change.

static void (*_syscalls[N_SYSCALLS])( pcb_t *curr );

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/**
** Name:  _sys_isr
**
** System call ISR
**
** @param vector    Vector number for the clock interrupt
** @param code      Error code (0 for this interrupt)
*/
static void _sys_isr( int vector, int code ) {

    // Keep the compiler happy.
    (void) vector;
    (void) code;

    // If there is no current process, we're in deep trouble.
    assert( _current != NULL );

    // Much less likely to occur, but still potentially problematic.
    assert2( _current->context != NULL );

    // Retrieve the system call code.
    uint32_t syscode = REG( _current, eax );

    // Validate the code.
    if( syscode >= N_SYSCALLS ) {
        // Uh-oh....
        __sprint( b256, "PID %d bad syscall 0x%x", _current->pid, syscode );
        WARNING( b256 );
        // Force a call to exit().
        syscode = SYS_exit;
        ARG(_current,1) = E_BAD_PARAM;
    }

    // Handle the system call.
    _syscalls[syscode]( _current );

    // Tell the PIC we're done.
    __outb( PIC_PRI_CMD_PORT, PIC_EOI );
}

/**
** Second-level syscall handlers
**
** All have this prototype:
**
**    static void _sys_NAME( pcb_t * );
**
** Values being returned to the user are placed into the EAX
** field in the context save area for that process.
*/

/**
** _sys_exit - terminate the calling process
**
** implements:
**      void exit( int32_t status );
**
** does not return
*/
static void _sys_exit( pcb_t *curr ) {

#if TRACING_SYSCALLS
    __cio_printf( "--> _sys_exit, pid %d", curr->pid );
#endif

    // set the termination status for this process
    curr->exit_status = ARG(curr,1);

#if TRACING_EXIT
    __cio_printf( " parent %d, status %d\n", curr->ppid, curr->exit_status );
#endif

    // perform all necessary exit processing
    _perform_exit( curr );

    // need a new current process
    _dispatch();
}

/**
** _sys_fork - create a new, duplicate process
**
** implements:
**      pid_t fork( void );
**
** returns:
**      parent - PID of new child, or -1 on error
**      child  - 0
*/
static void _sys_fork( pcb_t *curr ) {

#if TRACING_SYSCALLS
    __cio_printf( "--> _sys_fork, pid %d\n", curr->pid );
#endif

    // Make sure there's room for another process!
    if( _n_procs >= N_PROCS ) {
        // No room at the inn!
        RET(curr) = E_NO_PROCS;
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", E_NO_PROCS );
#endif
        return;
    }

    // First, allocate a PCB.
    pcb_t *new = _pcb_alloc();
    new->pg_dir = copy_pg_dir(curr->pg_dir);
    if( new == NULL ) {
        RET(curr) = E_NO_PROCS;
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", E_NO_PROCS );
#endif
        return;
    }

    // Create the stack for the child.
    new->stack = _stk_alloc(new->pg_dir);
    if( new->stack == NULL ) {
        _pcb_free( new );
        RET(curr) = E_NO_PROCS;
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", E_NO_PROCS );
#endif
        return;
    }

    // Duplicate the parent's stack.
    for(int i = 0; i < STACK_PAGES*2; i++){
        char * val = (char *) new->stack;
        val -= 0xdf000000;
        map_virt_page_to_phys((virt_addr)(0xdf000000 + val + i * 4096), (phys_addr)(val + i * 4096));    
    }
    __memcpy( (void *)new->stack, (void *)curr->stack, sizeof(stack_t) );
    for(int i = 0; i < STACK_PAGES*2; i++){
        char * val = (char *) new->stack;
        val -= 0xdf000000;
        unmap_virt(_current->pg_dir, (virt_addr)(0xdf000000 + val + i * 4096));    
    }
    // Set the child's identity.
    new->pid = _next_pid++;
    new->ppid = curr->pid;
    new->state = New;
    new->quantum = Q_DEFAULT;

    /*
    ** Now, we need to update the ESP and EBP values in the child's
    ** stack.  The problem is that because we duplicated the parent's
    ** stack, these pointers are still pointing back into that stack,
    ** which will cause problems as the two processes continue to execute.
    */

    // Figure out the byte offset from one stack to the other.
    int32_t offset = (void *) new->stack - (void *) curr->stack;

    // Add this to the child's context pointer.
    new->context = (context_t *) (((void *)curr->context) + offset);

    // Fix the child's ESP and EBP values IFF they're non-zero.
    if( REG(new,ebp) != 0 ) {
        REG(new,ebp) += offset;
    }
    if( REG(new,esp) != 0 ) {
        REG(new,esp) += offset;
    }

    // Follow the EBP chain through the child's stack.
    uint32_t *bp = (uint32_t *) REG(new,ebp);
    while( bp && *bp ) {
        *bp += offset;
        bp = (uint32_t *) *bp;
    }
    // char dst[32];
    // __sprint(dst, "bp %llx\n", bp);
    // swrites(dst);

    // Set the return values for the two processes.
    RET(curr) = new->pid;
    RET(new) = 0;
    // _context_dump( "fork: new 1:", new->context );
    // __delay(400);

    // Add the new process to the process table.
    int ix;
    for( ix = 0; ix < N_PROCS; ++ix ) {
        if( _processes[ix] == NULL ) {
            break;
        }
    }

    // Did we find an open slot?
    if( ix >= N_PROCS ) {
        PANIC( 0, "no empty slot in non-full process table" );
    }

    // Yes - record the new process.
    _processes[ix] = new;
    ++_n_procs;

    // Schedule the child, and let the parent continue.
    _schedule( new );
#if TRACING_SYSRET
    __cio_printf( "<-- %08x\n", RET(curr) );
#endif
    // _context_dump( "fork: current", curr->context );
    // _context_dump( "fork: new", new->context );
    // __delay(400);
}

/**
** _sys_execp - replace the memory image of this process with a
**              different program
**
** implements:
**      void exec( uint32_t phys_addr, prio_t prio, char *args[] );
**
** returns:
**      only on failure
*/
static void _sys_execp( pcb_t *curr ) {
    uint32_t entry = ARG(curr,1);
    prio_t prio = ARG(curr,2);
    char **args = (char **) ARG(curr,3);

#if TRACING_SYSCALLS
    __cio_printf( "--> _sys_execp, pid %d\n", curr->pid );
#endif

    uint32_t elf_entry = elf_load_program(entry);

    if (!elf_entry) {
        __sprint( b256, "*** execp(): could not load binary at address: %x\n",
                entry );
        PANIC( 0, b256 );
    }

    // Set up the new stack for the user.
    context_t *ct = _stk_setup( curr->stack, elf_entry, args );
    assert( ct != NULL );

    // Copy the context pointer into the current PCB.
    curr->context = ct;

    // It's also the current ESP for the process.
    curr->context->esp = (uint32_t) ct;

    // Assign the specified priority.
    curr->priority = prio;

    /*
    ** Decision:  (A) schedule this process and dispatch another,
    ** (B) just allow this one to continue executing in its current
    ** time slice, or (C) reset its time slice and let it continue?
    **
    ** We choose option A.
    */

    _schedule( curr );
    _dispatch();
}

/**
** _sys_kill - terminate a process with extreme prejudice
**
** implements:
**      status_t kill( pid_t victim );
**
** returns:
**      status of the kill attempt
*/
static void _sys_kill( pcb_t *curr ) {
    pid_t victim = ARG(curr,1);

#if TRACING_SYSCALLS
    __cio_printf( "--> _sys_kill, pid %d\n", curr->pid );
#endif
    
    // POTENTIAL DANGER:  What if we try kill(init) or kill(idle)?
    // Might want to guard for that here!

    // kill(0) is a request to kill the calling process
    if( victim == 0 ) {
        victim = curr->pid;
    }
    
    // locate the victim
    pcb_t *pcb = NULL;
    for( int i = 0; i < N_PROCS; ++i ) {
        if( _processes[i] != NULL && _processes[i]->pid == victim ) {
            pcb = _processes[i];
            break;
        }
    }

    // did we find the victim?
    if( pcb == NULL ) {
        // nope!
        RET(curr) = E_NOT_FOUND;
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", E_NOT_FOUND );
#endif
        return;
    }

    // can't declare this inside the switch statement....
    pcb_t *tmp;
    
    // how we process the victim depends on its current state:
    switch( pcb->state ) {
    
        // for the first two of these states, the process is on
        // a queue somewhere; just mark it as 'Killed', and when it
        // comes off that queue via _schedule() or _dispatch() we
        // will clean it up

    case Ready:
        // remove it from the ready queue
        tmp = _queue_remove_specific( _ready[pcb->priority], pcb );
        // verify that we got the correct PCB
        assert( tmp == pcb );
        // mark it as killed and clean it up
        pcb->exit_status = E_KILLED;
        _perform_exit( pcb );
        RET(curr) = E_SUCCESS;
        break;

    case Sleeping:
        // remove it from the sleep queue
        tmp = _queue_remove_specific( _sleeping, pcb );
        // verify that we got the correct PCB
        assert( tmp == pcb );
        // mark it as killed and clean it up
        pcb->exit_status = E_KILLED;
        _perform_exit( pcb );
        RET(curr) = E_SUCCESS;

    case Blocked:
        // we don't want to deque it because it's waiting for some
        // type of device event; instead, we mark it as Killed,
        // and when it comes up for scheduling or dispatching, we'll
        // clean it up then
        pcb->state = Killed;
        pcb->exit_status = E_KILLED;
        RET(curr) = E_SUCCESS;
        break;

    case Running:  // current process
        // we have met the enemy, and he is us!
        curr->exit_status = E_KILLED;
        _perform_exit( curr );
        // need a new 'current process"
        _dispatch();
        break;
    
    case Waiting:
        // much like 'Running', except that it's not the current
        // process, so we don't have to dispatch another one
        pcb->exit_status = E_KILLED;
        _perform_exit( pcb );
        RET(curr) = E_SUCCESS;
        break;
    
    case Killed:    // FALL THROUGH
    case Zombie:
        // you can't kill something if it's already dead
        RET(curr) = E_NOT_FOUND;
        break;
        
    default:
        // this is a really bad potential problem - we have an unexpected
        // (or bogus) process state, so we report that
        __sprint( b256, "*** kill(): victim %d, unknown state %d\n",
                pcb->pid, pcb->state );
        PANIC( 0, b256 );
    }
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", RET(curr) );
#endif
}

/**
** _sys_wait - wait for a child process to terminate
**
** implements:
**      pid_t wait( int32_t *status );
**
** returns:
**      pid of the terminated child, or E_NO_CHILDREN (intrinsic)
**      exit status of the child via a non-NULL 'status' parameter
*/
static void _sys_wait( pcb_t *curr ) {
    pcb_t *child = NULL;
    int nchildren = 0;

#if TRACING_SYSCALLS
    __cio_printf( "--> _sys_wait, pid %d\n", curr->pid );
#endif

    /*
    ** We want to do two things here:  (1) find out whether or
    ** not this process has any children in the system, and (2)
    ** find out whether any of them have terminated.  We'll loop
    ** until we find a Zombie child process or have gone through
    ** all the slots in the process table.
    **
    ** Note that we don't care which child process we reap here;
    ** there could be several, but we only need to find one.
    */

    for( int i = 0; i < N_PROCS; ++i ) {

        // only look at valid entries
        if( _processes[i] != NULL ) {

            // is this one of our children?
            if( _processes[i]->ppid == curr->pid ) {

                // yes - count it
                ++nchildren;

                // see if it has terminated
                if( _processes[i]->state == Zombie ) {
                    // yes!  remember it
                    child = _processes[i];
                    break;
                }
            }
        }
    }

    // no children at all
    if( nchildren == 0 ) {
        RET(curr) = E_NO_CHILDREN;
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", E_NO_CHILDREN );
#endif
        return;
    }

    // at least one child; did we find one to collect?
    if( child == NULL ) {

        // no - mark the parent as "Waiting"
        curr->state = Waiting;

        // select a new current process
        _dispatch();
        return;
    }

    // found a Zombie; collect its information and clean it up
    RET(curr) = child->pid;
    int *stat = (int *) ARG(curr,1);

    // if stat is NULL, the parent doesn't want the status
    if( stat != NULL ) {
        *stat = child->exit_status;
    }

    _pcb_cleanup( child );
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", RET(curr) );
#endif
}

/**
** _sys_sleep - put the current process to sleep for some length of time
**
** implements:
**      void sleep( uint32_t ms );
*/
static void _sys_sleep( pcb_t *curr ) {
    uint32_t ms = ARG(curr,1);

#if TRACING_SYSCALLS
    __cio_printf( "--> _sys_sleep, pid %d\n", curr->pid );
#endif

    if( ms == 0 ) {
        _schedule( curr );
    } else {
        curr->wakeup = _system_time + MS_TO_TICKS(ms);
        curr->state = Sleeping;
        status_t status = _queue_add( _sleeping,
                            (void *) curr, curr->wakeup );
        if( status != E_SUCCESS ) {
            __sprint(b256,"cannot put %d to sleep",curr->pid);
            WARNING( b256 );
            _schedule( curr );
        }
    }

    _dispatch();
}

/**
** _sys_read - read into a buffer from a stream
**
** implements:
**      int32_t read( int chan, void *buffer, uint32_t length );
**
** returns:
**      input data (in 'buffer')
**      number of bytes read, or an error code (intrinsic)
*/
static void _sys_read( pcb_t *curr ) {
    int n = 0;
    char *buf = (char *) ARG(curr,2);
    uint32_t length = ARG(curr,3);

#if TRACING_SYSCALLS
    __cio_printf( "--> _sys_read, pid %d\n", curr->pid );
#endif

    // try to get the next character(s)
    switch( ARG(curr,1) ) {
    case CHAN_CIO:
        // console input is non-blocking
        if( __cio_input_queue() < 1 ) {
            RET(curr) = E_NO_DATA;
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", E_NO_DATA );
#endif
            return;
        }
        // at least one character
        n = __cio_gets( buf, length );
        break;

    case CHAN_SIO:
        // this may block the process; if so,
        // _sio_reads() will dispatch a new one
        n = _sio_reads( buf, length );
        break;

    default:
        // bad channel code
        RET(curr) = E_BAD_CHAN;
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", E_BAD_CHAN );
#endif
        return;
    }

    // if there was data, return the byte count to the process;
    // otherwise, block the process until data is available
    if( n > 0 ) {

        RET(curr) = n;
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", n );
#endif

    } else {

        // mark it as blocked
        curr->state = Blocked;

        // put it on the SIO input queue
        assert( _queue_add(_reading,curr,0) == E_SUCCESS );

        // select a new current process
        _dispatch();
    }
}

/**
** _sys_write - write from a buffer to a stream
**
** implements:
**      int32_t write( int chan, const void *buffer, uint32_t length );
**
** returns:
**      number of bytes written, or an error code (intrinsic)
*/
static void _sys_write( pcb_t *curr ) {
    uint32_t chan = ARG(curr,1);
    char *buf = (char *) ARG(curr,2);
    uint32_t length = ARG(curr,3);

#if TRACING_SYSCALLS
    __cio_printf( "--> _sys_write, pid %d\n", curr->pid );
#endif

    // this is almost insanely simple, but it does separate the
    // low-level device access fromm the higher-level syscall implementation

    switch( chan ) {
    case CHAN_CIO:
        __cio_write( buf, length );
        RET(curr) = length;
        break;

    case CHAN_SIO:
        _sio_write( buf, length );
        RET(curr) = length;
        break;

    default:
        RET(curr) = E_BAD_CHAN;
        break;
    }
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", RET(curr) );
#endif
}

/**
** _sys_sysstat - return process statistics to the user
**
** implements:
**      int sysstat( uint32_t counts[] );
**
** returns:
**      per-state count of processes in the system (via the parameter)
**      number of processes, or an error code (intrinsic)
*/
static void _sys_sysstat( pcb_t *curr ) {
    uint32_t *counts = (uint32_t *) ARG(curr,1);

#if TRACING_SYSCALLS
    __cio_printf( "--> _sys_sysstat, pid %d\n", curr->pid );
#endif

    // verify that the user gave us a pointer we could use
    if( counts == NULL ) {
        RET(curr) = E_BAD_PARAM;
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", E_BAD_PARAM );
#endif
        return;
    }

    // collect the information
    int32_t n = _pcount( counts );

    RET(curr) = n;
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", _n_procs );
#endif
}

/**
** _sys_getpid - retrieve the PID of this process
**
** implements:
**      pid_t getpid( void );
**
** returns:
**      the PID of the calling process
*/
static void _sys_getpid( pcb_t *curr ) {

#if TRACING_SYSCALLS
    __cio_printf( "--> _sys_getpid, pid %d\n", curr->pid );
#endif
    RET(curr) = curr->pid;
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", curr->pid );
#endif
}

/**
** _sys_getppid - retrieve the PID of the parent of this process
**
** implements:
**      pid_t getppid( void );
**
** returns:
**      the PID of the parent of the calling process
*/
static void _sys_getppid( pcb_t *curr ) {

#if TRACING_SYSCALLS
    __cio_printf( "--> _sys_getppid, pid %d\n", curr->pid );
#endif
    RET(curr) = curr->ppid;
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", curr->ppid );
#endif
}

/**
** _sys_gettime - retrieve the current system time
**
** implements:
**      time_t gettime( void );
**
** returns:
**      the current system time
*/
static void _sys_gettime( pcb_t *curr ) {

#if TRACING_SYSCALLS
    __cio_printf( "--> _sys_gettime, pid %d\n", curr->pid );
#endif
    RET(curr) = _system_time;
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", _system_time );
#endif
}

/**
** _sys_getprio - retrieve the current process priority
**
** implements:
**      prio_t getprio( void );
**
** returns:
**      the current system time
*/
static void _sys_getprio( pcb_t *curr ) {

#if TRACING_SYSCALLS
    __cio_printf( "--> _sys_getprio, pid %d\n", curr->pid );
#endif
    RET(curr) = curr->priority;
#if TRACING_SYSRET
        __cio_printf( "<-- %08x\n", curr->priority );
#endif
}

/*
** PUBLIC FUNCTIONS
*/

/**
** Name:  _sys_init
**
** Syscall module initialization routine
**
** Dependencies:
**    Must be called after _sio_init()
*/
void _sys_init( void ) {

    __cio_puts( " Syscall:" );

    /*
    ** Set up the syscall jump table.  We do this here
    ** to ensure that the association between syscall
    ** code and function address is correct even if the
    ** codes change.
    */

    _syscalls[ SYS_exit ]     = _sys_exit;
    _syscalls[ SYS_fork ]     = _sys_fork;
    _syscalls[ SYS_execp ]    = _sys_execp;
    _syscalls[ SYS_kill ]     = _sys_kill;
    _syscalls[ SYS_wait ]     = _sys_wait;
    _syscalls[ SYS_sleep ]    = _sys_sleep;
    _syscalls[ SYS_read ]     = _sys_read;
    _syscalls[ SYS_write ]    = _sys_write;
    _syscalls[ SYS_sysstat ]  = _sys_sysstat;
    _syscalls[ SYS_getpid ]   = _sys_getpid;
    _syscalls[ SYS_getppid ]  = _sys_getppid;
    _syscalls[ SYS_gettime ]  = _sys_gettime;
    _syscalls[ SYS_getprio ]  = _sys_getprio;

    // install the second-stage ISR
    __install_isr( INT_VEC_SYSCALL, _sys_isr );

    // all done
    __cio_puts( " done" );
}

/**
** _perform_exit - do the real work for exit() and some kill() calls
**
** @param victim  Pointer to the PCB for the exiting process
*/
void _perform_exit( pcb_t *victim ) {
    pcb_t *parent = NULL;
    pcb_t *zombie = NULL;

#if TRACING_EXIT
    __cio_printf( "--> perform exit, victim PCB %08x pid %d ppid %d\n",
            (uint32_t) victim, victim->pid, victim->ppid );
#endif

    // set its state
    victim->state = Zombie;

    /*
    ** We need to locate the parent of this process.  We also need
    ** to reparent any children of this process.  We do these in
    ** a single loop.
    */

    for( int i = 0; i < N_PROCS; ++i ) {
        register pcb_t *curr = _processes[i];

        // make sure this is a valid entry
        if( curr == NULL ) {
            continue;
        }

        // see if this is our parent; if it isn't, see if
        // it's a child of the terminating process
        if( curr->pid == victim->ppid ) {

            // found the parent!
            parent = curr;
#if TRACING_EXIT
    __cio_printf( "--> perform exit, parent PCB %08x pid %d\n",
            (uint32_t) parent, parent->pid );
#endif

        } else if( curr->ppid == victim->pid ) {
#if TRACING_EXIT
    __cio_printf( "--> perform exit, child PCB %08x pid %d",
            (uint32_t) curr, curr->pid );
#endif

            curr->ppid = PID_INIT;
            if( curr->state == Zombie ) {
                // if it's already a zombie, remember it, so we
                // can pass it on to 'init'
                zombie = _processes[i];
#if TRACING_EXIT
    __cio_puts( " is a zombie\n" );
#endif
            }
#if TRACING_EXIT
    __cio_puts( " reparented\n" );
#endif
        }
    }

    // every process must have a parent, even if it's 'init'
    assert( parent != NULL );

    /*
    ** If we found a child that was already terminated, we need to
    ** wake up the init process if it's already waiting.
    **
    ** Note: we only need to do this for one Zombie child process -
    ** init will loop and collect the others after it finishes with
    ** this one.
    */

    if( zombie != NULL && _init_pcb->state == Waiting ) {
        
        // *****************************************************
        // This code assumes that Waiting processes are *not* in
        // a queue, but instead are just in the process table with
        // a state of 'Waiting'.  This simplifies life immensely,
        // because we don't need to dequeue it - we can just
        // schedule it and let it go.
        // *****************************************************

        // intrinsic return value is the PID
        RET(_init_pcb) = zombie->pid;

        // may also want to return the exit status
        int32_t *ptr = (int32_t *) ARG(_init_pcb,1);
        if( ptr != NULL ) {
            // *****************************************************
            // Potential VM issue here!  This code assigns the exit
            // status into a variable in the parent's address space.
            // This works in the baseline because we aren't using
            // any type of memory protection.  If address space
            // separation is implemented, this code will very likely
            // STOP WORKING, and will need to be fixed.
            // *****************************************************
            *ptr = zombie->exit_status;
        }
#if TRACING_EXIT
    __cio_printf( "--> perform exit, first zombie %d given to init\n",
            zombie->pid );
#endif

        // all done - schedule 'init', and clean up the zombie
        _schedule( _init_pcb );
        _pcb_cleanup( zombie );
    }

    // if the parent is already waiting, wake it up
    if( parent->state == Waiting ) {

        // intrinsic return value is the PID
        RET(parent) = victim->pid;

        // may also want to return the exit status
        int32_t *ptr = (int32_t *) ARG(parent,1);
        if( ptr != NULL ) {
            // *****************************************************
            // Potential VM issue here!  This code assigns the exit
            // status into a variable in the parent's address space.
            // This works in the baseline because we aren't using
            // any type of memory protection.  If address space
            // separation is implemented, this code will very likely
            // STOP WORKING, and will need to be fixed.
            // *****************************************************
            *ptr = victim->exit_status;
        }
#if TRACING_EXIT
    __cio_printf( "--> perform exit, victim %d given to parent %d\n",
            victim->pid, parent->pid );
#endif

        // all done - schedule the parent, and clean up the zombie
        _schedule( parent );
        _pcb_cleanup( victim );

    } else {

        // parent isn't waiting, so we become a Zombie
        victim->state = Zombie;

    }
    /*
    ** Note: we don't call _dispatch() here - we leave that for 
    ** the calling routine, as it's possible we don't need to
    ** choose a new current process.
    */
}
