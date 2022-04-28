#define SP_KERNEL_SRC

#include "paging.h"
#include "lib.h"
#include "phys_alloc.h"
#include "x86arch.h"

uint8_t paging_init;


struct page_table * alloc_pg_tbl(){
    struct page_table * pg_tbl = (struct page_table *) alloc_frame();
    __memset(pg_tbl, sizeof(pg_tbl->entry), 0);

    return pg_tbl;
}

void free_pg_tbl(struct page_table * tbl){
    free_frame((phys_addr) tbl);
}

struct page_directory * alloc_pg_dir(){
    struct page_directory * pg_dir = (struct page_directory *) alloc_frame();
    __memset(pg_dir, sizeof(*pg_dir), 0);
    return pg_dir;
}

void free_pg_dir(struct page_directory * dir){
    free_frame((phys_addr) dir);
}

void pte_set_attr(pte_t * pte, uint32_t attr) {
    *pte |= attr;
}

void pte_del_attr(pte_t * pte, uint32_t attr) {
    *pte &= ~(attr);
}

void pte_set_frame(pte_t * pte, phys_addr phys) {
    *pte &= (~I86_PTE_FRAME);
    *pte |= I86_PTE_FRAME & phys;
}

phys_addr pte_get_frame(pte_t * pte) {
    return *pte & I86_PTE_FRAME;
}

void pde_set_attr(pde_t * pde, uint32_t attr) {
    *pde |= attr;
}

void pde_del_attr(pde_t * pde, uint32_t attr) {
    *pde &= (~attr);
}

void pde_set_frame(pde_t * pde, phys_addr phys) {
    *pde &= (~I86_PDE_FRAME);
    *pde |= I86_PDE_FRAME & phys;
}

phys_addr pde_get_frame(pde_t * pde){
    return *pde & I86_PDE_FRAME;
}

// Allocate a frame, set the pte to use the frame, mark as present
char alloc_page(pte_t * pte) {
    phys_addr addr = alloc_frame();
    if(!addr){
        return false;
    }
    pte_set_frame(pte, addr);
    pte_set_attr(pte, I86_PTE_PRESENT);

    return true;
}

// Free the backing frame and mark the page not present
void free_page(pte_t * pte) {
    phys_addr addr = pte_get_frame(pte);
    if(addr){
        free_frame(addr);
    }
    pte_del_attr(pte, I86_PTE_PRESENT);
}

pte_t * find_pte_entry(struct page_table * pg_tbl, virt_addr addr){
    if(!addr){
        return 0;
    }
    return &pg_tbl->entry[PAGE_TABLE_INDEX(addr)];
}

pde_t * find_pde_entry(struct page_directory * pg_dir, virt_addr addr){
    if(!addr){
        return 0;
    }   
    return &pg_dir->entry[PAGE_DIRECTORY_INDEX(addr)];
}

struct page_directory * current_pg_dir;
struct page_directory * kernel_pg_dir;
struct page_directory * get_current_pg_dir(){
    return current_pg_dir;
}

struct page_directory * get_kernel_pg_dir(){
    return kernel_pg_dir;
}

void set_page_directory(struct page_directory * pg_dir){
    current_pg_dir = pg_dir;
    // set cr3 to page directory
    __asm__ volatile("mov %0, %%cr3":: "r"(&pg_dir->entry));
    // Toggle paging bit in c30
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; 
    __asm__ volatile("mov %0, %%cr0":: "r"(cr0));
}

