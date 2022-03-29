#ifndef USER_P_H_
#define USER_P_H_

#include "users.h"
#include "ulib.h"

/**
** User function P:  exit, sleep, write, gettime
**
** Reports itself, then loops reporting the current time
**
** Invoked as:  userP  x  [ n  [ t ] ]
**   where x is the ID character
**         n is the iteration count (defaults to 3)
**         t is the sleep time (defaults to 2 seconds)
*/

int32_t userP( int argc, char *argv[] ) {
    int count = 3;    // default iteration count
    char ch = 'p';    // default character to print
    int nap = 2;      // nap time
    char buf[512];
    int32_t counts[N_STATES];

    // process the command-line arguments
    if( argc < 2 ) {
        bad_args( "userP", 2, argc, argv );
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
    sprint( buf, "== %c running, start at %d\n", ch, now );
    cwrites( buf );

    write( CHAN_SIO, &ch, 1 );

    for( int i = 0; i < count; ++i ) {
        sleep( SEC_TO_MS(nap) );
        now = gettime();
        int num = sysstat( counts );
        sprint( buf, "== %c @ time %d, %d procs:\n", ch, now, num );
        char buf2[32];
        for( int i = 0; i < N_STATES; ++i ) {
            sprint( buf2, " %d/%d", i, counts[i] );
            strcat( buf, buf2 );
        }
        strcat( buf, "\n" );
        cwrites( buf );
        write( CHAN_SIO, &ch, 1 );
    }

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
