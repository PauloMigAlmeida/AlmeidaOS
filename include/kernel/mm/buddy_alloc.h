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
} buddy_alloc_ref_t;

buddy_alloc_ref_t buddy_alloc_init(mem_map_region_t mem_reg);

#endif /* INCLUDE_KERNEL_MM_BUDDY_ALLOC_H_ */
