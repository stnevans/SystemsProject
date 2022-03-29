#ifndef MAIN_2_H_
#define MAIN_2_H_

#include "users.h"
#include "ulib.h"

/**
** User function main #2:  write
**
** Prints its ID, then loops N times delaying and printing, then returns
** without calling exit().  Verifies the return byte count from each call
** to write().
**
** Invoked as:  main2  x  n
**   where x is the ID character
**         n is the iteration count
*/

int32_t main2( int argc, char *argv[] ) {
    int n;
    int count = 30;   // default iteration count
    char ch = '2';    // default character to print
    char buf[128];

    // process the command-line arguments
    if( argc < 3 ) {
        bad_args( "main2", 3, argc, argv );
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
    return( 0 );
}

#endif
