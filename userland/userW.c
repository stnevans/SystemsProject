#ifndef USER_W_H_
#define USER_W_H_

#include "users.h"
#include "ulib.h"

/**
** User function W:  exit, sleep, write, getpid, gettime
**
** Reports its presence, then iterates 'n' times printing identifying
** information and sleeping, before exiting.
**
** Invoked as:  userW  x  [ n  [ t ] ]
**   where x is the ID character
**         n is the iteration count (defaults to 20)
**         t is the sleep time (defaults to 3 seconds)
*/

int32_t userW( int argc, char *argv[] ) {
    int count = 20;   // default iteration count
    char ch = 'w';    // default character to print
    int nap = 3;      // nap length
    char buf[128];

    // process the command-line arguments
    if( argc < 2 ) {
        bad_args( "userW", 2, argc, argv );
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
    report( ch, getpid() );

    uint32_t time = gettime();

    sprint( buf, " %c[%d] ", ch, time );

    for( int i = 0; i < count ; ++i ) {
        swrites( buf );
        sleep( SEC_TO_MS(nap) );
    }

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
