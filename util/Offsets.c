/*
** File:    Offsets.c
**
** Author:      Warren R. Carithers
**
** Description:     Print byte offsets for fields in various structures.
**
** This program exists to simplify life.  If/when fields in a structure are
** changed, this can be modified, recompiled and executed to come up with
** byte offsets for use in accessing structure fields from assembly language.
**
** IMPORTANT NOTE:  compiling this on a 64-bit architecture will yield
** incorrect results by default, as 64-bit GCC versions most often use
** the LP64 model (longs and pointers are 64 bits).  Add the "-mx32"
** option to the compiler (compile for x86_64, but use 32-bit sizes),
** and make sure you have the 'libc6-dev-i386' package installed (for
** Ubuntu systems).
**
** If invoked with the -h option, generates a header file named offsets.h
** which contains CPP macros for type sizes and the offsets into pcb_t and
** context_t; otherwise, prints the same information to stdout.
*/

#define SP_KERNEL_SRC

#include "common.h"

// avoid complaints about stdio.h
#undef NULL

#include "process.h"
#include "stacks.h"
#include "queues.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/*
** Header comment prefix
*/
char h_prefix[] = "/**\n"
"** @file offsets.h\n"
"**\n"
"** GENERATED AUTOMATICALLY - DO NOT EDIT\n"
"**\n"
"** This header file contains C Preprocessor macros which expand\n"
"** into the byte offsets needed to reach fields within structs\n"
"** used in the baseline system.  Should those struct declarations\n"
"** change, the Offsets program should be modified (if needed),\n"
"** recompiled, and re-run to recreate this file.\n"
"*/\n"
"\n"
"#ifndef OFFSETS_H_\n"
"#define OFFSETS_H_\n";

char h_suffix[] = "\n"
"#endif\n";

// are we generating the .h file?
int genheader = 0;

// macro name prefix to use
char *sname = "???";

// header file stream
FILE *hfile;

// prefix for header file lines

// produce a report line
void process( char *field, size_t bytes ) {
    if( genheader ) {
        char name[64];
        sprintf( name, "%s_%s", sname, field );
        fprintf( hfile, "#define\t%-23s\t%u\n", name, bytes );
    } else {
        printf( "  %-10s  %u\n", field, bytes );
    }
}

void setheader( void ) {
    // trigger output into the header file
    genheader = 1;

    hfile = fopen( "offsets.h", "w" );
    if( hfile == NULL ) {
        perror( "offsets.h" );
        exit( 1 );
    }

    fputs( h_prefix, hfile );
}

void hsection( char *name, char *typename, size_t size ) {
    if( genheader ) {
        sname = name;
        fprintf( hfile, "\n// Offsets into %s\n// Size: %u bytes\n\n",
                typename, size );
    } else {
        printf( "Offsets into %s (%u bytes):\n", typename, size );
    }
}

void tsection( char *name, char *typename ) {
    if( genheader ) {
        sname = name;
        fprintf( hfile, "\n// Sizes of %s types\n\n", typename );
    } else {
        printf( "Sizes of %s types:\n", typename );
    }
}

int main( int argc, char *argv[] ) {

    if( argc > 1 && strcmp(argv[1],"-h") == 0 ) {
        setheader();
    }

    tsection( "SZ", "basic" );
    process( "char", sizeof(char) );
    process( "short", sizeof(short) );
    process( "int", sizeof(int) );
    process( "long", sizeof(long) );
    process( "long_long", sizeof(long long) );
    fputc( '\n', genheader ? hfile : stdout );

    tsection( "SZ", "our" );
    process( "int8_t", sizeof(int8_t) );
    process( "int16_t", sizeof(int16_t) );
    process( "int32_t", sizeof(int32_t) );
    process( "int64_t", sizeof(int64_t) );
    process( "uint8_t", sizeof(uint8_t) );
    process( "uint16_t", sizeof(uint16_t) );
    process( "uint32_t", sizeof(uint32_t) );
    process( "uint64_t", sizeof(uint64_t) );
    process( "uint_t", sizeof(uint_t) );
    process( "bool_t", sizeof(bool_t) );
    process( "pid_t", sizeof(pid_t) );
    process( "state_t", sizeof(state_t) );
    process( "time_t", sizeof(time_t) );
    process( "status_t", sizeof(status_t) );
    process( "prio_t", sizeof(prio_t) );
    process( "queue_t", sizeof(queue_t) );
    process( "key_t", sizeof(key_t) );
    process( "stack_t", sizeof(stack_t) );
    fputc( '\n', genheader ? hfile : stdout );

    hsection( "CTX", "context_t", sizeof(context_t) );
    process( "ss", offsetof(context_t,ss) );
    process( "gs", offsetof(context_t,gs) );
    process( "fs", offsetof(context_t,fs) );
    process( "es", offsetof(context_t,es) );
    process( "ds", offsetof(context_t,ds) );
    process( "edi", offsetof(context_t,edi) );
    process( "esi", offsetof(context_t,esi) );
    process( "ebp", offsetof(context_t,ebp) );
    process( "esp", offsetof(context_t,esp) );
    process( "ebx", offsetof(context_t,ebx) );
    process( "edx", offsetof(context_t,edx) );
    process( "ecx", offsetof(context_t,ecx) );
    process( "eax", offsetof(context_t,eax) );
    process( "vector", offsetof(context_t,vector) );
    process( "code", offsetof(context_t,code) );
    process( "eip", offsetof(context_t,eip) );
    process( "cs", offsetof(context_t,cs) );
    process( "eflags", offsetof(context_t,eflags) );
    fputc( '\n', genheader ? hfile : stdout );

    hsection( "PCB", "pcb_t", sizeof(pcb_t) );
    process( "context", offsetof(pcb_t,context) );
    process( "stack", offsetof(pcb_t,stack) );
    process( "wakeup", offsetof(pcb_t,wakeup) );
    process( "exit_status", offsetof(pcb_t,exit_status) );
    process( "pid", offsetof(pcb_t,pid) );
    process( "ppid", offsetof(pcb_t,ppid) );
    process( "state", offsetof(pcb_t,state) );
    process( "priority", offsetof(pcb_t,priority) );
    process( "quantum",offsetof(pcb_t,quantum) );
    process( "ticks", offsetof(pcb_t,ticks) );
    process( "pg_dir", offsetof(pcb_t,pg_dir) );

    if( genheader ) {
        fputs( h_suffix, hfile );
        fclose( hfile );
    }

    return( 0 );
}
