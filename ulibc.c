/**
** @file ulibc.c
**
** @author Numerous CSCI-452 classes
**
** C implementations of user-level library functions
*/

#include "common.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

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
** with 'User' as the priority level.
**
** @param entry The function which is the entry point of the new code
** @param args  The argument vector for the new process
**
** @returns PID of the new process, or an error code
*/
pid_t spawn( int32_t (*entry)(int,char*[]), char *args[] ) {
    pid_t pid;

    pid = fork();
    if( pid != 0 ) {
        // failure, or we are the parent
        return( pid );
    }

    // we are the child
    execp( entry, User, args );

    // uh-oh....

    char buf[256];

    pid = getpid();
    sprint( buf, "PID %d exec() of %08x failed\n", pid, (uint32_t) entry );
    cwrites( buf );

    exit( E_FAILURE );
    return( 0 );   // shut the compiler up
}

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
** @returns Only on failure, returns an error code
*/
void exec( int32_t (*entry)(int,char*[]), char *args[] ) {
    execp( entry, getprio(), args );
}

/**
** cwritech(ch) - write a single character to the console
**
** @param ch The character to write
**
** @returns The return value from calling write()
*/
int32_t cwritech( char ch ) {
   return( write(CHAN_CIO,&ch,1) );
}

/**
** cwrites(str) - write a NUL-terminated string to the console
**
** @param str The string to write
**
*/
int32_t cwrites( const char *str ) {
   int len = strlen(str);
   return( write(CHAN_CIO,str,len) );
}

/**
** cwrite(buf,size) - write a sized buffer to the console
**
** @param buf  The buffer to write
** @param size The number of bytes to write
**
** @returns The return value from calling write()
*/
int32_t cwrite( const char *buf, uint32_t size ) {
   return( write(CHAN_CIO,buf,size) );
}

/**
** swritech(ch) - write a single character to the SIO
**
** @param ch The character to write
**
** @returns The return value from calling write()
*/
int32_t swritech( char ch ) {
   return( write(CHAN_SIO,&ch,1) );
}

/**
** swrites(str) - write a NUL-terminated string to the SIO
**
** @param str The string to write
**
** @returns The return value from calling write()
*/
int32_t swrites( const char *str ) {
   int len = strlen(str);
   return( write(CHAN_SIO,str,len) );
}

/**
** swrite(buf,size) - write a sized buffer to the SIO
**
** @param buf  The buffer to write
** @param size The number of bytes to write
**
** @returns The return value from calling write()
*/
int32_t swrite( const char *buf, uint32_t size ) {
   return( write(CHAN_SIO,buf,size) );
}

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
int str2int( register const char *str, register int base ) {
    register int num = 0;
    register char bchar = '9';
    int sign = 1;

    // check for leading '-'
    if( *str == '-' ) {
        sign = -1;
        ++str;
    }

    if( base != 10 ) {
        bchar = '0' + base - 1;
    }

    // iterate through the characters
    while( *str ) {
        if( *str < '0' || *str > bchar )
            break;
        num = num * base + *str - '0';
        ++str;
    }

    // return the converted value
    return( num * sign );
}

/**
** strlen(str) - return length of a NUL-terminated string
**
** @param str The string to examine
**
** @return The length of the string, or 0
*/
uint32_t strlen( register const char *str ) {
    register uint32_t len = 0;

    while( *str++ ) {
        ++len;
    }

    return( len );
}

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
char *strcpy( register char *dst, register const char *src ) {
    register char *tmp = dst;

    while( (*dst++ = *src++) )
        ;

    return( tmp );
}

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
char *strcat( register char *dst, register const char *src ) {
    register char *tmp = dst;

    while( *dst )  // find the NUL
        ++dst;

    while( (*dst++ = *src++) )  // append the src string
        ;

    return( tmp );
}

