/*
 * page.c
 *
 *  Created on: 15/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/page.h"
#include "kernel/mm/addressconv.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/printk.h"

//0x100000 -> gonna use this just for testing the kernel migration to the higher-half memory
#define PAGE_BASE_ADDR          UNSAFE_VA(0x100000)
#define PAGE_SIZE               0x1000
#define PREP_BASE_ADDR(addr)    (((addr) >> 12) & (UINT64_MAX >> (sizeof(uint64_t) * CHAR_BIT - (64-36))))
#define PAGE_STD_BITS           0x03

static pml4e_t *pml4_table = (pml4e_t*) (PAGE_BASE_ADDR);
static pdpe_t *pdpe_table = (pdpe_t*) (PAGE_BASE_ADDR + PAGE_SIZE);
static pde_t *pde_table = (pde_t*) (PAGE_BASE_ADDR + PAGE_SIZE * 2);
static pte_t *pte_table = (pte_t*) (PAGE_BASE_ADDR + PAGE_SIZE * 3);

// Paulo....please, please, pleeeeease come up with a better name for this crap
static uint64_t pte_add_counter = 0;

extern volatile void kernel_virt_end_addr;

void paging_init(void) {
    printk_info("kernel_virt_end_addr: 0x%.16llx", &kernel_virt_end_addr);

    pml4e_t hh_pml4_entry = {
            .no_execute_bit = 0,
            .available_guardhole = 0,
            .pdpe_base_addr = PREP_BASE_ADDR(UNSAFE_PA(PAGE_BASE_ADDR + PAGE_SIZE)),
            .flags = PAGE_STD_BITS
    };

    pml4_table[256] = hh_pml4_entry;

    pdpe_t hh_pdpe_entry = {
            .no_execute_bit = 0,
            .available_guardhole = 0,
            .pde_base_addr = PREP_BASE_ADDR((hh_pml4_entry.pdpe_base_addr << 12) + PAGE_SIZE),
            .flags = PAGE_STD_BITS
    };

    pdpe_table[0] = hh_pdpe_entry;

    for (size_t i = 0; i < 5; i++) {
        pde_t hh_pde_entry = {
                .no_execute_bit = 0,
                .available_guardhole = 0,
                .pte_base_addr = PREP_BASE_ADDR((hh_pdpe_entry.pde_base_addr << 12) + PAGE_SIZE * (i + 1)),
                .flags = PAGE_STD_BITS
        };
        pde_table[i] = hh_pde_entry;
    }

    for (size_t i = 0; i < (512 * 5); i++) {
        pte_t hh_pte_entry = {
                .no_execute_bit = 0,
                .available_guardhole = 0,
                .phys_pg_base_addr = PREP_BASE_ADDR(pte_add_counter),
                .flags = PAGE_STD_BITS
        };

        pte_table[i] = hh_pte_entry;

        pte_add_counter += PAGE_SIZE;
    }

    load_cr3(pa((uint64_t) pml4_table));

}
