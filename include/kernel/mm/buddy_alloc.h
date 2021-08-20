/*
 * buddy_alloc.h
 *
 *  Created on: 18/08/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_MM_BUDDY_ALLOC_H_
#define INCLUDE_KERNEL_MM_BUDDY_ALLOC_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/arch/mem.h"

typedef struct {
    mem_map_region_t header_mem_reg;
    mem_map_region_t content_mem_reg;
    uint8_t max_pow_order;
    uint8_t min_pow_order;
} buddy_alloc_ref_t;

uint64_t buddy_alloc_calc_header_space(uint64_t mem_space);
buddy_alloc_ref_t buddy_alloc_init(mem_map_region_t h_mem_reg, mem_map_region_t c_mem_reg);

#endif /* INCLUDE_KERNEL_MM_BUDDY_ALLOC_H_ */
