/**
** File:	users.c
**
** Author:	CSCI-452 class of 20205
**
** Contributor:
**
** Description:	User-level code.
*/

#include "common.h"
#include "users.h"

/*
** USER PROCESSES
**
** Each is designed to test some facility of the OS; see the users.h
** header file for a summary of which system calls are tested by
** each user function.
**
** Output from user processes is usually alphabetic.  Uppercase
** characters are "expected" output; lowercase are "erroneous"
** output.
**
** More specific information about each user process can be found in
** the header comment for that function (below).
**
** To spawn a specific user process, uncomment its SPAWN_x
** definition in the users.h header file.
*/

/*
** Prototypes for all user main routines (even ones that may not exist,
** for completeness)
*/

int32_t idle( int, char *[] );

int32_t main1( int, char *[] ); int32_t main2( int, char *[] );
int32_t main3( int, char *[] ); int32_t main4( int, char *[] );
int32_t main5( int, char *[] ); int32_t main6( int, char *[] );

int32_t userA( int, char *[] ); int32_t userB( int, char *[] );
int32_t userC( int, char *[] ); int32_t userD( int, char *[] );
int32_t userE( int, char *[] ); int32_t userF( int, char *[] );
int32_t userG( int, char *[] ); int32_t userH( int, char *[] );
int32_t userI( int, char *[] ); int32_t userJ( int, char *[] );
int32_t userK( int, char *[] ); int32_t userL( int, char *[] );
int32_t userM( int, char *[] ); int32_t userN( int, char *[] );
int32_t userO( int, char *[] ); int32_t userP( int, char *[] );
int32_t userQ( int, char *[] ); int32_t userR( int, char *[] );
int32_t userS( int, char *[] ); int32_t userT( int, char *[] );
int32_t userU( int, char *[] ); int32_t userV( int, char *[] );
int32_t userW( int, char *[] ); int32_t userX( int, char *[] );
int32_t userY( int, char *[] ); int32_t userZ( int, char *[] );

/*
** The user processes
**
** We #include the source code from the userland/ directory only if
** a specific process is being spawned.
**
** Remember to #include the code required by any process that will
** be spawned - e.g., userH spawns userZ.  The user code files all
** contain CPP include guards, so multiple inclusion of a source
** file shouldn't cause problems.
*/

#if defined(SPAWN_A) || defined(SPAWN_B) || defined(SPAWN_C)
#include "userland/main1.c"
#endif

#if defined(SPAWN_D) || defined(SPAWN_E)
#include "userland/main2.c"
#endif

#if defined(SPAWN_F) || defined(SPAWN_G)
#include "userland/main3.c"
#endif

#if defined(SPAWN_H)
#include "userland/userH.c"
#include "userland/userZ.c"
#endif

#if defined(SPAWN_I)
#include "userland/userI.c"
#include "userland/userW.c"
#endif

#if defined(SPAWN_J)
#include "userland/userJ.c"
#include "userland/userY.c"
#endif

#if defined(SPAWN_K) || defined(SPAWN_L)
#include "userland/main4.c"
#include "userland/userX.c"
#endif

#if defined(SPAWN_M) || defined(SPAWN_N)
#include "userland/main5.c"
#include "userland/userW.c"
#include "userland/userZ.c"
#endif

#if defined(SPAWN_P)
#include "userland/userP.c"
#endif

#if defined(SPAWN_Q)
#include "userland/userQ.c"
#endif

#if defined(SPAWN_R)
#include "userland/userR.c"
#endif

#if defined(SPAWN_S)
#include "userland/userS.c"
#endif

#if defined(SPAWN_T) || defined(SPAWN_U)
#include "userland/main6.c"
#include "userland/userW.c"
#endif

#if defined(SPAWN_V)
#include "userland/userV.c"
#endif

/*
** System processes - these should always be included here
*/

#include "userland/init.c"

#include "userland/idle.c"
