/*
 * buddy_alloc.c
 *
 *  Created on: 18/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/buddy_alloc.h"
#include "kernel/compiler/bug.h"
#include "kernel/lib/printk.h"
#include "kernel/lib/math.h"

/*
 * Typically the lower limit would be small enough to minimize the average
 * wasted space per allocation, but large enough to avoid excessive overhead.
 */
#define BUDDY_ALLOC_SMALLEST_BLOCK   4096

buddy_alloc_ref_t buddy_alloc_init(mem_map_region_t mem_reg) {
    BUG_ON(mem_reg.length != flp2(mem_reg.length));

    uint64_t n_pages = mem_reg.length / BUDDY_ALLOC_SMALLEST_BLOCK;

    mem_map_region_t header_mem_reg = {
            .base_addr = mem_reg.base_addr,
            .length = n_pages
    };

    mem_map_region_t content_mem_reg = {
            .base_addr = mem_reg.base_addr + n_pages,
            .length = mem_reg.length - n_pages
    };

    buddy_alloc_ref_t ref = {
            .header_mem_reg = header_mem_reg,
            .content_mem_reg = content_mem_reg
    };
    return ref;
}

