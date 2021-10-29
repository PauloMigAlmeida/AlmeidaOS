/*
 * page.h
 *
 *  Created on: 15/08/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_MM_PAGE_H_
#define INCLUDE_KERNEL_MM_PAGE_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"
#include "kernel/arch/mem.h"

/*
 * Notes to myself:
 *
 * I'm assuming 4-level paging only (48-bit). Should I ever want to go
 * for 5-level paging (52-bit_, I will have to tweak the available_guardhole
 * bitfield for that.
 */

typedef struct {
	uint16_t flags:12;
	uint64_t pdpe_base_addr:36;
	uint16_t available_guardhole:15;
	uint8_t no_execute_bit:1;
} __packed pml4e_t;

typedef struct {
	uint16_t flags:12;
	uint64_t pde_base_addr:36;
	uint16_t available_guardhole:15;
	uint8_t no_execute_bit:1;
} __packed pdpe_t;

typedef struct {
	uint16_t flags:12;
	uint64_t pte_base_addr:36;
	uint16_t available_guardhole:15;
	uint8_t no_execute_bit:1;
} __packed pde_t;

typedef struct {
	uint16_t flags:12;
	uint64_t phys_pg_base_addr:36;
	uint16_t available_guardhole:15;
	uint8_t no_execute_bit:1;
} __packed pte_t;

uint64_t paging_calc_space_needed(uint64_t bytes);
uint64_t pageframedb_calc_space_needed(uint64_t pagetable_bytes);
void paging_init(mem_map_region_t k_pages_struct_rg, mem_map_region_t k_pfdb_struct_rg);
void paging_contiguous_map(uint64_t p_start_addr, uint64_t p_end_addr, uint64_t v_base_start_addr);
void paging_reload_cr3();

#endif /* INCLUDE_KERNEL_MM_PAGE_H_ */
