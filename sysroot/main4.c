#ifndef MAIN_4_H_
#define MAIN_4_H_

#include "users.h"
#include "ulib.h"

/**
** User function main #4:  exit, fork, exec, sleep, write, getpid
**
** Loops, spawning N copies of userX and sleeping between spawns.
**
** Invoked as:  main4  x  n
**   where x is the ID character
**         n is the iteration count (defaults to 5)
*/

int32_t main( int argc, char *argv[] ) {
    int count = 5;    // default iteration count
    char ch = '4';    // default character to print
    int nap = 30;     // nap time
    char msg2[] = "*4*";
    char buf[32];

    // process the command-line arguments
    if( argc < 2 ) {
        bad_args( "main4", 2, argc, argv );
    } else {
        ch = argv[1][0];
    }
    if( argc > 2 ) {
        count = str2int( argv[2], 10 );
    }

    // announce our presence
    write( CHAN_SIO, &ch, 1 );

    // second argument to X is our PID * 100 plus the iteration number
    pid_t me = getpid() * 100;

    ARGS3( userX, "userX", "X", buf, NULL );

    for( int i = 0; i < count ; ++i ) {
        write( CHAN_SIO, &ch, 1 );
        sprint( buf, "%d", me + i );
        int whom = spawn( userX, argv_userX );
        if( whom < 0 ) {
            swrites( msg2 );
        } else {
            write( CHAN_SIO, &ch, 1 );
        }
        sleep( SEC_TO_MS(nap) );
    }

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
