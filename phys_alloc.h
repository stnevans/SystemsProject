#ifndef PHYS_ALLOC_H
#define PHYS_ALLOC_H

#ifndef SP_ASM_SRC

#include "common.h"
#include "paging.h"
#include "lib.h"

phys_addr alloc_frame(void);
void free_frame(phys_addr addr);
void _phys_alloc_init(phys_addr addr, uint32_t  num_frames);
#endif
#endif