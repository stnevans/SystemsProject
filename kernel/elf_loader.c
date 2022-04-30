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

static bool_t elf_load_segment(uint32_t addr, uint32_t offset, uint32_t vaddr, uint32_t size, uint32_t align) {
    uint32_t paddr = addr + offset;
    uint32_t num_pages = (size / align) + 1;
    uint32_t cur_vaddr = vaddr; 

    if (!vaddr) return 0;

    while (num_pages) {
        if (!alloc_page_at(get_current_pg_dir(), cur_vaddr)) {
            return false;
        }

        cur_vaddr += SZ_PAGE;
        num_pages -= 1;
    }

    __memcpy((void*)vaddr, (void*)paddr, size);

    return true;
}

static bool_t elf_read_phdrs(uint32_t addr, uint32_t phoff, uint16_t phentsize, uint16_t phnum) {
    uint32_t phaddr = addr + phoff;
    Elf32_Phdr *curr;

    while (phnum) {
        curr = (Elf32_Phdr *)(phaddr);
        
        if (curr->p_type == PT_LOAD) {
            elf_load_segment(addr, curr->p_offset, curr->p_vaddr, curr->p_memsz, curr->p_align);
        }

        phaddr += phentsize;
        phnum -= 1;
    }
    
    return true;
}

static bool_t elf_verify(Elf32_Ehdr *hdr) {
    if (!hdr) return 0;
    if (hdr->e_ident[EI_MAG0] != ELFMAG0) return false;
    if (hdr->e_ident[EI_MAG1] != ELFMAG1) return false;
    if (hdr->e_ident[EI_MAG2] != ELFMAG2) return false;
    if (hdr->e_ident[EI_MAG3] != ELFMAG3) return false;
    return true;
}

uint32_t elf_load_program(uint32_t addr) {
    Elf32_Ehdr *hdr = (Elf32_Ehdr*) addr;
    char buf[128];

    if (!elf_verify(hdr)) {
        sprint("ELF: invalid ELF header at %x!\n", addr);
        cwrites(buf);
        return 0;
    }

    uint32_t entry = hdr->e_entry;
    if (!elf_read_phdrs(addr, hdr->e_phoff, hdr->e_phentsize, hdr->e_phnum)) {
        cwrites( "ELF: Error reading program headers!\n" );
        return 0;
    }

    return entry;
}
