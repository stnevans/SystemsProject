/**
** @file ports.h
** 
** @author CSCI-452 class of 20215
**
** authors: Jacob Doll & Eric Chen
**
** description: This is the file for the ports used in the ATA driver
** Note: Much of the implementation is pulled from Jacob's phoenixos project 
** with his permission
** Note 2: Most of the functions that are here are already found in lib.h
** the only exception is insw()
*/

#ifndef _PORTS_H_
#define _PORTS_H_

#include "lib.h"
#include "x86arch.h"

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/

static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

static inline void outb(uint16_t port, uint8_t data) {
    __asm__ volatile("out %%al, %%dx" : : "a" (data), "d" (port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t result;
    __asm__ volatile("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

static inline void outw(uint16_t port, uint16_t data) {
    __asm__ volatile("out %%ax, %%dx" : : "a" (data), "d" (port));
}

static inline void insw(uint16_t port, uint8_t* data, uint32_t size) {
    __asm__ volatile("rep insw" : "+D" (data), "+c" (size) : "d" (port) : "memory");
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

