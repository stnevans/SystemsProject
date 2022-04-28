#ifndef MAIN_1_H_
#define MAIN_1_H_

#include "users.h"
#include "ulib.h"

/**
** User function main #1:  exit, write
**
** Prints its ID, then loops N times delaying and printing, then exits.
** Verifies the return byte count from each call to write().
**
** Invoked as:  main1  x  n
**   where x is the ID character
**         n is the iteration count
*/

int32_t main( int argc, char *argv[] ) {
    int n;
    int count = 30;   // default iteration count
    char ch = '1';    // default character to print
    char buf[128];

    // process the command-line arguments
    if( argc < 3 ) {
        bad_args( "main1", 3, argc, argv );
    } else {
        ch = argv[1][0];
        count = str2int( argv[2], 10 );
    }

    // announce our presence
    n = swritech( ch );
    if( n != 1 ) {
        sprint( buf, "== %c, write #1 returned %d\n", ch, n );
        cwrites( buf );
    }

    // iterate and print the required number of other characters
    for( int i = 0; i < count; ++i ) {
        DELAY(STD);
        n = swritech( ch );
        if( n != 1 ) {
            sprint( buf, "== %c, write #2 returned %d\n", ch, n );
            cwrites( buf );
        }
    }

    // all done!
    exit( 0 );

    // should never reach this code; if we do, something is
    // wrong with exit(), so we'll report it

    char msg[] = "*1*";
    msg[1] = ch;
    n = write( CHAN_SIO, msg, 3 );    /* shouldn't happen! */
    if( n != 3 ) {
        sprint( buf, "User %c, write #3 returned %d\n", ch, n );
        cwrites( buf );
    }

    // this should really get us out of here
    return( 42 );
}

#endif
