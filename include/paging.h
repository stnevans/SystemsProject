#ifndef PAGING_H
#define PAGING_H

#ifndef SP_ASM_SRC

#include "common.h"

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

typedef uint32_t pte_t;
typedef uint32_t pde_t;
typedef uint32_t phys_addr;
typedef uint32_t virt_addr;

struct page_table {
    pte_t entry[1024];
};

struct page_directory {
    pde_t entry[1024];
};

// http://www.brokenthorn.com/Resources/OSDev18.html
enum PAGE_PTE_FLAGS {

    I86_PTE_PRESENT = 1,           // 0000000000000000000000000000001
    I86_PTE_WRITABLE = 2,          // 0000000000000000000000000000010
    I86_PTE_USER = 4,              // 0000000000000000000000000000100
    I86_PTE_WRITETHOUGH = 8,       // 0000000000000000000000000001000
    I86_PTE_NOT_CACHEABLE = 0x10,  // 0000000000000000000000000010000
    I86_PTE_ACCESSED = 0x20,       // 0000000000000000000000000100000
    I86_PTE_DIRTY = 0x40,          // 0000000000000000000000001000000
    I86_PTE_PAT = 0x80,            // 0000000000000000000000010000000
    I86_PTE_CPU_GLOBAL = 0x100,    // 0000000000000000000000100000000
    I86_PTE_LV4_GLOBAL = 0x200,    // 0000000000000000000001000000000
    I86_PTE_FRAME = 0x7FFFF000     // 1111111111111111111000000000000
};

enum PAGE_PDE_FLAGS {

    I86_PDE_PRESENT = 1,         // 0000000000000000000000000000001
    I86_PDE_WRITABLE = 2,        // 0000000000000000000000000000010
    I86_PDE_USER = 4,            // 0000000000000000000000000000100
    I86_PDE_PWT = 8,             // 0000000000000000000000000001000
    I86_PDE_PCD = 0x10,          // 0000000000000000000000000010000
    I86_PDE_ACCESSED = 0x20,     // 0000000000000000000000000100000
    I86_PDE_DIRTY = 0x40,        // 0000000000000000000000001000000
    I86_PDE_4MB = 0x80,          // 0000000000000000000000010000000
    I86_PDE_CPU_GLOBAL = 0x100,  // 0000000000000000000000100000000
    I86_PDE_LV4_GLOBAL = 0x200,  // 0000000000000000000001000000000
    I86_PDE_FRAME = 0x7FFFF000   // 1111111111111111111000000000000
};


