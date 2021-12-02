/*
 * pageframe.h
 *
 *  Created on: 29/10/2021
 *      Author: paulo
 */

#ifndef INCLUDE_KERNEL_MM_PAGEFRAME_H_
#define INCLUDE_KERNEL_MM_PAGEFRAME_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/arch/mem.h"

struct pageframe_t {
    uintptr_t phy_addr;
    struct pageframe_t *next;
};

typedef struct {
    struct pageframe_t *free;
    struct pageframe_t *used;
} pageframe_database_t;

pageframe_database_t pageframe_init(mem_map_region_t k_pages_struct_rg, mem_map_region_t k_pfdb_struct_rg);
uint64_t pageframe_calc_space_needed(uint64_t pagetable_bytes);
uint64_t pageframe_alloc(pageframe_database_t *pfdb);
void pageframe_free(pageframe_database_t *pfdb, uint64_t phy_addr);

#endif /* INCLUDE_KERNEL_MM_PAGEFRAME_H_ */
