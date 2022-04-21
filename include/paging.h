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



// These functions are meant to manipulate or get the basic attributes of a pte
void pte_set_attr(pte_t* pte, uint32_t attr);
void pte_del_attr(pte_t* pte, uint32_t attr);
void pte_set_frame(pte_t* pte, phys_addr phys);
phys_addr pte_get_frame(pte_t * pte);

// These functions are meant to manipulate or get the basic attributes of a pde
void pde_set_attr(pde_t* pde, uint32_t attr);
void pde_del_attr(pde_t* pde, uint32_t attr);
void pde_set_frame(pde_t* pde, phys_addr phys);
phys_addr pde_get_frame(pde_t * pde);

// Finds the pte for a given virtual address on a page table
pte_t * find_pte_entry(struct page_table * pg_tbl, virt_addr addr);
// Finds the page directory for a given virtual address on a page directory
pde_t * find_pde_entry(struct page_directory * pd_tbl, virt_addr addr);

// Functions to allocate and free page tables and page directories
struct page_table * alloc_pg_tbl(void);
void free_pg_tbl(struct page_table * tbl);
struct page_directory * alloc_pg_dir(void);
void free_pg_dir(struct page_directory * dir);


// Creates a base page directory. The directory identity maps the first 600 KB and also maps it to 00000000c0000000
struct page_directory * get_base_pg_dir(void);

// Associate a given virtual address with a physical frame. Uses the current page directory
void map_virt_page_to_phys(virt_addr virt, phys_addr phys);

// Associate a given virtual address with a physical frame.
void map_virt_page_to_phys_pg_dir(struct page_directory * pg_dir, virt_addr virt, phys_addr phys);

// Get the current page directory
struct page_directory * get_current_pg_dir(void);

// Delete a page directory. Frees all of the page tables that are part of the directory.
// Notably, this does not free the frames in the page tables.
void delete_pg_dir(struct page_directory * pg_dir);

// Copy a page directory. A new page table is created for every table within the first directory.
// This does not copy the frames in the page table. That means every piece of memory is backed by the same frame.
// Basically like vfork()
struct page_directory * copy_pg_dir(struct page_directory * pg_dir);

// Get the page directory for the kernel. This is basically the base_pg_dir, but it also has the kernel stack.
// This would be used if we made kthreads for example.
struct page_directory * get_kernel_pg_dir(void);



// Allocate a frame at a given virtual address. Returns true if a page was allocated.
// Returns false if there was already a frame or if allocation failed.
bool_t alloc_page_at(struct page_directory * pg_dir, virt_addr virt);

// Unmap a given virtual address. This does not free the frame.
void unmap_virt(struct page_directory * pg_dir, virt_addr virt);

// Free a frame at a given virtual address
void free_frame_at(struct page_directory * pg_dir, virt_addr virt);

// Set the page directory to the passed one
void set_page_directory(struct page_directory * pg_dir);


// Test if paging is init
uint8_t is_paging_init(void);

// Initialize the paging module
void _paging_init(void);

#endif

#endif