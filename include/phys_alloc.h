#ifndef PHYS_ALLOC_H
#define PHYS_ALLOC_H

#ifndef SP_ASM_SRC

#include "common.h"
#include "paging.h"
#include "lib.h"

// Allocate us a frame. 
phys_addr alloc_frame(void);
// Free us a frame
void free_frame(phys_addr addr);
// Initialize the physical allocator. Give a address for us to store frames and a number of frames we can use
void _phys_alloc_init(phys_addr addr, uint32_t  num_frames);
#endif
#endif