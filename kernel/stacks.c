/**
** @file stacks.c
**
** @author  CSCI-452 class of 20215
**
** Stack module implementation
*/

#define SP_KERNEL_SRC

#include "common.h"

#include "bootstrap.h"
#include "stacks.h"
#include "kernel.h"
#include "scheduler.h"
#include "paging.h"
// also need the exit_helper() entry point
void exit_helper( void );

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// stack management
//
// our "free list" uses the first word in the stack
// as a pointer to the next free stack

static stack_t *_stack_list;

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
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
void _stk_init( void ) {

    __cio_puts( " Stacks:" );

    // no preallocation here, so the initial free list is empty
    _stack_list = NULL;

    // allocate the first stack for the OS
    _system_stack = _stk_alloc(NULL);
    assert( _system_stack != NULL );

    // set the initial ESP for the OS - it should point to the
    // next-to-last uint32 in the stack, so that when the code
    // at isr_save switches to the system stack and pushes the
    // error code and vector number, ESP is aligned at a multiple
    // of 16 address.
    _system_esp = ((uint32_t *) (_system_stack + 1)) - 2;

    // all done!
    __cio_puts( " done" );
}

/**
** _stk_alloc() - allocate a stack
**
** @return a pointer to the allocated stack, or NULL
*/
stack_t *_stk_alloc( struct page_directory * pg_dir ) {
    stack_t *new;

    // see if there is an available stack
    if( _stack_list == NULL ) {

        // none available - create a new one
        char * val = _km_page_alloc( STACK_PAGES*2 );
        for(int i = 0; i < STACK_PAGES*2; i++){
            if(!pg_dir){
                map_virt_page_to_phys((virt_addr) (0xdf000000 + val + i * 4096), (phys_addr) (val + i * 4096));
            }else{
                map_virt_page_to_phys_pg_dir(pg_dir, (virt_addr) (0xdf000000 + val + i * 4096), (phys_addr) (val + i * 4096));    
            }
        }
        val += 0xdf000000;
        new = (stack_t *) val;
    } else {

        // OK, we know that there is at least one free stack;
        // just take the first one from the list

        new = _stack_list;



        // Map the page for the relevant process
        char * val = (char *) new;
        val -= 0xdf000000;

        for(int i = 0; i < STACK_PAGES*2; i++){
            if(!pg_dir){
                map_virt_page_to_phys((virt_addr) (0xdf000000 + val + i * 4096), (phys_addr)(val + i * 4096));
            }else{
                map_virt_page_to_phys_pg_dir(pg_dir, (virt_addr) (0xdf000000 + val + i * 4096), (phys_addr)(val + i * 4096));    
            }
        }

        // Map the pages for us!
        if(pg_dir){
            for(int i = 0; i < STACK_PAGES*2; i++){
                map_virt_page_to_phys((virt_addr) (0xdf000000 + val + i * 4096), (phys_addr)(val + i * 4096));
            }
        }
        // unlink it by making its successor the new head of
        // the list.  this is strange, because GCC is weird
        // about doing something like
        //     _stack_list = (stack_t *) new[0];
        // because 'new' is an array type
        //
        _stack_list = (stack_t *) ((uint32_t *)new)[0];
        
        // clear out the fields in this one just to be safe
        __memclr( new, sizeof(stack_t) );
        //Unmap the pages for us
        if(pg_dir){
            for(int i = 0; i < STACK_PAGES*2; i++){
                unmap_virt(_current->pg_dir, (virt_addr) (0xdf000000 + val + i * 4096));
            }
        }
    }

#if TRACING_STACKS
    __cio_printf( "STK: allocated %08x\n", (uint32_t) new );
#endif
    // pass it back to the caller
    return( new );
}

/**
** _stk_free() - return a stack to the free list
**
** Deallocates the supplied stack
**
** @param stk   The stack to be returned to the free list
*/
void _stk_free( stack_t *stk ) {
#if TRACING_STACKS
    __cio_printf( "STK: freeing %08x\n", (uint32_t) stk );
#endif

    // sanity check!
    if( stk == NULL ) {
        return;
    }

    // just stick this one at the front of the list

    // start by making its first word point to the
    // current head of the free list.  again, we have
    // to work around the "array type" issue here

    ((uint32_t *)stk)[0] = (uint32_t) _stack_list;

    // now, this one is the new head of the list

    _stack_list = stk;
}

