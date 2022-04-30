/**
** @file ?
**
** @author CSCI-452 class of 20215
**
** ?
*/

#define	SP_KERNEL_SRC

#include "common.h"

#include "elf_loader.h"
#include "paging.h"

/**
**_elf_load_segment(addr,offset,vaddr,size)
** 
** Loads a segment of the binary into memory.
**
** @param addr      Physical address of binary in memory
** @param offset    Offset of the segment into the binary
** @param vaddr     Virtual address to map segment to
** @param size      Size of the segment
*/
static bool_t _elf_load_segment(uint32_t addr, uint32_t offset, uint32_t vaddr, uint32_t size) {
    uint32_t paddr = addr + offset;
    uint32_t num_pages = (size / SZ_PAGE) + 1;
    if(size % 4096 + vaddr % 4096 > 4096){
        num_pages += 1;
    }
    uint32_t cur_vaddr = vaddr; 

    if (!vaddr) return true;

    while (num_pages) {
        if(!is_mapped(get_current_pg_dir(), cur_vaddr)){
            if (!alloc_page_at(get_current_pg_dir(), cur_vaddr)) {
                return false;
            }
        }

        cur_vaddr += SZ_PAGE;
        num_pages -= 1;
    }

    __memcpy((void*)vaddr, (void*)paddr, size);

    return true;
}

/**
**_elf_read_phdrs(addr,phoff,phentsize,phnum)
**
** Parses the ELF program headers and each segment described into memory.
**
** @param addr      Physical address of binary in memory
** @param phoff     Offset of program header into binary
** @param phentsiz  Size of each program header table entry
** @param phnum     Number of program header entries
**
** @return True if successfully parsed program header, false if not
*/
static bool_t _elf_read_phdrs(uint32_t addr, uint32_t phoff, uint16_t phentsize, uint16_t phnum) {
    uint32_t phaddr = addr + phoff;
    Elf32_Phdr *curr;

    while (phnum) {
        curr = (Elf32_Phdr *)(phaddr);
        
        if (curr->p_type == PT_LOAD) {
            if (!_elf_load_segment(addr, curr->p_offset, curr->p_vaddr, curr->p_memsz)) {
                return false;
            }
        }

        phaddr += phentsize;
        phnum -= 1;
    }
    
    return true;
}

/**
** _elf_veryfy(hdr)
**
** Verifies that binary contains a valid ELF header.
**
** @param hdr   Pointer to header structure
**
** @return True if a valid header, false if not.
*/
static bool_t _elf_verify(Elf32_Ehdr *hdr) {
    if (!hdr) return 0;
    if (hdr->e_ident[EI_MAG0] != ELFMAG0) return false;
    if (hdr->e_ident[EI_MAG1] != ELFMAG1) return false;
    if (hdr->e_ident[EI_MAG2] != ELFMAG2) return false;
    if (hdr->e_ident[EI_MAG3] != ELFMAG3) return false;
    return true;
}

/**
** _elf_load_program(address)
**
** Loads ELF binary stored in physical memory. Returns entry point on success.
**
** @param address	Location in physical memory to read from
**
** @return The entry point of the program or zero on failure. 
*/
uint32_t _elf_load_program(uint32_t addr) {
    Elf32_Ehdr *hdr = (Elf32_Ehdr*) addr;

    if (!_elf_verify(hdr)) {
        __cio_printf("ELF: invalid ELF header at %x!\n", addr);
        return 0;
    }

    uint32_t entry = hdr->e_entry;
    if (!_elf_read_phdrs(addr, hdr->e_phoff, hdr->e_phentsize, hdr->e_phnum)) {
        __cio_printf( "ELF: Error reading program headers!\n" );
        return 0;
    }

    return entry;
}
