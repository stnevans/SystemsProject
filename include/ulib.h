/**
** @file ulib.h
**
** @author Numerous CSCI-452 classes
**
** Declarations for user-level library functions
**
** This module implements a simple collection of support functions
** similar to the standard C library.
*/

#ifndef ULIB_H_
#define ULIB_H_

#include "common.h"

/*
** General (C and/or assembly) definitions
*/

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
*/

/*
** Types
*/

/*
** Globals
*/

/*
** Prototypes
*/

/*
**********************************************
** SYSTEM CALLS
**********************************************
*/

/**
** exit - terminate the calling process
**
** usage:   exit(status);
**
** @param status   Termination status of this process
**
** @return Does not return
*/
void exit( int32_t status );

/**
** fork - create a new process
**
** usage:   pid = fork();
**
** @return 0 to the child, -1 on error or the child's PID to the parent
*/
pid_t fork( void );

/**
** execp - replace this program with a different one
**
** usage:   execp(entry,prio,args)
**
** @param entry The entry point of the new code
** @param prio  The desired priority for this process
** @param args  Argument vector for the process
**
** @returns Only on failure
*/
void execp( int32_t (*entry)(int,char*[]), prio_t prio, char *args[] );

/**
** kill - terminate a process with extreme prejudice
**
** usage:   n = kill(pid);
**
** @param pid The PID of the desired victim, or 0 for this process
**
** @returns E_SUCCESS on success, else < 0 on an error
*/
status_t kill( pid_t pid );

/**
** wait - wait for a child process to terminate
**
** usage:   pid = wait(&status);
**
** @param status Pointer to int32_t into which the child's status is placed,
**               or NULL
**
** @returns The PID of the terminated child, or an error code
**
** If there are no children in the system, returns an error code (*status
** is unchanged).
**
** If there are one or more children in the system and at least one has
** terminated but hasn't yet been cleaned up, cleans up that process and
** returns its information; otherwise, blocks until a child terminates.
*/
pid_t wait( int32_t *status );

/**
** sleep - put the current process to sleep for some length of time
**
** usage:   sleep(n);
**
** @param n Desired sleep time (in MS), or 0 to yield the CPU
*/
void sleep( uint32_t msec );

/**
** read - read into a buffer from a stream
**
** usage:   n = read(channel,buf,length)
**
** @param chan   I/O stream to read from
** @param buf    Buffer to read into
** @param length Maximum capacity of the buffer
**
** @returns  The count of bytes transferred, or an error code
*/
int32_t read( int chan, void *buffer, uint32_t length );

/**
** write - write from a buffer to a stream
**
** usage:   n = write(channel,buf,length)
**
** @param chan   I/O stream to write to
** @param buf    Buffer to write from
** @param length Maximum capacity of the buffer
**
** @returns  The count of bytes transferred, or an error code
*/
int32_t write( int chan, const void *buffer, uint32_t length );

/**
** sysstat - retrieve counts of processes at each different state
**
** usage:   n = sysstat(&counts);
**
** @param counts Pointer to int32_t array into which the counts
**               of processes will be placed
**
** @returns The number of processes in the system
*/
int sysstat( int32_t *status );

/**
** getpid - retrieve PID of this process
**
** usage:   n = getpid();
**
** @returns The PID of this process
*/
pid_t getpid( void );

/**
** getppid - retrieve PID of the parent of this process
**
** usage:   n = getppid();
**
** @returns The PID of the parent of this process
*/
pid_t getppid( void );

/**
** gettime - retrieve the current system time
**
** usage:   n = gettime();
**
** @returns The current system time
*/
time_t gettime( void );

/**
** getprio - retrieve the priority value for the current process
**
** usage:   n = getprio();
**
** @returns The priority of the current process
*/
prio_t getprio( void );

/**
** bogus - a bogus system call, for testing our syscall ISR
**
** usage:   bogus();
**
** @return Does not return
*/
void bogus( void );

/*
**********************************************
** CONVENIENT "SHORTHAND" VERSIONS OF SYSCALLS
**********************************************
*/

/**
** spawn - create a new process
**
** usage:   pid = spawn(entry,args);
**
** Performs a fork(); on success, the child performs an execp()
** with User as the priority value.
**
** @param entry The function which is the entry point of the new code
** @param args  The argument vector for the new process
**
** @returns PID of the new process, or an error code
*/
pid_t spawn( int32_t (*entry)(int,char*[]), char *args[] );

/** 
** exec - replace this program with a different one
**
** usage:   exec(entry,args)
**
** Calls execp(entry,getprio(),args)
**
** @param entry The entry point of the new code
** @param args  Argument vector for the process
**
** @returns Only on failure
*/
void exec( int32_t (*entry)(int,char*[]), char *args[] );

/**
** cwritech(ch) - write a single character to the console
**
** @param ch The character to write
**
** @returns The return value from calling write()
*/
int32_t cwritech( char ch );

/**
** cwrites(str) - write a NUL-terminated string to the console
**
** @param str The string to write
**
*/
int32_t cwrites( const char *str );

/**
** cwrite(buf,leng) - write a sized buffer to the console
**
** @param buf  The buffer to write
** @param leng The number of bytes to write
**
** @returns The return value from calling write()
*/
int32_t cwrite( const char *buf, uint32_t leng );

