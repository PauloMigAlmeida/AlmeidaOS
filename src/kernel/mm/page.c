/*
 * page.c
 *
 *  Created on: 15/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/page.h"
#include "kernel/mm/init.h"
#include "kernel/mm/addressconv.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/string.h"
#include "kernel/lib/math.h"
#include "kernel/lib/bit.h"
#include "kernel/compiler/bug.h"
#include "kernel/compiler/freestanding.h"

#define PREP_BASE_ADDR(addr)        (((addr) >> 12) & (UINT64_MAX >> (sizeof(uint64_t) * CHAR_BIT - (64-36))))
#define PAGE_STD_BITS               0x03

typedef struct {
    uint64_t phys_root;     ///< Physical address of root page table (PML4T) entry
    uint64_t virt_root;     ///< Virtual address of root page table (PML4T) entry
} pagetable_t;

static pagetable_t k_root_pgt;

/*
 * Database Operations:
 *  -> Get next available free page frame or BUG_ON if none is available -> Done
 *  -> Free page frame by reference -> TBD
 *
 *  Questions: Should that be pagetable or pageframe? I guess this is a matter of nomenclature really
 *      as both of them occupy the same amount og space... darn it, this thing gets more confusing
 *      over time.
 *
 */

typedef struct {
    /* all members below deal with physical addresses */
    uintptr_t p_start_addr;
    uintptr_t p_end_addr;
    uintptr_t p_next_free_addr;
} pageframe_database;

static pageframe_database pfdb;

uint64_t paging_calc_space_needed(uint64_t bytes) {
    return clp2((bytes / PAGE_SIZE) * sizeof(pml4e_t));
}

uint64_t pageframe_alloc(void) {
    /* check if we haven't run out of page frames to allocate */
    BUG_ON(pfdb.p_next_free_addr > pfdb.p_end_addr);

    /* alloc page frame */
    uint64_t ret = pfdb.p_next_free_addr;

    /* adjust references to the next free page frame */
    pfdb.p_next_free_addr += PAGEFRAME_SIZE;

    return ret;
}

void paging_init(mem_map_region_t k_pages_struct_rg) {
    /* setup page frame database */
    pfdb.p_start_addr = k_pages_struct_rg.base_addr;
    pfdb.p_end_addr = k_pages_struct_rg.base_addr + k_pages_struct_rg.length;
    pfdb.p_next_free_addr = k_pages_struct_rg.base_addr;

    /* clean area in which page tables will eventually be stored at */
    memzero((void*) k_pages_struct_rg.base_addr, k_pages_struct_rg.length);

    /* allocate the kernel root page table that will be used across the OS */
    k_root_pgt.phys_root = pageframe_alloc();
    k_root_pgt.virt_root = va(k_root_pgt.phys_root);
}

__force_inline bool is_page_entry_empty(void *entry) {
    return *((uint64_t*) entry) == 0;
}

void page_alloc(pml4e_t *pml4_pgtable, uint64_t v_addr, uint64_t p_dest_addr) {

    /* Alloc PML4if needed */
    uint16_t pm4l_idx = extract_bit_chunk(39, 47, v_addr);
    if (is_page_entry_empty(&pml4_pgtable[pm4l_idx])) {
        uintptr_t pdp_pgtable_addr = pageframe_alloc();

        pml4e_t hh_pml4_entry = {
                .no_execute_bit = 0,
                .available_guardhole = 0,
                .pdpe_base_addr = PREP_BASE_ADDR(pdp_pgtable_addr),
                .flags = PAGE_STD_BITS
        };

        pml4_pgtable[pm4l_idx] = hh_pml4_entry;
    }

    /* Alloc PDP if needed */
    uint16_t pdp_idx = extract_bit_chunk(30, 38, v_addr);
    pdpe_t *pdp_pgtable = (pdpe_t*) va(pml4_pgtable[pm4l_idx].pdpe_base_addr << 12);
    if (is_page_entry_empty(&pdp_pgtable[pdp_idx])) {
        uintptr_t pd_pgtable_addr = pageframe_alloc();

        pdpe_t hh_pdpe_entry = {
                .no_execute_bit = 0,
                .available_guardhole = 0,
                .pde_base_addr = PREP_BASE_ADDR(pd_pgtable_addr),
                .flags = PAGE_STD_BITS
        };

        pdp_pgtable[pdp_idx] = hh_pdpe_entry;
    }

    /* Alloc PD if needed */
    int pd_idx = extract_bit_chunk(21, 29, v_addr);
    pde_t *pd_pgtable = (pde_t*) va(pdp_pgtable[pdp_idx].pde_base_addr << 12);
    if (is_page_entry_empty(&pd_pgtable[pd_idx])) {
        uintptr_t pt_pgtable_addr = pageframe_alloc();

        pde_t hh_pde_entry = {
                .no_execute_bit = 0,
                .available_guardhole = 0,
                .pte_base_addr = PREP_BASE_ADDR(pt_pgtable_addr),
                .flags = PAGE_STD_BITS
        };
        pd_pgtable[pd_idx] = hh_pde_entry;
    }

    /* Alloc PT if needed */
    uint16_t pt_idx = extract_bit_chunk(12, 20, v_addr);
    pte_t *pt_pgtable = (pte_t*) va(pd_pgtable[pd_idx].pte_base_addr << 12);
    if (is_page_entry_empty(&pt_pgtable[pt_idx])) {
        pte_t hh_pte_entry = {
                .no_execute_bit = 0,
                .available_guardhole = 0,
                .phys_pg_base_addr = PREP_BASE_ADDR(p_dest_addr),
                .flags = PAGE_STD_BITS
        };
        pt_pgtable[pt_idx] = hh_pte_entry;
    }

}

void paging_contiguous_map(uint64_t p_start_addr, uint64_t p_end_addr, uint64_t v_base_start_addr) {
    pml4e_t *pml4_table = (pml4e_t *)k_root_pgt.virt_root;

    while (p_start_addr <= p_end_addr) {
        page_alloc(pml4_table, v_base_start_addr, p_start_addr);

        p_start_addr += PAGE_SIZE;
        v_base_start_addr += PAGE_SIZE;
    }
}

void paging_reload_cr3() {
    //TODO: Figure out a way to move k_pml4_table somewhere else so this function can get a pgtable argument instead
    load_cr3(k_root_pgt.phys_root);
}
