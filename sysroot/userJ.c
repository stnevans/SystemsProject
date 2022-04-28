#ifndef USER_J_H_
#define USER_J_H_

#include "users.h"
#include "ulib.h"

/**
** User function J:  exit, fork, exec, write, getpid
**
** Reports, tries to spawn lots of children, then exits
**
** Invoked as:  userJ  x  [ n ]
**   where x is the ID character
**         n is the number of children to spawn (defaults to 2 * N_PROCS)
*/

int32_t main( int argc, char *argv[] ) {
    int count = 2 * N_PROCS;   // number of children to spawn
    char ch = 'J';             // default character to print

    // process the command-line arguments
    if( argc < 2 ) {
        bad_args( "userJ", 2, argc, argv );
    } else {
        ch = argv[1][0];
        if( argc > 2 ) {
            count = str2int( argv[2], 10 );
        }
    }

    // announce our presence
    write( CHAN_SIO, &ch, 1 );

    // set up the command-line arguments
    ARGS3( userY, "userY", "Y", "10", NULL );

    for( int i = 0; i < count ; ++i ) {
        int32_t whom = spawn( BIN_USERY, argv_userY );
        if( whom < 0 ) {
            write( CHAN_SIO, "!j!", 3 );
        } else {
            write( CHAN_SIO, &ch, 1 );
        }
    }

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
