/*
 * page.c
 *
 *  Created on: 15/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/page.h"
#include "kernel/compiler/bug.h"
#include "kernel/mm/init.h"
#include "kernel/mm/pageframe.h"
#include "kernel/mm/addressconv.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/string.h"
#include "kernel/lib/math.h"
#include "kernel/lib/bit.h"


#define PREP_BASE_ADDR(addr)        (((addr) >> 12) & (UINT64_MAX >> (sizeof(uint64_t) * CHAR_BIT - (64-36))))

#define PAGE_PRESENT_BIT            (1 << 0)
#define PAGE_READ_WRITE_BIT         (1 << 1)
#define PAGE_USER_SUPERVISOR_BIT    (1 << 2)
#define PAGE_PL_WRITETHR_BIT        (1 << 3)
#define PAGE_PL_CACHEDIS_BIT        (1 << 4)
#define PAGE_ACCESSED_BIT           (1 << 5)
#define PAGE_DIRTY_BIT              (1 << 6)
#define PAGE_PAGESIZE_BIT           (1 << 7)
#define PAGE_GLOBAL_BIT             (1 << 8)

#define PAGE_STD_BITS               PAGE_PRESENT_BIT | PAGE_READ_WRITE_BIT

/* kernel space pagetable */
static pagetable_t k_root_pgt;


/*
 * this code block makes the assumption that
 * the space needed is contiguous like from 0x0 to 0x10000
 *
 * On mm/init.c I ensured that this is the case but should
 * this requirement ever change then another algorithm
 * must be considered
 */
uint64_t paging_calc_space_needed(uint64_t bytes) {
    unsigned int mb_2 = (2 * 1024 * 1024);

    unsigned int pte_tables = bytes / mb_2;
    if (bytes % mb_2 != 0) {
        pte_tables++;
    }

    unsigned int pd_tables = pte_tables / 512;
    if (pd_tables % 512 != 0) {
        pd_tables++;
    }

    unsigned int pdp_tables = pd_tables / 512;
    if (pdp_tables % 512 != 0) {
        pdp_tables++;
    }

    unsigned int pm4l_tables = 1;

    unsigned int total_tables = pm4l_tables + pdp_tables + pd_tables + pte_tables;

    return total_tables * PAGE_SIZE;
}

void paging_init(mem_map_region_t k_pages_struct_rg, mem_map_region_t k_pfdb_struct_rg) {

    /* clean area in which page tables will eventually be stored at */
    memzero((void*) k_pages_struct_rg.base_addr, k_pages_struct_rg.length);
    memzero((void*) k_pfdb_struct_rg.base_addr, k_pfdb_struct_rg.length);

    /* initialise pageframe database */
    pageframe_init(k_pages_struct_rg, k_pfdb_struct_rg);

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
    pml4e_t *pml4_table = (pml4e_t*) k_root_pgt.virt_root;

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
