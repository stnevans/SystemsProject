#ifndef USER_V_H_
#define USER_V_H_

#include "users.h"
#include "ulib.h"

/**
** User function V:  exit, sleep, write
**
** Reports itself, then loops reporting the current time
**
** Invoked as:  userV  x  [ n  [ t ] ]
**   where x is the ID character
**         n is the iteration count (defaults to 3)
**         t is the sleep time (defaults to 2 seconds)
*/

int32_t userV( int argc, char *argv[] ) {
    int count = 3;    // default iteration count
    char ch = 'v';    // default character to print
    int nap = 2;      // nap time
    char buf[128];

    // process the command-line arguments
    if( argc < 2 ) {
        bad_args( "userV", 2, argc, argv );
    } else {
        ch = argv[1][0];
        if( argc > 2 ) {
            count = str2int( argv[2], 10 );
            if( argc > 3 ) {
                nap = str2int( argv[3], 10 );
            }
        }
    }

    // announce our presence
    time_t now = gettime();
    sprint( buf, "== %c starting @ %d\n", ch, now );
    cwrites( buf );

    write( CHAN_SIO, &ch, 1 );

    for( int i = 0; i < count; ++i ) {
        sleep( SEC_TO_MS(nap) );
        time_t now = gettime();
        sprint( buf, " [%c @ %d] ", ch, now );
        swrites( buf );
    }

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
