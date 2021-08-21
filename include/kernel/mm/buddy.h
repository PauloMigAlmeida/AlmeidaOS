/*
 * buddy.h
 *
 *  Created on: 18/08/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_MM_BUDDY_H_
#define INCLUDE_KERNEL_MM_BUDDY_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/arch/mem.h"

typedef struct {
    mem_map_region_t header_mem_reg;
    mem_map_region_t content_mem_reg;
    uint8_t max_pow_order;
    uint8_t min_pow_order;
} buddy_ref_t;

uint64_t buddy_calc_header_space(uint64_t mem_space);
buddy_ref_t buddy_init(mem_map_region_t h_mem_reg, mem_map_region_t c_mem_reg);
void* buddy_alloc(buddy_ref_t *ref, uint64_t bytes);
void buddy_free(buddy_ref_t *ref, uintptr_t ptr);


#endif /* INCLUDE_KERNEL_MM_BUDDY_H_ */
