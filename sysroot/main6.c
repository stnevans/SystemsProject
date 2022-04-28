#ifndef MAIN_6_H_
#define MAIN_6_H_

#include "users.h"
#include "ulib.h"

/**
** User function main #6: exit, fork, exec, kill, wait, sleep, write, getpid
**
** Reports, then loops spawing userW, sleeps, then waits for or kills
** all its children.
**
** Invoked as:  main6  x  b  c
**   where x is the ID character
**         b is wait/kill indicator ('w' or 'k')
**         c is the child count
*/

#ifndef MAX_CHILDREN
#define MAX_CHILDREN    50
#endif

int32_t main( int argc, char *argv[] ) {
    int count = 3;    // default child count
    char ch = '6';    // default character to print
    int nap = 8;      // nap time
    char what = 1;    // wait or kill?  default is wait
    char buf[128];
    pid_t children[MAX_CHILDREN];
    int nkids = 0;

    // process the command-line arguments
    if( argc < 4 ) {
        bad_args( "main6", 4, argc, argv );
    } else {
        ch = argv[1][0];
        what = argv[2][0] == 'w';   // 'w' -> wait, else -> kill
        count = str2int( argv[3], 10 );
    }

    // secondary output (for indicating errors)
    char ch2[] = "*?*";
    ch2[1] = ch;

    // announce our presence
    write( CHAN_SIO, &ch, 1 );

    // set up the argument vector
    ARGS4( userW, "userW", "W", "10", "5", NULL );

    for( int i = 0; i < count; ++i ) {
        pid_t whom = spawn( BIN_USERW, argv_userW );
        if( whom < 0 ) {
            swrites( ch2 );
        } else {
            children[nkids++] = whom;
        }
    }

    // let the children start
    sleep( SEC_TO_MS(nap) );

    // collect exit status information

    int n = 0;

    do {
        pid_t this;
        int32_t status;

        // are we waiting for or killing it?
        if( what ) {
            this = wait( &status );
        } else {
            this = kill( n++ );
        }

        // what was the result?
        if( this != E_SUCCESS ) {
            // uh-oh - something went wrong
            // "no children" means we're all done
            if( this != E_NO_CHILDREN ) {
                if( what ) {
                    sprint( buf, "!! %c: wait() status %d\n", ch, this );
                } else {
                    sprint( buf, "!! %c: kill() status %d\n", ch, this );
                }
                cwrites( buf );
            }
            // regardless, we're outta here
            break;
        } else {
            int i;
            for( i = 0; i < nkids; ++i ) {
                if( children[i] == this ) {
                    break;
                }
            }
            if( i < nkids ) {
                sprint( buf, "== %c: child %d (%d) status %d\n",
                        ch, i, this, status );
            } else {
                sprint( buf, "!! %c: child PID %d term, NOT FOUND\n",
                        ch, this );
            }
        }

        cwrites( buf );

    } while( n < nkids );

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
