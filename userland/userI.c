#ifndef USER_I_H_
#define USER_I_H_

#include "users.h"
#include "ulib.h"

#ifndef MAX_CHILDREN
#define MAX_CHILDREN    50
#endif

/**
** User function I: exit, fork, exec, kill, sleep, write, getpid,
**
** Reports, then loops spawing userW, sleeps, kills two children, then
** loops checking the status of all its children
**
** Invoked as:  userI [ x [ n ] ]
**   where x is the ID character (defaults to 'i')
**         n is the number of children to spawn (defaults to 5)
*/

int32_t userI( int argc, char *argv[] ) {
    int count = 5;    // default child count
    char ch = 'i';    // default character to print
    int nap = 5;      // nap time
    char buf[128];
    pid_t children[MAX_CHILDREN];
    int nkids = 0;

    // process the command-line arguments
    if( argc > 1 ) {
        ch = argv[1][0];
        if( argc > 2 ) {
            count = str2int( argv[2], 10 );
        }
    }

    // secondary output (for indicating errors)
    char ch2[] = "*?*";
    ch2[1] = ch;

    // announce our presence
    write( CHAN_SIO, &ch, 1 );

    // set up the argument vector
    // we run:  userW 10 5

    ARGS4( userW, "userW", "W", "10", "5", NULL );

    for( int i = 0; i < count; ++i ) {
        pid_t whom = spawn( userW, argv_userW );
        if( whom < 0 ) {
            swrites( ch2 );
        } else {
            swritech( ch );
            children[nkids++] = whom;
        }
    }

    // let the children start
    sleep( SEC_TO_MS(nap) );

    // kill two of them
    int32_t status = kill( children[1] );
    if( status ) {
        sprint( buf, "!! %c: kill(%d) status %d\n", ch, children[1], status );
        cwrites( buf );
        children[1] = -42;
    }
    status = kill( children[3] );
    if( status ) {
        sprint( buf, "!! %c: kill(%d) status %d\n", ch, children[3], status );
        cwrites( buf );
        children[3] = -42;
    }

    // collect child information
    while( 1 ) {
        pid_t n = wait( NULL );
        if( n == E_NO_CHILDREN ) {
            // all done!
            break;
        }
        for( int i = 0; i < count; ++i ) {
            if( children[i] == n ) {
                sprint( buf, "== %c: child %d (%d)\n", ch, i, children[i] );
                cwrites( buf );
            }
        }
        sleep( SEC_TO_MS(nap) );
    };

    // let init() clean up after us!

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
