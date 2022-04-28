#ifndef USER_S_H_
#define USER_S_H_

#include "users.h"
#include "ulib.h"

/**
** User function S:  exit, sleep, write
**
** Reports itself, then loops forever, sleeping on each iteration
**
** Invoked as:  userS  x  [ s ]
**   where x is the ID character
**         s is the sleep time (defaults to 20)
*/

int32_t main( int argc, char *argv[] ) {
    char ch = 's';    // default character to print
    int nap = 20;     // nap time
    char buf[128];

    // process the command-line arguments
    if( argc < 2 ) {
        bad_args( "userS", 2, argc, argv );
    } else {
        ch = argv[1][0];
        if( argc > 2 ) {
            nap = str2int( argv[2], 10 );
        }
    }

    // announce our presence
    write( CHAN_SIO, &ch, 1 );
    sprint( buf, "userS sleeping %d(%d)\n", nap, SEC_TO_MS(nap) );
    cwrites( buf );

    for(;;) {
        sleep( SEC_TO_MS(nap) );
        write( CHAN_SIO, &ch, 1 );
    }

    sprint( buf, "!! %c exiting!?!?!?\n", ch );
    cwrites( buf );
    exit( 1 );

    return( 42 );  // shut the compiler up!
}

#endif
