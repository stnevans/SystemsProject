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

static int elf_load_segment(uint32_t addr, uint32_t offset, uint32_t vaddr, uint32_t size, uint32_t align) {
    uint32_t paddr = addr + offset;
    uint32_t num_pages = (size / align) + 1; 
    char buf[128];

    swrites("elf_load_segment\n");
    if (!vaddr) return 0;

    while (num_pages) {
        sprint(buf, "Mapping Region: %x: %x\n", vaddr, paddr);
        swrites(buf);

        map_virt_page_to_phys(vaddr, paddr);

        paddr += align;
        vaddr += addr;
        num_pages -= 1;
    }

    return 1;
}

static int elf_read_phdrs(uint32_t addr, uint32_t phoff, uint16_t phentsize, uint16_t phnum) {
    uint32_t phaddr = addr + phoff;
    Elf32_Phdr *curr;

    swrites("elf_read_phdrs\n");


    while (phnum) {
        curr = (Elf32_Phdr *)(phaddr);
        
        if (curr->p_type == PT_LOAD) {
            elf_load_segment(addr, curr->p_offset, curr->p_vaddr, curr->p_memsz, curr->p_align);
        }

        phaddr += phentsize;
        phnum -= 1;
    }
    
    return 1;
}

static int elf_verify(Elf32_Ehdr *hdr) {
    if (!hdr) return 0;
    if (hdr->e_ident[EI_MAG0] != ELFMAG0) return 0;
    if (hdr->e_ident[EI_MAG1] != ELFMAG1) return 0;
    if (hdr->e_ident[EI_MAG2] != ELFMAG2) return 0;
    if (hdr->e_ident[EI_MAG3] != ELFMAG3) return 0;
    return 1;
}

uint32_t elf_load_program(uint32_t addr) {
    Elf32_Ehdr *hdr = (Elf32_Ehdr*) addr;

    if (!elf_verify(hdr)) {
        cwrites( "ELF: invalid ELF header!\n" );
        return 0;
    }

    uint32_t entry = hdr->e_entry;
    if (!elf_read_phdrs(addr, hdr->e_phoff, hdr->e_phentsize, hdr->e_phnum)) {
        cwrites( "ELF: Error reading program headers!\n" );
        return 0;
    }

    return 0;
}
