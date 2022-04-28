#ifndef USER_Q_H_
#define USER_Q_H_

#include "users.h"
#include "ulib.h"

/**
** User function Q:  exit, write, bogus
**
** Reports itself, then tries to execute a bogus system call
**
** Invoked as:  userQ  x
**   where x is the ID character
*/

int32_t main( int argc, char *argv[] ) {
    char ch = 'q';    // default character to print
    char buf[128];

    // process the command-line arguments
    if( argc < 2 ) {
        bad_args( "userQ", 2, argc, argv );
    } else {
        ch = argv[1][0];
    }

    // announce our presence
    write( CHAN_SIO, &ch, 1 );

    // try something weird
    bogus();

    // should not have come back here!
    sprint( buf, "!!!!! %c returned from bogus syscall!?!?!\n", ch );
    cwrites( buf );

    exit( 1 );

    return( 42 );  // shut the compiler up!
}

#endif
