#ifndef USER_Z_H_
#define USER_Z_H_

#include "users.h"
#include "ulib.h"

/**
** User function Z:  exit, sleep, write, getpid, getppid
**
** Prints its ID, then records PID and PPID, loops printing its ID,
** and finally re-gets PPID for comparison.  Yields after every second
** ID print in the loop.
**
** This code is used as a handy "spawn me" test routine; it is spawned
** by several of the standard test processes.
**
** Invoked as:  userZ  x  [ n ]
**   where x is the ID character
**         n is the iteration count (defaults to 10)
*/

int32_t main( int argc, char *argv[] ) {
    int count = 10;   // default iteration count
    char ch = 'z';    // default character to print
    char buf[128], buf2[128];

    // process the command-line arguments
    if( argc < 2 ) {
        bad_args( "userZ", 2, argc, argv );
    } else {
        ch = argv[1][0];
        if( argc > 2 ) {
            count = str2int( argv[2], 10 );
        }
    }

    // announce our presence
    swritech( ch );

    // record the relevant PIDs and report it to the console
    pid_t me = getpid();
    pid_t parent = getppid();

    sprint( buf, "%c[%d/%d]", ch, me, parent );

    sprint( buf2, "== %s running\n", buf );
    cwrites( buf2 );

    // iterate for a while; occasionally yield the CPU
    for( int i = 0; i < count ; ++i ) {
        swrites( buf );
        DELAY(STD);
        if( i & 1 ) {
            sleep( 0 );
        }
    }

    // get "new" parent PID and report the result
    parent = getppid();

    sprint( buf2, "== %s exiting, parent now %d\n", buf, parent );
    cwrites( buf2 );

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
