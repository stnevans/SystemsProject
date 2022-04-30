#ifndef INIT_H_
#define INIT_H_

#include "users.h"
#include "ulib.h"

/**
** Initial process; it starts the other top-level user processes.
**
** Prints a message at startup, '+' after each user process is spawned,
** and '!' before transitioning to wait() mode to the SIO, and
** startup and transition messages to the console.  It also reports
** each child process it collects via wait() to the console along
** with that child's exit status.
*/

/*
** For the test programs in the baseline system, command-line arguments
** follow these rules.  The first (up to) three entries will be strings
** containing the following things:
**
**      argv[0] the name used to "invoke" this process
**      argv[1] the "character to print" (identifies the process)
**      argv[2] either an iteration count or a sleep time
**
** See the comment at the beginning of each user-code source file for
** information on the argument list that code expects.
*/

int32_t init( int argc, char *argv[] ) {
    pid_t whom;
    char ch = '+';
    static int invoked = 0;
    char buf[128];

    if( invoked > 0 ) {
        cwrites( "*** INIT RESTARTED??? ***\n" );
        for(;;);
    }

    cwrites( "Init started\n" );
    ++invoked;

    // ignore our command line
    (void) argc;
    (void) argv;

    // home up, clear on a TVI 925
    swritech( '\x1a' );
    // wait a bit
    DELAY(STD);

    // a bit of Dante to set the mood
    swrites( "\n\nSpem relinquunt qui huc intrasti!\n\n\r" );

    /*
    ** Start all the other users
    */

    // first, we start the idle process
    //
    // we do the fork() and execp() directly because we
    // want to start it with 'Deferred' priority.

    ARGS1( idle, "idle", "." );
    whom = fork();
    if( whom < 0 ) {
        cwrites( "init, fork() for idle failed\n" );
    } else {
        if( whom == 0 ) {
            execp( BIN_IDLE, Deferred, argv_idle );
            cwrites( "init, execp() for idle failed\n" );
            exit( FAILURE );
        } else {
            sprint( buf, "INIT: idle is pid %d\n", whom );
            cwrites( buf );
            swritech( ch );
        }
    }

    // wait a bit
    DELAY(STD);

    // Now, start the "ordinary" users
    cwrites( "INIT: starting user processeseses\n" );

    // We use spawn() for these, as it invokes execp() with
    // 'User' as the priority level.

    // set up for users A, B, and C initially
    swritech( 'a' );

#ifdef SPAWN_A
    swritech( 'a' );

    ARGS2( userA, "main1", "A", "30" );
    whom = spawn( BIN_MAIN1, argv_userA );
    if( whom < 0 ) {
        cwrites( "init, spawn() user A failed\n" );
    }
    swritech( ch );
    swritech( 'a' );
#endif

#ifdef SPAWN_B
    ARGS2( userB, "main1", "B", "30" );
    whom = spawn( BIN_MAIN1, argv_userB );
    if( whom < 0 ) {
        cwrites( "init, spawn() user B failed\n" );
    }
    swritech( ch );
    swritech( 'b' );
#endif

#ifdef SPAWN_C
    ARGS2( userC, "main1", "C", "30" );
    whom = spawn( BIN_MAIN1, argv_userC );
    if( whom < 0 ) {
        cwrites( "init, spawn() user C failed\n" );
    }
    swritech( ch );
    swritech( 'c' );
#endif

    // Users D and E are like A-C, but uses main2 instead

#ifdef SPAWN_D
    ARGS2( userD, "main2", "D", "20" );
    whom = spawn( BIN_MAIN2, argv_userD );
    if( whom < 0 ) {
        cwrites( "init, spawn() user D failed\n" );
    }
    swritech( ch );
    swritech( 'd' );
#endif

#ifdef SPAWN_E
    ARGS2( userE, "main2", "E", "20" );
    whom = spawn( BIN_MAIN2, argv_userE );
    if( whom < 0 ) {
        cwrites( "init, spawn() user E failed\n" );
    }
    swritech( ch );
    swritech( 'e' );
#endif

    // F and G behave the same way: report, sleep, exit
    // F sleeps for 20 seconds; G sleeps for 10 seconds

#ifdef SPAWN_F
    ARGS2( userF, "main3", "F", "20" );
    whom = spawn( BIN_MAIN3, argv_userF );
    if( whom < 0 ) {
        cwrites( "init, spawn() user F failed\n" );
    }
    swritech( ch );
    swritech( 'f' );
#endif

#ifdef SPAWN_G
    ARGS2( userG, "main3", "G", "10" );
    whom = spawn( BIN_MAIN3, argv_userG );
    if( whom < 0 ) {
        cwrites( "init, spawn() user G failed\n" );
    }
    swritech( ch );
    swritech( 'g' );
#endif

    // User H tests reparenting of orphaned children
    
#ifdef SPAWN_H
    ARGS2( userH, "userH", "H", "4" );
    whom = spawn( BIN_USERH, argv_userH );
    if( whom < 0 ) {
        cwrites( "init, spawn() user H failed\n" );
    }
    swritech( ch );
    swritech( 'h' );
#endif

    // User I spawns several children, kills one, and waits for all
    
#ifdef SPAWN_I
    ARGS1( userI, "userI", "I" );
    whom = spawn( BIN_USERI, argv_userI );
    if( whom < 0 ) {
        cwrites( "init, spawn() user I failed\n" );
    }
    swritech( ch );
    swritech( 'i' );
#endif

    // User J tries to spawn 2 * N_PROCS children

#ifdef SPAWN_J
    ARGS1( userJ, "userJ", "J" );
    whom = spawn( BIN_USERJ, argv_userJ );
    if( whom < 0 ) {
        cwrites( "init, spawn() user J failed\n" );
    }
    swritech( ch );
    swritech( 'j' );
#endif

    // Users K and L iterate spawning copies of userX and sleeping
    // for varying amounts of time.

#ifdef SPAWN_K
    ARGS2( userK, "main4", "K", "8" );
    whom = spawn( BIN_MAIN4, argv_userK );
    if( whom < 0 ) {
        cwrites( "init, spawn() user K failed\n" );
    }
    swritech( ch );
    swritech( 'k' );
#endif

#ifdef SPAWN_L
    ARGS2( userL, "main4", "L", "5" );
    whom = spawn( BIN_MAIN4, argv_userL );
    if( whom < 0 ) {
        cwrites( "init, spawn() user L failed\n" );
    }
    swritech( ch );
    swritech( 'l' );
#endif

    // Users M and N spawn copies of userW and userZ via main5

#ifdef SPAWN_M
    ARGS3( userM, "main5", "M", "f", "5" );
    whom = spawn( BIN_MAIN5, argv_userM );
    if( whom < 0 ) {
        cwrites( "init, spawn() user M failed\n" );
    }
    swritech( ch );
    swritech( 'm' );
#endif

#ifdef SPAWN_N
    ARGS3( userN, "main5", "N", "t", "5" );
    whom = spawn( BIN_MAIN5, argv_userN );
    if( whom < 0 ) {
        cwrites( "init, spawn() user N failed\n" );
    }
    swritech( ch );
    swritech( 'n' );
#endif

    // There is no user O

    // User P iterates, reporting system time and stats, and sleeping

#ifdef SPAWN_P
    ARGS3( userP, "userP", "P", "3", "2" );
    whom = spawn( BIN_USERP, argv_userP );
    if( whom < 0 ) {
        cwrites( "init, spawn() user P failed\n" );
    }
    swritech( ch );
    swritech( 'p' );
#endif

    // User Q tries to execute a bad system call

#ifdef SPAWN_Q
    ARGS1( userQ, "userQ", "Q" );
    whom = spawn( BIN_USERQ, argv_userQ );
    if( whom < 0 ) {
        cwrites( "init, spawn() user Q failed\n" );
    }
    swritech( ch );
    swritech( 'q' );
#endif

    // User R reads from the SIO one character at a time, forever

#ifdef SPAWN_R
    ARGS2( userR, "userR", "R", "10" );
    whom = spawn( BIN_USERR, argv_userR );
    if( whom < 0 ) {
        cwrites( "init, spawn() user R failed\n" );
    }
    swritech( ch );
    swritech( 'r' );
#endif

    // User S loops forever, sleeping on each iteration

#ifdef SPAWN_S
    ARGS2( userS, "userS", "S", "20" );
    whom = spawn( BIN_USERS, argv_userS );
    if( whom < 0 ) {
        cwrites( "init, spawn() user S failed\n" );
    }
    swritech( ch );
    swritech( 's' );
#endif

    // Users T and U run main6(); they spawn copies of userW,
    // then wait for them all or kill them all

#ifdef SPAWN_T
    // User T:  wait for any child each time
    // "main6 T 1 << 8 + 6"
    ARGS3( userT, "main6", "T", "w", "6" );
    whom = spawn( BIN_MAIN6, argv_userT );
    if( whom < 0 ) {
        cwrites( "init, spawn() user T failed\n" );
    }
    swritech( ch );
    swritech( 't' );
#endif

#ifdef SPAWN_U
    // User U:  kill all children
    ARGS3( userU, "main6", "U", "k", "6" );
    whom = spawn( BIN_MAIN6, argv_userU );
    if( whom < 0 ) {
        cwrites( "init, spawn() user U failed\n" );
    }
    swritech( ch );
    swritech( 'u' );
#endif

    // User V reports itself and the current time

#ifdef SPAWN_V
    // User V:  get and print the time
    ARGS3( userV, "userV", "V", "10", "5" );
    whom = spawn( BIN_USERV, argv_userV );
    if( whom < 0 ) {
        cwrites( "init, spawn() user V failed\n" );
    }
    swritech( ch );
    swritech( 'v' );
#endif

    // Users W through Z are spawned elsewhere

    swrites( " !!!\r\n\n" );

    /*
    ** At this point, we go into an infinite loop waiting
    ** for our children (direct, or inherited) to exit.
    */

    cwrites( "init() transitioning to wait() mode\n" );

    for(;;) {
        int32_t status;
        pid_t whom = wait( &status );

        // PIDs must be positive numbers!
        if( whom <= 0 ) {
            // cwrites( "INIT: wait() says 'no children'???\n" );
        } else {
            sprint( buf, "INIT: %d exit(%d)\n", whom, status );
            cwrites( buf );
        }
    }

    /*
    ** SHOULD NEVER REACH HERE
    */

    cwrites( "*** INIT IS EXITING???\n" );
    exit( 1 );

    return( 0 );  // shut the compiler up!
}

#endif