/**
** swritech(ch) - write a single character to the SIO
**
** @param ch The character to write
**
** @returns The return value from calling write()
*/
int32_t swritech( char ch );

/**
** swrites(str) - write a NUL-terminated string to the SIO
**
** @param str The string to write
**
** @returns The return value from calling write()
*/
int32_t swrites( const char *str );

/**
** swrite(buf,leng) - write a sized buffer to the SIO
**
** @param buf  The buffer to write
** @param leng The number of bytes to write
**
** @returns The return value from calling write()
*/
int32_t swrite( const char *buf, uint32_t leng );

/*
**********************************************
** STRING MANIPULATION FUNCTIONS
**********************************************
*/

/**
** str2int(str,base) - convert a string to a number in the specified base
**
** @param str   The string to examine
** @param base  The radix to use in the conversion
**
** @return The converted integer
*/
int str2int( register const char *str, register int base );

/**
** strlen(str) - return length of a NUL-terminated string
**
** @param str The string to examine
**
** @return The length of the string, or 0
*/
uint32_t strlen( const char *str );

/**
** strcpy(dst,src) - copy a NUL-terminated string
**
** @param dst The destination buffer
** @param src The source buffer
**
** @return The dst parameter
**
** NOTE:  assumes dst is large enough to hold the copied string
*/
char *strcpy( register char *dst, register const char *src );

/**
** strcat(dst,src) - append one string to another
**
** @param dst The destination buffer
** @param src The source buffer
**
** @return The dst parameter
**
** NOTE:  assumes dst is large enough to hold the resulting string
*/
char *strcat( register char *dst, register const char *src );

/**
** strcmp(s1,s2) - compare two NUL-terminated strings
**
** @param s1 The first source string
** @param s2 The second source string
**
** @return negative if s1 < s2, zero if equal, and positive if s1 > s2
*/
int strcmp( register const char *s1, register const char *s2 );

/**
** pad(dst,extra,padchar) - generate a padding string
**
** @param dst     Pointer to where the padding should begin
** @param extra   How many padding bytes to add
** @param padchar What character to pad with
**
** @return Pointer to the first byte after the padding
**
** NOTE: does NOT NUL-terminate the buffer
*/
char *pad( char *dst, int extra, int padchar );

/**
** padstr(dst,str,len,width,leftadjust,padchar - add padding characters
**                                               to a string
**
** @param dst        The destination buffer
** @param str        The string to be padded
** @param len        The string length, or -1
** @param width      The desired final length of the string
** @param leftadjust Should the string be left-justified?
** @param padchar    What character to pad with
**
** @return Pointer to the first byte after the padded string
**
** NOTE: does NOT NUL-terminate the buffer
*/
char *padstr( char *dst, char *str, int len, int width,
                   int leftadjust, int padchar );

/**
** sprint(dst,fmt,...) - formatted output into a string buffer
**
** @param dst The string buffer
** @param fmt Format string
**
** The format string parameter is followed by zero or more additional
** parameters which are interpreted according to the format string.
**
** NOTE:  assumes the buffer is large enough to hold the result string
**
** NOTE:  relies heavily on the x86 parameter passing convention
** (parameters are pushed onto the stack in reverse order as
** 32-bit values).
*/
void sprint( char *dst, char *fmt, ... );

/*
**********************************************
** MISCELLANEOUS USEFUL SUPPORT FUNCTIONS
**********************************************
*/

/**
** exit_helper() - dummy "startup" function
**
** calls exit(%eax) - serves as the "return to" code for main()
** functions, in case they don't call exit() themselves
*/
void exit_helper( void );

/**
** cvt_dec(buf,value)
**
** convert a 32-bit signed value into a NUL-terminated character string
**
** @param buf    Destination buffer
** @param value  Value to convert
**
** @return The number of characters placed into the buffer
**          (not including the NUL)
**
** NOTE:  assumes buf is large enough to hold the resulting string
*/
int cvt_dec( char *buf, int32_t value );

/**
** cvt_hex(buf,value)
**
** convert a 32-bit unsigned value into an (up to) 8-character
** NUL-terminated character string
**
** @param buf    Destination buffer
** @param value  Value to convert
**
** @return The number of characters placed into the buffer
**          (not including the NUL)
**
** NOTE:  assumes buf is large enough to hold the resulting string
*/
int cvt_hex( char *buf, uint32_t value );

/**
** cvt_oct(buf,value)
**
** convert a 32-bit unsigned value into an (up to) 11-character
** NUL-terminated character string
**
** @param buf   Destination buffer
** @param value Value to convert
**
** @return The number of characters placed into the buffer
**          (not including the NUL)
**
** NOTE:  assumes buf is large enough to hold the resulting string
*/
int cvt_oct( char *buf, uint32_t value );

/**
** report(ch,pid) 
**
** Report to the console that user 'ch' is running as 'pid'
**
** @param ch   The one-character name of the user process
** @param whom The PID of the process
*/
void report( char ch, pid_t whom );

/**
** bad_args(who,ac,av)
**
** report that a user program was invoked with bad arguments
**
** @param who   Which program this is
** @param n     Expected number of arguments
** @param ac    argc given to the program
** @param av    argv given to the program
*/
void bad_args( char *who, int n, int ac, char *av[] );

#endif
/* SP_ASM_SRC */

#endif
