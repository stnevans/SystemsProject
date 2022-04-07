/**
** @file
** file:   filesystem.h
**
** @author CSCI-452 class of 20215
**
** author: Eric Chen
**
** description:
*/

#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include "common.h"

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

#define MAX_FILENAME 8
#define MAX_FILETYPE 3
#define MAXBLOCKS 1024
#define BLOCKSIZE 1024

/*
** Types
*/

// NOTE: Consider having one block that represents
//    a directory, FAT, and data block

typedef struct file_struct {
    char* filename;
    char* filetype;
    uint32_t pos;
    uint32_t size;

} file_t

typedef struct super_block {
    


} superblock_t

/*
** Globals
*/

/*
** Prototypes
*/
file_t create_file(char* name, char* type, uint32_t size);

void delete_file(file_t del_file);

void file_open(file_t new_file, char* mode);

void file_close(file_t new_file);

void create_dir();

void rm_dir();

#endif
/* SP_ASM_SRC */

#endif
