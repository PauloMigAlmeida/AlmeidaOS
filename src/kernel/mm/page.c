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
#include "kernel/lib/printk.h"

#define PAGE_SHIFT                  12
#define PREP_BASE_ADDR(addr)        (((addr) >> PAGE_SHIFT) & (UINT64_MAX >> (sizeof(uint64_t) * CHAR_BIT - (64-36))))

#define PML4E(a)                    (extract_bit_chunk(39, 47, a))
#define PDPTE(a)                    (extract_bit_chunk(30, 38, a))
#define PDE(a)                      (extract_bit_chunk(21, 29, a))
#define PTE(a)                      (extract_bit_chunk(12, 20, a))

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

void paging_init(pagetable_t *pgtable, mem_map_region_t k_pages_struct_rg, mem_map_region_t k_pfdb_struct_rg) {

    /* clean area in which page tables will eventually be stored at */
    memzero((void*) k_pages_struct_rg.base_addr, k_pages_struct_rg.length);
    memzero((void*) k_pfdb_struct_rg.base_addr, k_pfdb_struct_rg.length);

    /* initialise pageframe database */
    pgtable->pfdb = pageframe_init(k_pages_struct_rg, k_pfdb_struct_rg);

    /* allocate the root page table that will be used across the OS */
    pgtable->phys_root = pageframe_alloc(&pgtable->pfdb);
    pgtable->virt_root = va(pgtable->phys_root);
}

__force_inline static bool is_page_entry_empty(void *entry) {
    return *((uint64_t*) entry) == 0;
}

void page_alloc(pagetable_t *pgtable, uint64_t v_addr, uint64_t p_dest_addr, uint16_t flags) {

    /* decompose virtual address into pagetable indexes */
    uint16_t pm4l_idx = PML4E(v_addr);
    uint16_t pdp_idx = PDPTE(v_addr);
    uint16_t pd_idx = PDE(v_addr);
    uint16_t pt_idx = PTE(v_addr);

    /* Alloc PML4if needed */
    pml4e_t *pml4_pgtable = (pml4e_t*) pgtable->virt_root;
    if (is_page_entry_empty(&pml4_pgtable[pm4l_idx])) {
        uintptr_t pdp_pgtable_addr = pageframe_alloc(&pgtable->pfdb);

        pml4e_t hh_pml4_entry = {
                .no_execute_bit = 0,
                .available_guardhole = 0,
                .pdpe_base_addr = PREP_BASE_ADDR(pdp_pgtable_addr),
                .flags = flags
        };

        pml4_pgtable[pm4l_idx] = hh_pml4_entry;
    }

    /* Alloc PDP if needed */
    pdpe_t *pdp_pgtable = (pdpe_t*) va(pml4_pgtable[pm4l_idx].pdpe_base_addr << PAGE_SHIFT);
    if (is_page_entry_empty(&pdp_pgtable[pdp_idx])) {
        uintptr_t pd_pgtable_addr = pageframe_alloc(&pgtable->pfdb);

        pdpe_t hh_pdpe_entry = {
                .no_execute_bit = 0,
                .available_guardhole = 0,
                .pde_base_addr = PREP_BASE_ADDR(pd_pgtable_addr),
                .flags = flags
        };

        pdp_pgtable[pdp_idx] = hh_pdpe_entry;
    }

    /* Alloc PD if needed */
    pde_t *pd_pgtable = (pde_t*) va(pdp_pgtable[pdp_idx].pde_base_addr << PAGE_SHIFT);
    if (is_page_entry_empty(&pd_pgtable[pd_idx])) {
        uintptr_t pt_pgtable_addr = pageframe_alloc(&pgtable->pfdb);

        pde_t hh_pde_entry = {
                .no_execute_bit = 0,
                .available_guardhole = 0,
                .pte_base_addr = PREP_BASE_ADDR(pt_pgtable_addr),
                .flags = flags
        };
        pd_pgtable[pd_idx] = hh_pde_entry;
    }

    /* Alloc PT if needed */
    pte_t *pt_pgtable = (pte_t*) va(pd_pgtable[pd_idx].pte_base_addr << PAGE_SHIFT);
    if (is_page_entry_empty(&pt_pgtable[pt_idx])) {
        pte_t hh_pte_entry = {
                .no_execute_bit = 0,
                .available_guardhole = 0,
                .phys_pg_base_addr = PREP_BASE_ADDR(p_dest_addr),
                .flags = flags
        };
        pt_pgtable[pt_idx] = hh_pte_entry;
    }

}

static bool is_pagetable_empty(const void *pgtable) {
    bool ret = true;
    const char *src = (const char*) pgtable;
    for (size_t i = 0; i < 512; i++) {
        if (*src != 0) {
            ret = false;
            break;
        }
    }
    return ret;
}

static bool page_free_resources(pagetable_t *pgtable, uint64_t pgt_phy_addr, uint64_t v_addr, int level) {

    if (level <= 0)
        return true;

    uintptr_t *pgt_virt_addr = (uintptr_t*) va(pgt_phy_addr);

    uint16_t idx = 0;
    if (level == 4)
        idx = PML4E(v_addr);
    else if (level == 3)
        idx = PDPTE(v_addr);
    else if (level == 2)
        idx = PDE(v_addr);
    else if (level == 1)
        idx = PTE(v_addr);
    else
        fatal();

    uintptr_t *page_entry = (uintptr_t*) va((pgt_phy_addr + (idx * sizeof(uint64_t))));
    uint64_t lpgt_phy_base_addr = extract_bit_chunk(12, 51, *page_entry) << PAGE_SHIFT;

    if (page_free_resources(pgtable, lpgt_phy_base_addr, v_addr, level - 1)) {
        /* zero-out entry that is about to be freed */
        memzero(page_entry, sizeof(uint64_t));

        /* delete pageframe if possible */
        if (is_pagetable_empty(pgt_virt_addr)) {
            pageframe_free(&pgtable->pfdb, pgt_phy_addr);
            return true;
        }
    }

    return false;

}

void page_free(pagetable_t *pgtable, uint64_t v_addr) {
    /* free pagetables and pageframes where possible */
    page_free_resources(pgtable, pgtable->phys_root, v_addr, 4);

    /* invalidate entry */
    invalidate_page(v_addr);
}

void paging_contiguous_map(pagetable_t *pgtable, uint64_t p_start_addr, uint64_t p_end_addr, uint64_t v_base_start_addr,
        uint16_t flags) {

    printk_fine("mapping 0x%.16llx - 0x%.16llx to 0x%.16llx - 0x%.16llx", p_start_addr, p_end_addr, v_base_start_addr,
            v_base_start_addr + (p_end_addr - p_start_addr));

    while (p_start_addr <= p_end_addr) {
        page_alloc(pgtable, v_base_start_addr, p_start_addr, flags);
        p_start_addr += PAGE_SIZE;
        v_base_start_addr += PAGE_SIZE;
    }
}

void paging_reload_cr3(pagetable_t *pgtable) {
    load_cr3(pgtable->phys_root);
}
