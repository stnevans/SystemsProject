#ifndef MAIN_5_H_
#define MAIN_5_H_

#include "users.h"
#include "ulib.h"

/**
** User function main #5:  exit, fork, exec, write
**
** Iterates spawning copies of userW (and possibly userZ), reporting
** their PIDs as it goes.
**
** Invoked as:  main5  b  n
**   where x is the ID character
**         b is the w&z boolean
**         n is the iteration count
*/

int32_t main( int argc, char *argv[] ) {
    int count = 5;  // default iteration count
    char ch = '5';  // default character to print
    int alsoZ = 0;  // also do userZ?
    char msg2[] = "*5*";

    // process the command-line arguments
    if( argc < 4 ) {
        bad_args( "main5", 4, argc, argv );
    } else {
        ch = argv[1][0];
        alsoZ = argv[2][0] == 't';
        count = str2int( argv[3], 10 );
    }

    // announce our presence
    write( CHAN_SIO, &ch, 1 );

    // set up the argument vector(s)

    // W:  15 iterations, 5-second sleep
    ARGS4( userW, "userW", "W", "15", "5", NULL );

    // Z:  15 iterations
    ARGS3( userZ, "userZ", "Z", "15", NULL );

    for( int i = 0; i < count; ++i ) {
        write( CHAN_SIO, &ch, 1 );
        pid_t whom = spawn( BIN_USERW, argv_userW );
        if( whom < 1 ) {
            swrites( msg2 );
        }
        if( alsoZ ) {
            whom = spawn( BIN_USERZ, argv_userZ );
            if( whom < 1 ) {
                swrites( msg2 );
            }
        }
    }

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
