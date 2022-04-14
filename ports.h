/**
** @file ports.h
** 
** @author CSCI-452 class of 20215
**
** author: Jacob Doll & Eric Chen
**
** description:
*/

#ifndef _PORTS_H_
#define _PORTS_H_

#include <stdint.h>

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/

static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

static inline void outb(uint16_t port, uint8_t data) {
    asm volatile("out %%al, %%dx" : : "a" (data), "d" (port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t result;
    asm volatile("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

static inline void outw(uint16_t port, uint16_t data) {
    asm volatile("out %%ax, %%dx" : : "a" (data), "d" (port));
}

static inline void insw(uint16_t port, uint8_t* data, uint32_t size) {
    asm volatile("rep insw" : "+D" (data), "+c" (size) : "d" (port) : "memory");
}

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
**
** Anything that should not be visible to something other than
** the C compiler should be put here.
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

#endif
/* SP_ASM_SRC */

#endif

