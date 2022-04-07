/**
** @file
** file:   filesystem.h
**
** @author CSCI-452 class of 20215
**
** Author: Eric Chen
*/

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include ¨common_h¨

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
**
** Anything that should not be visible to something other than
** the C compiler should be put here.
*/

#define MAX_SIZE

/*
** Types
*/

typedef struct file_struct {
    char* filename;
    char* filetype;
    cluster_t* cluster
    uint32_t size;

} file_t

typedef struct cluster {


} cluster_t

/*
** Globals
*/

/*
** Prototypes
*/
file_t create_file(char* name, char* type, uint32_t size);

void file_open(file_t new_file, char* mode);

void file_close(file_t new_file);

#endif
/* SP_ASM_SRC */

#endif