/**
** Name:    alloc_pg_tbl
**
** Alloc a page table
** @return a pointer to a page table or NULL if no memory is available
**
*/
struct page_table * alloc_pg_tbl(void);
/**
** Name:    free_pg_tbl
**
** Free a page table
**
** @param tbl   The table to free
*/
void free_pg_tbl(struct page_table * tbl);
/**
** Name:    alloc_pg_dir
**
** Alloc a page directory
** @return a pointer to a page directory or NULL if no memory is available
**
*/
struct page_directory * alloc_pg_dir(void);
/**
** Name:    free_pg_dir
**
** Free a page directory
**
** @param tbl   The directory to free
*/
void free_pg_dir(struct page_directory * dir);
/**
** Name:    pte_set_attr
**
** Sets an attribute of a pte
**
** @param pte   A pointer to the pte
** @param attr  The attribute to set
*/
void pte_set_attr(pte_t * pte, uint32_t attr);
/**
** Name:    pte_del_attr
**
** Deletes an attribute of a pte
**
** @param pte   A pointer to the pte
** @param attr  The attribute to del
*/
void pte_del_attr(pte_t * pte, uint32_t attr);
/**
** Name:    pte_set_frame
**
** Sets the frame for a pte
**
** @param pte   A pointer to the pte
** @param phys  The physical address of the frame
*/
void pte_set_frame(pte_t * pte, phys_addr phys);
/**
** Name:    pte_get_frame
**
** Gets the frame of a pte
**
** @return  The frame backing a pte
*/
phys_addr pte_get_frame(pte_t * pte);
/**
** Name:    pde_set_attr
**
** Sets an attribute of a pde
**
** @param pde   A pointer to the pde
** @param attr  The attribute to set
*/
void pde_set_attr(pde_t * pde, uint32_t attr);
/**
** Name:    pde_del_attr
**
** Deletes an attribute of a pde
**
** @param pde   A pointer to the pde
** @param attr  The attribute to del
*/
void pde_del_attr(pde_t * pde, uint32_t attr);
/**
** Name:    pde_set_frame
**
** Sets the frame for a pde
**
** @param pde   A pointer to the pde
** @param phys  The physical address of the frame
*/
void pde_set_frame(pde_t * pde, phys_addr phys);
/**
** Name:    pde_get_frame
**
** Gets the frame of a pde
**
** @return  The frame backing a pde
*/
phys_addr pde_get_frame(pde_t * pde);
/**
** Name:    alloc_page
**
** Allocate a frame, set the pte to use the frame, mark as present 
**
** @param pte the pte to create a frame for
**
** @return false if a frame could not allocate for the pte. True otherwise 
*/
char alloc_page(pte_t * pte);
/**
** Name:    free_page
**
**  Free the backing frame and mark the page not present
**
** @param pte the pte to free the frame for
*/
void free_page(pte_t * pte);
/**
** Name:    find_pte_entry
**
**  Find the pte entry given a page table and address
**
** @param pg_tbl the page table to search
** @param addr the virtual address
**
** @return The address of the pte corresponding to addr
*/
pte_t * find_pte_entry(struct page_table * pg_tbl, virt_addr addr);
/**
** Name:    find_pde_entry
**
**  Find the pte entry given a page table and address
**
** @param pg_dir the page directory to search
** @param addr the virtual address
**
** @return The address of the pde corresponding to addr
*/
pde_t * find_pde_entry(struct page_directory * pg_dir, virt_addr addr);
/**
** Name:    get_current_pg_dir
**
**  Get the current page directory
**
** @return A pointer to the current pg_dir
*/
struct page_directory * get_current_pg_dir(void);
/**
** Name:    get_kernel_pg_dir
**
**  Get the kernel page directory
**
** @return A pointer to the kernel pg_dir
*/
struct page_directory * get_kernel_pg_dir(void);
/**
** Name:    set_page_directory
**
**  Set cr3 to use the passed page directory
** Also enable the paging bit in cr0
**
** @param pg_dir the page directory to use
*/
void set_page_directory(struct page_directory * pg_dir);
/**
** Name:    map_virt_page_to_phys
**
**  Map a virtual page to a physical frame
**
** @param virt the wanted virtual address
** @param phys the backing frame address
*/
void map_virt_page_to_phys(virt_addr virt, phys_addr phys);
/**
** Name:    map_virt_page_to_phys_pg_dir
**
**  Map a virtual page to a physical frame
**
** @param pg_dir the page directory to apply the changes to
** @param virt the wanted virtual address
** @param phys the backing frame address
*/
void map_virt_page_to_phys_pg_dir(struct page_directory * pg_dir, virt_addr virt, phys_addr phys);
/**
** Name:    unmap_virt
**
**  Unmap a virtual address
**
** @param pg_dir the page directory to apply the changes to
** @param virt the virtual address to unmap
*/
void unmap_virt(struct page_directory * pg_dir, virt_addr virt);
/**
** Name:    get_base_pg_dir
**
**  Get a basic page directory that will be used for the kernel.
**  Identity maps the bottom 600KB.
**  Maps the bottom 1MB to 0xc0000000
**
** @return a pointer to the base page directory
*/
struct page_directory * get_base_pg_dir(void);
/**
** Name:    copy_pg_dir
**
** Copies a page directory. Each page table uses a different frame,
** but each pte uses the same frame.
**
** @param pg_dir the page directory to copy
**
** @return a pointer to a page directory that is a copy
*/
struct page_directory * copy_pg_dir(struct page_directory * pg_dir);
/**
** Name:    delete_pg_dir
**
** This just clears the page tables of a page directory.
** Something else is repsonsible for freeing the backing frames for each pte
**
** @param pg_dir the page directory to delete
*/
void delete_pg_dir(struct page_directory * pg_dir);
/**
** Name:    alloc_page_at
**
** Allocate a new page at a given virtual address.
**
** @param pg_dir the page directory to use
** @param virt the address to try to create a page at
**
** @return true if a page was allocated. false if a page was already there or it failed
*/
bool_t alloc_page_at(struct page_directory * pg_dir, virt_addr virt);
/**
** Name:    is_mapped
**
** Test if an address is already mapped
**
** @param pg_dir the page directory to use
** @param virt the address to check if mapped
**
** @return true if an address was already mapped
*/
bool_t is_mapped(struct page_directory * pg_dir, virt_addr virt);
/**
** Name:    free_frame_at
**
** Free the frame at a given virtual address. 
** Also unmaps the address
**
** @param pg_dir the page directory to use
** @param virt the address to free the frame of and unmap
*/
void free_frame_at(struct page_directory * pg_dir, virt_addr virt);
/**
** Name:    _paging_init
**
** Initializes the paging module. Turns on paging
*/
void _paging_init(void);
/**
** Name:    is_paging_init
**
** Tests if paging is initialized
**
** @return true if paging has been initialized. false if not.
*/
uint8_t is_paging_init(void);
#endif

#endif