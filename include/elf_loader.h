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

#define EI_NIDENT (16)

typedef struct {
	uint8_t		e_ident[EI_NIDENT];
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

typedef struct {
	Elf32_Word		p_type;
	Elf32_Off		p_offset;
	Elf32_Addr		p_vaddr;
	Elf32_Addr		p_paddr;
	Elf32_Word		p_filesz;
	Elf32_Word		p_memsz;
	Elf32_Word		p_flags;
	Elf32_Word		p_align;
} Elf32_Phdr;

#define	PT_NULL		0		/* Program header table entry unused */
#define PT_LOAD		1		/* Loadable program segment */
#define PT_DYNAMIC	2		/* Dynamic linking information */
#define PT_INTERP	3		/* Program interpreter */
#define PT_NOTE		4		/* Auxiliary information */
#define PT_SHLIB	5		/* Reserved */
#define PT_PHDR		6		/* Entry for header table itself */
#define PT_TLS		7		/* Thread-local storage segment */
#define	PT_NUM		8		/* Number of defined types */

uint32_t elf_load_program(uint32_t address);

#endif
