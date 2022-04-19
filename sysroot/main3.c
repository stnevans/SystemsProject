#ifndef MAIN_3_H_
#define MAIN_3_H_

#include "users.h"
#include "ulib.h"

/**
** User function main #3:  exit, sleep, write
**
** Prints its ID, then loops N times sleeping and printing, then exits.
**
** Invoked as:  main3  x  s
**   where x is the ID character
**         s is the sleep time in seconds
*/

int32_t main( int argc, char *argv[] ) {
    char ch = '3';    // default character to print
    int nap = 10;     // default sleep time

    // process the command-line arguments
    if( argc < 3 ) {
        bad_args( "main3", 3, argc, argv );
    } else {
        ch = argv[1][0];
        nap = str2int( argv[2], 10 );
    }

    // announce our presence a little differently
    report( ch, getpid() );

    write( CHAN_SIO, &ch, 1 );

    for( int i = 0; i < 30 ; ++i ) {
        sleep( SEC_TO_MS(nap) );
        write( CHAN_SIO, &ch, 1 );
    }

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
