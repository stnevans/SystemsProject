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

static int verify_elf(Elf32_Ehdr *hdr) {
    if (!hdr) return 0;
    if (hdr->e_ident[EI_MAG0] != ELFMAG0) return 0;
    if (hdr->e_ident[EI_MAG1] != ELFMAG1) return 0;
    if (hdr->e_ident[EI_MAG2] != ELFMAG2) return 0;
    if (hdr->e_ident[EI_MAG3] != ELFMAG3) return 0;
    return 1;
}

uint32_t load_program(uint32_t address) {
    Elf32_Ehdr *hdr = (Elf32_Ehdr*) address;

    if (!verify_elf(hdr)) {
        cwrites( "ELF: invalid ELF header!\n" );
        return 0;
    }

    

    return 0;
}
