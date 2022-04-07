/**
** @file 
** file: disk.h
**
** @author CSCI-452 class of 20215
**
** author: Eric Chen
**
** description:
*/

#ifndef DISK_H_
#define DISK_H_

#include "common_h"

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

#define MAX_CLUSTER 512

/*
** Types
*/

typedef struct disk_format {
    uint32_t blocksize;


} disk_t;

/*
** Globals
*/

/*
** Prototypes
*/

#endif
/* SP_ASM_SRC */

#endif