void map_virt_page_to_phys(virt_addr virt, phys_addr phys){
    struct page_directory * pg_dir = current_pg_dir;
    pde_t * pd_entry = &pg_dir->entry[PAGE_DIRECTORY_INDEX(virt)];
    
    // Check if pde is present already. If not, we alloc a new frame for one.
    if((*pd_entry & I86_PDE_PRESENT) != I86_PDE_PRESENT){
        // make a frame. use that as our new page table
        struct page_table * new_table = alloc_pg_tbl();
        
        pde_set_attr(pd_entry, I86_PDE_PRESENT);
        pde_set_attr(pd_entry, I86_PDE_WRITABLE);
        pde_set_frame(pd_entry, (phys_addr) new_table);
    }
    // We now have a present pde. So we just need to set the relevant pte bits.
    struct page_table * tbl = (struct page_table *) PAGE_GET_PHYSICAL_ADDRESS(pd_entry);
    // let's assume we have idenitiy mapping at the bottom
    pte_t * pt_entry = &tbl->entry[PAGE_TABLE_INDEX(virt)];

    //Set the frame and mark present
    pte_set_frame(pt_entry, phys);
    pte_set_attr(pt_entry, I86_PTE_PRESENT);
    pte_set_attr(pt_entry, I86_PTE_WRITABLE);
}
void map_virt_page_to_phys_pg_dir(struct page_directory * pg_dir, virt_addr virt, phys_addr phys){
    pde_t * pd_entry = &pg_dir->entry[PAGE_DIRECTORY_INDEX(virt)];
    
    // Check if pde is present already. If not, we alloc a new frame for one.
    if((*pd_entry & I86_PDE_PRESENT) != I86_PDE_PRESENT){
        // make a frame. use that as our new page table
        struct page_table * new_table = alloc_pg_tbl();
        
        pde_set_attr(pd_entry, I86_PDE_PRESENT);
        pde_set_attr(pd_entry, I86_PDE_WRITABLE);
        pde_set_frame(pd_entry, (phys_addr) new_table);
    }
    // We now have a present pde. So we just need to set the relevant pte bits.
    struct page_table * tbl = (struct page_table *) PAGE_GET_PHYSICAL_ADDRESS(pd_entry);
    // let's assume we have idenitiy mapping at the bottom
    pte_t * pt_entry = &tbl->entry[PAGE_TABLE_INDEX(virt)];

    //Set the frame and mark present
    pte_set_frame(pt_entry, phys);
    pte_set_attr(pt_entry, I86_PTE_PRESENT);
    pte_set_attr(pt_entry, I86_PTE_WRITABLE);
}


void unmap_virt(struct page_directory * pg_dir, virt_addr virt){
    pde_t * pd_entry = &pg_dir->entry[PAGE_DIRECTORY_INDEX(virt)];
    
    // We now have a present pde. So we just need to set the relevant pte bits.
    struct page_table * tbl = (struct page_table *) PAGE_GET_PHYSICAL_ADDRESS(pd_entry);
    if(tbl){
    // let's assume we have idenitiy mapping at the bottom
        pte_t * pt_entry = &tbl->entry[PAGE_TABLE_INDEX(virt)];
        *pt_entry = 0;
    }
}

#define KERNEL_START 0

struct page_directory * get_base_pg_dir(){
    struct page_table * ident_tbl = alloc_pg_tbl();
    // Identity map the first bit (0 - 100*1024)
    for(int i = 0, addr = 0; i < 600; i++, addr+= 4096){
        pte_t pte = 0;

        pte_set_attr(&pte, I86_PTE_PRESENT);
        pte_set_attr(&pte, I86_PTE_WRITABLE);

        pte_set_frame(&pte, addr);

        ident_tbl->entry[PAGE_TABLE_INDEX(addr)] = pte;
    }
    virt_addr virt = 0xc0000000;
    phys_addr phys = KERNEL_START;
    struct page_table * highmem_tbl = alloc_pg_tbl();
    for(int i =0; i < 1024; i++, virt+=4096, phys+=4096){
        pte_t pte = 0;

        pte_set_attr(&pte, I86_PTE_PRESENT);
        pte_set_attr(&pte, I86_PTE_WRITABLE);

        pte_set_frame(&pte, phys);

        highmem_tbl->entry[PAGE_TABLE_INDEX(virt)] = pte;
    }

    struct page_directory * pg_dir = alloc_pg_dir();
    pde_t * ident_de = &pg_dir->entry[PAGE_DIRECTORY_INDEX(0x0)];
    pde_set_attr(ident_de, I86_PDE_PRESENT);
    pde_set_attr(ident_de, I86_PDE_WRITABLE);
    pde_set_frame(ident_de, (phys_addr) ident_tbl);

    pde_t * highmem_de = &pg_dir->entry[PAGE_DIRECTORY_INDEX(0xc0000000)];
    pde_set_attr(highmem_de, I86_PDE_PRESENT);
    pde_set_attr(highmem_de, I86_PDE_WRITABLE);
    pde_set_frame(highmem_de, (phys_addr) highmem_tbl);

