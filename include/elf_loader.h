/**
** @file elf.h
**
** @author CSCI-452 class of 20215
**
** Description: Definitions for elf loading.
*/

#ifndef ELF_H_
#define ELF_H_

#include "common.h"

typedef uint16_t Elf32_Half;	// Unsigned half int
typedef uint32_t Elf32_Off;	// Unsigned offset
typedef uint32_t Elf32_Addr;	// Unsigned address
typedef uint32_t Elf32_Word;	// Unsigned int
typedef int32_t  Elf32_Sword;	// Signed int

typedef struct {
	uint8_t		e_ident[ELF_NIDENT];
	Elf32_Half	e_type;
	Elf32_Half	e_machine;
	Elf32_Word	e_version;
	Elf32_Addr	e_entry;
	Elf32_Off	e_phoff;
	Elf32_Off	e_shoff;
	Elf32_Word	e_flags;
	Elf32_Half	e_ehsize;
	Elf32_Half	e_phentsize;
	Elf32_Half	e_phnum;
	Elf32_Half	e_shentsize;
	Elf32_Half	e_shnum;
	Elf32_Half	e_shstrndx;
} Elf32_Ehdr;

enum Elf_Ident {
	EI_MAG0		  = 0, // 0x7F
	EI_MAG1		  = 1, // 'E'
	EI_MAG2		  = 2, // 'L'
	EI_MAG3		  = 3, // 'F'
	EI_CLASS	  = 4, // Architecture (32/64)
	EI_DATA		  = 5, // Byte Order
	EI_VERSION	  = 6, // ELF Version
	EI_OSABI	  = 7, // OS Specific
	EI_ABIVERSION = 8, // OS Specific
	EI_PAD	 	  = 9  // Padding
};
 
# define ELFMAG0	0x7F // e_ident[EI_MAG0]
# define ELFMAG1	'E'  // e_ident[EI_MAG1]
# define ELFMAG2	'L'  // e_ident[EI_MAG2]
# define ELFMAG3	'F'  // e_ident[EI_MAG3]
 
# define ELFDATA2LSB (1)  // Little Endian
# define ELFCLASS32	 (1)  // 32-bit Architecture

uint32_t load_program(uint32_t address);

#endif