/*
** Process management/control
*/

/**
** _stk_setup - set up the stack for a new process
**
** @param stk    - The stack to be set up
** @param entry  - Entry point for the new process
** @param args   - Argument vector to be put in place
**
** @return A pointer to the context_t on the stack, or NULL
*/
context_t *_stk_setup( stack_t *stk, uint32_t entry, char *args[] ) {

    /*
    ** We must duplicate the argv array here, because the first
    ** thing we do before setting up the new stack for the user
    ** is to clear out the old contents.  This is a potential
    ** problem if we are called via the _sys_execp() system call,
    ** because "args" is actually on the user stack.
    **
    ** Figure out how many arguments & argument chars there are.
    */

    int argbytes = 0;
    int argc = 0;

    for( argc = 0; args[argc] != NULL; ++argc ) {
        argbytes += __strlen( args[argc] ) + 1;
    }

    // Round up the byte count to the next multiple of four.
    argbytes = (argbytes + 3) & 0xfffffffc;

#if TRACING_STACK
    __cio_printf( "=== _stk_setup(%08x,%08x) %d args:",
            (uint32_t) stk, entry, argc );
    for( int i = 0; i < argc; ++i ) {
        __cio_printf( " '%s'", args[i] );
    }
    __cio_putchar( '\n' );
#endif

    /*
    ** Allocate the arrays.  We are safe using dynamic arrays here
    ** because we're using the OS stack, not the user stack.  Once
    ** we have copied the strings, it's safe to clear the stack.
    **
    ** We want the argstrings and argv arrays to contain all zeroes.
    ** The C standard states, in section 6.7.8, that
    **
    **   "21 If there are fewer initializers in a brace-enclosed list
    **       than there are elements or members of an aggregate, or
    **       fewer characters in a string literal used to initialize an
    **       array of known size than there are elements in the array,
    **       the remainder of the aggregate shall be initialized
    **       implicitly the same as objects that have static storage
    **       duration."
    **
    ** Sadly, because we're using variable-sized arrays, we can't
    ** rely on this, so we have to call __memclr() instead. :-(  In
    ** truth, it doesn't really cost us much more time, but it's an
    ** annoyance.
    */

    char argstrings[ argbytes ];
    char *argv[ argc + 1 ];

    __memclr( argstrings, argbytes );
    __memclr( argv, (argc + 1) * sizeof(char *) );

    // Next, duplicate the argument strings, and create pointers to
    // each one in our argv.
    char *tmp = argstrings;
    for( int i = 0; i < argc; ++i ) {
        int nb = __strlen(args[i]) + 1; // bytes (incl. NUL) in this string
        __strcpy( tmp, args[i] );   // add to our buffer
        argv[i] = tmp;              // remember where it was
        tmp += nb;                  // move on
    }

    // trailing NULL pointer
    argv[argc] = NULL;

#if TRACING_STACK
    __cio_puts( "=== buffer: '" );
    for( int i = 0; i < argbytes; ++i ) {
        __put_char_or_code( argstrings[i] );
    }
    __cio_printf( "'\n=== _stk_setup, temp %d args (%d bytes):",
            argc, argbytes );
    for( int i = 0; i <= argc; ++i ) {
        __cio_printf( " [%d] ", i );
        if( argv[i] ) {
            __cio_printf( "'%s'", argv[i] );
        } else {
            __cio_puts( "NULL" );
        }
    }
    __cio_putchar( '\n' );
#endif

    // Now that we have duplicated the strings, we can clear out the
    // old contents of the stack.
    __memclr( stk, sizeof(stack_t) );

    /*
    ** Set up the initial stack contents for a (new) user process.
    **
    ** We reserve one longword at the bottom of the stack to hold a
    ** pointer to where argv is on the stack.
    ** 
    ** Above that, we simulate a call from exit_helper() with an
    ** argument vector by pushing the arguments and then the argument
    ** count.  We follow this up by pushing the address of the entry point
    ** of exit_helper() as a "return address".  Above that, we place a
    ** context_t area that is initialized with the standard initial register
    ** contents.
    **
    ** The low end of the stack will contain these values:
    **
    **      esp ->  context      <- context save area
    **              ...          <- context save area
    **              context      <- context save area
    **              exit_helper  <- return address for faked call to main()
    **              argc         <- argument count for main()
    **         /->  argv         <- argv pointer for main()
    **         |     ...         <- argv array w/trailing NULL
    **         |     ...         <- argv character strings
    **         \--- ptr          <- last word in stack
    **
    ** Stack alignment rules for the SysV ABI i386 supplement dictate that
    ** the 'argc' parameter must be at an address that is a multiple of 16;
    ** see below for more information.
    */

    // Pointer to the last word in stack.
    uint32_t *ptr = ((uint32_t *)( stk + 1 )) - 1;

    // Pointer to where the arg strings should be filled in.
    uint32_t *fill = (uint32_t *) ( (uint32_t) ptr - argbytes );

    // Copy over the argv strings.
    __memcpy( (void *)fill, argstrings, argbytes );

    // Remember where the strings are (for later)
    char *strings = (char *) fill;
  
    /*
    ** Next, we need to copy over the argv pointers.  Start by
    ** determining where 'argc' should go.
    **
    ** Stack alignment is controlled by the SysV ABI i386 supplement,
    ** version 1.2 (June 23, 2016), which states in section 2.2.2:
    **
    **   "The end of the input argument area shall be aligned on a 16
    **   (32 or 64, if __m256 or __m512 is passed on stack) byte boundary.
    **   In other words, the value (%esp + 4) is always a multiple of 16
    **   (32 or 64) when control is transferred to the function entry
    **   point. The stack pointer, %esp, always points to the end of the
    **   latest allocated stack frame."
    **
    ** Isn't technical documentation fun?  Ultimately, this means that
    ** the first parameter to main() should be on the stack at an address
    ** that is a multiple of 16.
    **
    ** The space needed for argc, argv, and the argv array itself is
    ** argc + 3 words (argc+1 for the argv entries, plus one word each
    ** for argc and argv).  We back up that much from 'fill'.
    */

    int nwords = argc + 3;
    uint32_t *argcptr = fill - nwords;

    /*
    ** Next, back up until the low-order four bits are zeroes.  Because
    ** we're moving to a lower address whose upper 28 bits are identical to
    ** the address we currently have, we can do this with a bitwise AND to
    ** just turn off the lower four bits.  Once that's done, we can reset
    ** our 'fill' pointer so that it points to argv[0].
    */

    // fill = ((uint32_t *)(((uint32_t)argcptr) & 0xfffffff0)) + nwords - 1;
    argcptr = (uint32_t *) ( ((uint32_t)argcptr) & 0xfffffff0 );
    fill = argcptr + 2;

    /*
    ** Copy in 'argc' and 'argv', and set our backup argv pointer.
    */

    *argcptr = argc;
    *ptr = *(argcptr + 1) = (uint32_t) fill;

    /*
    ** Next, we copy in all argc+1 pointers.  This is complicated slightly
    ** by the need to adjust the pointers; they currently point into the
    ** local argstrings array.  We do this by adding the distance between
    ** the start of the argstrings array and the duplicate of that data on
    ** the stack.
    */

    // Calculate the distance between the two argstring arrays.
    int32_t distance = strings - &argstrings[0];

    // Adjust and copy the string pointers.
    for( int i = 0; i <= argc; ++i ) {
        if( argv[i] != NULL ) {
            *fill = ((uint32_t) argv[i]) + distance;
        } else {
            *fill = NULL;
        }
        ++fill;
    }

    // reset 'fill' to where argc was placed
    fill = argcptr;

#if TRACING_STACK
    // get argv from the stack
    char **targs = (char **) *(fill + 1);
    __cio_printf( "=== _stk_setup, copied %d args:", argc );
    for( int i = 0; i < argc; ++i ) {
        __cio_printf( " '%s'", targs[i] );
    }
    __cio_putchar( '\n' );
#endif

    // The dummy return address goes right above 'argc'.
    *--fill = (uint32_t) exit_helper;

    /*
    ** Now, we need to set up the initial context for the executing
    ** process.
    **
    ** When this process is dispatched, the context restore code will
    ** pop all the saved context information off the stack, leaving the
    ** "return address" on the stack as if the main() for the process
    ** had been "called" from the exit_helper() function.  When main()
    ** returns, it will "return" to the entry point of exit_helper(),
    ** which will then call exit().
    */

    // Locate the context save area on the stack.
    context_t *ct = ((context_t *) fill) - 1;

    /*
    ** We cleared the entire stack earlier, so all the context
    ** fields currently contain zeroes.  We now need to fill in
    ** all the important fields.
    */

    ct->eflags = DEFAULT_EFLAGS;    // IE enabled, PPL 0
    ct->eip = entry;                // initial EIP
    ct->cs = GDT_CODE;              // segment registers
    ct->ss = GDT_STACK;
    ct->ds = ct->es = ct->fs = ct->gs = GDT_DATA;

// #if TRACING_STACK
    // __cio_printf( "=== context @ %08x\n", ct );
    // _context_dump( "=== new context", ct );
    // __delay(200);
// #endif

    /*
    ** Return the new context pointer to the caller.  It will be our
    ** caller's responsibility to schedule this process.
    */
    
    return( ct );
}

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
** @param s     The stack to dump out
** @param lim   Limit on the number of words to dump (0 for all)
*/