    return pg_dir;
}

struct page_directory * copy_pg_dir(struct page_directory * pg_dir){
    struct page_directory * pg_cpy = alloc_pg_dir();
    for(int i = 0; i < 1024; i++){
        pde_t * old_dir_entry = &pg_dir->entry[i];
        if(*old_dir_entry){
            struct page_table * old_pg_tbl = (struct page_table *) pde_get_frame(old_dir_entry);
            struct page_table * new_pg_tbl = alloc_pg_tbl();
            for(int j = 0; j < 1024; j++){
                new_pg_tbl->entry[j] = old_pg_tbl->entry[j];
            }
            pde_t * new_dir_entry = &pg_cpy->entry[i];
            *new_dir_entry = *old_dir_entry;
            pde_set_frame(new_dir_entry, (phys_addr) new_pg_tbl);
        }
    }
    return pg_cpy;
}

// This just clears the page tables of a page directory.
// Something else is repsonsible for freeing the backing frames for each pte
void delete_pg_dir(struct page_directory * pg_dir){
    for(int i = 0; i < 1024; i++){
        pde_t * old_dir_entry = &pg_dir->entry[i];
        if(*old_dir_entry){
            free_pg_tbl((struct page_table * )pde_get_frame(old_dir_entry));
        }
    }
    free_pg_dir(pg_dir);
}


// Return true if a page was allocated at the given virtual address.
// Return false if a page was already there or it failed.
bool_t alloc_page_at(struct page_directory * pg_dir, virt_addr virt){
    pde_t * pd_entry = &pg_dir->entry[PAGE_DIRECTORY_INDEX(virt)];
    
    // Check if pde is present already. If not, we alloc a new frame for one.
    if((*pd_entry & I86_PDE_PRESENT) != I86_PDE_PRESENT){
        // make a frame. use that as our new page table
        struct page_table * new_table = alloc_pg_tbl();
        
        pde_set_attr(pd_entry, I86_PDE_PRESENT);
        pde_set_attr(pd_entry, I86_PDE_WRITABLE);
        pde_set_frame(pd_entry, (phys_addr) new_table);
    }
    // We now have a present pde. So we just need to set the relevant pte bits.
    struct page_table * tbl = (struct page_table *) PAGE_GET_PHYSICAL_ADDRESS(pd_entry);
    

    // let's assume we have idenitiy mapping at the bottom
    pte_t * pt_entry = &tbl->entry[PAGE_TABLE_INDEX(virt)];
    if(*pt_entry){
        return false;
    }
    phys_addr frame = alloc_frame();
    if(!frame){
        return false;
    }
    map_virt_page_to_phys_pg_dir(pg_dir, virt, frame);
    return true;
}

void free_frame_at(struct page_directory * pg_dir, virt_addr virt){
    pde_t * pd_entry = &pg_dir->entry[PAGE_DIRECTORY_INDEX(virt)];
    if(!(*pd_entry)){
        return;
    }
    if(pde_get_frame(pd_entry) == 0){
        return;
    }
    struct page_table * tbl = (struct page_table *) PAGE_GET_PHYSICAL_ADDRESS(pd_entry);
    

    // let's assume we have idenitiy mapping at the bottom
    pte_t * pt_entry = &tbl->entry[PAGE_TABLE_INDEX(virt)];
    phys_addr frame = pte_get_frame(pt_entry);
    *pt_entry = 0;
    free_frame(frame);    
}

static void _page_fault_isr( int vector, int code ) {
    uint32_t cr2;
    __asm__ __volatile__ (
        "mov %%cr2, %%eax\n\t"
        "mov %%eax, %0\n\t"
    : "=m"(cr2)
    :
    : "%eax");
    __cio_printf("We got a page fault at %x\n", cr2);
}

void _paging_init() {
    __cio_puts( " Paging:" );

    struct page_directory * pg_dir = get_base_pg_dir();
    kernel_pg_dir = pg_dir;
    set_page_directory(pg_dir);

    paging_init = 1;
    __install_isr( INT_VEC_PAGE_FAULT, _page_fault_isr );

    __cio_puts( " done" );
}

uint8_t is_paging_init(){
    return paging_init;
}