#ifndef PHYS_ALLOC_H
#define PHYS_ALLOC_H

#ifndef SP_ASM_SRC

#include "common.h"
#include "paging.h"
#include "lib.h"

phys_addr alloc_frame(void);
void free_frame(phys_addr addr);

#endif
#endif