// buffer sizes (rounded up a bit)
#define HBUFSZ      48
#define CBUFSZ      24

void _stk_dump( const char *msg, stack_t *stk, uint32_t limit ) {
    int words = sizeof(stack_t) / sizeof(uint32_t);
    int eliding = 0;
    char oldbuf[HBUFSZ], buf[HBUFSZ], cbuf[CBUFSZ];
    uint32_t addr = (uint32_t ) stk;
    uint32_t *sp = (uint32_t *) stk;
    char hexdigits[] = "0123456789ABCDEF";

    // if a limit was specified, dump only that many words

    if( limit > 0 ) {
        words = limit;
        if( (words & 0x3) != 0 ) {
            // round up to a multiple of four
            words = (words & 0xfffffffc) + 4;
        }
        // skip to the new starting point
        sp += (STACK_WORDS - words);
        addr = (uint32_t) sp;
    }

    __cio_puts( "*** stack" );
    if( msg != NULL ) {
        __cio_printf( " (%s):\n", msg );
    } else {
        __cio_puts( ":\n" );
    }

    /**
    ** Output lines begin with the 8-digit address, followed by a hex
    ** interpretation then a character interpretation of four words:
    **
    ** aaaaaaaa*..xxxxxxxx..xxxxxxxx..xxxxxxxx..xxxxxxxx..cccc.cccc.cccc.cccc
    **
    ** Output lines that are identical except for the address are elided;
    ** the next non-identical output line will have a '*' after the 8-digit
    ** address field (where the '*' is in the example above).
    */

    oldbuf[0] = '\0';

    while( words > 0 ) {
        register char *bp = buf;   // start of hex field
        register char *cp = cbuf;  // start of character field
        uint32_t start_addr = addr;

        // iterate through the words for this line

        for( int i = 0; i < 4; ++i ) {
            register uint32_t curr = *sp++;
            register uint32_t data = curr;

            // convert the hex representation

            // two spaces before each entry
            *bp++ = ' ';
            *bp++ = ' ';

            for( int j = 0; j < 8; ++j ) {
                uint32_t value = (data >> 28) & 0xf;
                *bp++ = hexdigits[value];
                data <<= 4;
            }

            // now, convert the character version
            data = curr;

            // one space before each entry
            *cp++ = ' ';

            for( int j = 0; j < 4; ++j ) {
                uint32_t value = (data >> 24) & 0xff;
                *cp++ = (value >= ' ' && value < 0x7f) ? (char) value : '.';
                data <<= 8;
            }
        }
        *bp = '\0';
        *cp = '\0';
        words -= 4;
        addr += 16;

        // if this line looks like the last one, skip it

        if( __strcmp(oldbuf,buf) == 0 ) {
            ++eliding;
            continue;
        }

        // it's different, so print it

        // start with the address
        __cio_printf( "%08x%c", start_addr, eliding ? '*' : ' ' );
        eliding = 0;

        // print the words
        __cio_printf( "%s %s\n", buf, cbuf );

        // remember this line
        __memcpy( (uint8_t *) oldbuf, (uint8_t *) buf, HBUFSZ );
    }
}