/**
** strcmp(s1,s2) - compare two NUL-terminated strings
**
** @param s1 The first source string
** @param s2 The second source string
**
** @return negative if s1 < s2, zero if equal, and positive if s1 > s2
*/
int strcmp( register const char *s1, register const char *s2 ) {

    while( *s1 != 0 && (*s1 == *s2) )
        ++s1, ++s2;

    return( *(const unsigned char *)s1 - *(const unsigned char *)s2 );
}


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
char *pad( char *dst, int extra, int padchar ) {
    while( extra > 0 ){
        *dst++ = (char) padchar;
        extra -= 1;
    }
    return dst;
}

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
                   int leftadjust, int padchar ) {
    int    extra;

    // determine the length of the string if we need to
    if( len < 0 ){
        len = strlen( str );
    }

    // how much filler must we add?
    extra = width - len;

    // add filler on the left if we're not left-justifying
    if( extra > 0 && !leftadjust ){
        dst = pad( dst, extra, padchar );
    }

    // copy the string itself
    for( int i = 0; i < len; ++i ) {
        *dst++ = str[i];
    }

    // add filler on the right if we are left-justifying
    if( extra > 0 && leftadjust ){
        dst = pad( dst, extra, padchar );
    }

    return dst;
}

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
void sprint( char *dst, char *fmt, ... ) {
    int32_t *ap;
    char buf[ 12 ];
    char ch;
    char *str;
    int leftadjust;
    int width;
    int len;
    int padchar;

    /*
    ** Get characters from the format string and process them
    **
    ** We use the "old-school" method of handling variable numbers
    ** of parameters.  We assume that parameters are passed on the
    ** runtime stack in consecutive longwords; thus, if the first
    ** parameter is at location 'x', the second is at 'x+4', the
    ** third at 'x+8', etc.  We use a pointer to a 32-bit thing
    ** to point to the next "thing", and interpret it according
    ** to the format string.
    */
    
    // get the pointer to the first "value" parameter
    ap = (int *)(&fmt) + 1;

    // iterate through the format string
    while( (ch = *fmt++) != '\0' ){
        /*
        ** Is it the start of a format code?
        */
        if( ch == '%' ){
            /*
            ** Yes, get the padding and width options (if there).
            ** Alignment must come at the beginning, then fill,
            ** then width.
            */
            leftadjust = 0;
            padchar = ' ';
            width = 0;
            ch = *fmt++;
            if( ch == '-' ){
                leftadjust = 1;
                ch = *fmt++;
            }
            if( ch == '0' ){
                padchar = '0';
                ch = *fmt++;
            }
            while( ch >= '0' && ch <= '9' ){
                width *= 10;
                width += ch - '0';
                ch = *fmt++;
            }

            /*
            ** What data type do we have?
            */
            switch( ch ) {

            case 'c':  // characters are passed as 32-bit values
                ch = *ap++;
                buf[ 0 ] = ch;
                buf[ 1 ] = '\0';
                dst = padstr( dst, buf, 1, width, leftadjust, padchar );
                break;

            case 'd':
                len = cvt_dec( buf, *ap++ );
                dst = padstr( dst, buf, len, width, leftadjust, padchar );
                break;

            case 's':
                str = (char *) (*ap++);
                dst = padstr( dst, str, -1, width, leftadjust, padchar );
                break;

            case 'x':
                len = cvt_hex( buf, *ap++ ) + 2;
                dst = padstr( dst, buf, len, width, leftadjust, padchar );
                break;

            case 'o':
                len = cvt_oct( buf, *ap++ );
                dst = padstr( dst, buf, len, width, leftadjust, padchar );
                break;

            }
        } else {
            // no, it's just an ordinary character
            *dst++ = ch;
        }
    }

    // NUL-terminate the result
    *dst = '\0';
}

/*
**********************************************
** MISCELLANEOUS USEFUL SUPPORT FUNCTIONS
**********************************************
*/

/**
** cvt_dec0(buf,value) - local support routine for cvt_dec()
**
** convert a 32-bit unsigned integer into a NUL-terminated character string
**
** @param buf    Destination buffer
** @param value  Value to convert
**
** @return The number of characters placed into the buffer
**          (not including the NUL)
**
** NOTE:  assumes buf is large enough to hold the resulting string
*/
char *cvt_dec0( char *buf, int value ) {
    int quotient;

    quotient = value / 10;
    if( quotient < 0 ) {
        quotient = 214748364;
        value = 8;
    }
    if( quotient != 0 ) {
        buf = cvt_dec0( buf, quotient );
    }
    *buf++ = value % 10 + '0';
    return buf;
}

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
int cvt_dec( char *buf, int32_t value ) {
    char *bp = buf;

    if( value < 0 ) {
        *bp++ = '-';
        value = -value;
    }

    bp = cvt_dec0( bp, value );
    *bp  = '\0';

    return( bp - buf );
}

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
int cvt_hex( char *buf, uint32_t value ) {
    char hexdigits[] = "0123456789ABCDEF";
    int chars_stored = 0;

    for( int i = 0; i < 8; i += 1 ) {
        int val = value & 0xf0000000;
        if( chars_stored || val != 0 || i == 7 ) {
            ++chars_stored;
            val = (val >> 28) & 0xf;
            *buf++ = hexdigits[val];
        }
        value <<= 4;
    }

    *buf = '\0';

    return( chars_stored );
}

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
int cvt_oct( char *buf, uint32_t value ){
        int     i;
        int     chars_stored = 0;
        char    *bp = buf;
        int     val;

        val = ( value & 0xc0000000 );
        val >>= 30;
        for( i = 0; i < 11; i += 1 ){

                if( i == 10 || val != 0 || chars_stored ){
                        chars_stored = 1;
                        val &= 0x7;
                        *bp++ = val + '0';
                }
                value <<= 3;
                val = ( value & 0xe0000000 );
                val >>= 29;
        }
        *bp = '\0';

        return bp - buf;
}

/**
** report(ch,pid)
**
** Report to the console that user 'ch' is running as 'pid'
**
** @param ch   The one-character name of the user process
** @param whom The PID of the process
*/
void report( char ch, pid_t whom ) {
    char buf[20];  // up to " x(nnnnnnnnnnnnnnn)"

    buf[0] = ' ';
    buf[1] = ch;
    buf[2] = '(';
    int i = cvt_dec( buf+3, (int) whom );
    buf[3+i] = ')';
    buf[3+i+1] = '\0';
    cwrites( buf );       // one syscall vs. five
}

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
void bad_args( char *who, int n, int ac, char *av[] ) {
    char buf[512];
    char *ptr;

    // initial message
    sprint( buf, "\n!! %s expected %d got %d args", who ? who : "(?)", ac );

    // check for NULL arg vector
    if( av == NULL ) {
        strcat( buf, " (NULL argv)\n" );
        cwrites( buf );
        return;
    }

    // find the NUL
    ptr = buf + strlen( buf );

    // add the arg strings
    for( int i = 0; i < ac; ++i ) {
        if( av[i] != NULL ) {
            sprint( ptr, " '%s'", av[i] );
            ptr += strlen( ptr );
        } else {
            strcpy( ptr, " NULL" );
            ptr += 5;
        }
    }

    // add a newline
    strcat( ptr, "\n" );

    cwrites( buf );       // one syscall vs. five
}
