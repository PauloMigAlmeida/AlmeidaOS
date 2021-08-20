/*
 * buddy_alloc.c
 *
 *  Created on: 18/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/buddy_alloc.h"
#include "kernel/compiler/macro.h"
#include "kernel/compiler/bug.h"
#include "kernel/lib/printk.h"
#include "kernel/lib/math.h"
#include "kernel/lib/string.h"

/*
 * Typically the lower limit would be small enough to minimize the average
 * wasted space per allocation, but large enough to avoid excessive overhead.
 */
#define BUDDY_ALLOC_SMALLEST_BLOCK   4096

typedef struct {
    uint64_t base_addr;
    uint8_t pow_order;
    uint8_t used;
} buddy_slot_t;

void __print_mem_region(mem_map_region_t rg) {
    printk_info("%s: start: 0x%llx end: 0x%llx length (Kb): %llu", __func__, rg.base_addr,
            rg.base_addr + rg.length,
            rg.length / 1024);
}

uint64_t buddy_alloc_goto_porder_idx(buddy_alloc_ref_t *ref, uint8_t pow_order) {
    uint64_t offset_addr = 0;
    int cond = (ref->max_pow_order - pow_order);
    for (int i = (ref->max_pow_order - ref->min_pow_order); i >= cond; i--) {
        offset_addr += upow(2, i) * sizeof(buddy_slot_t);
    }
    return ref->header_mem_reg.base_addr + offset_addr;
}

uint64_t buddy_alloc_calc_header_space(uint64_t mem_space) {
    BUG_ON(mem_space != flp2(mem_space));

    int max_k = ilog2(mem_space);
    int min_k = ilog2(BUDDY_ALLOC_SMALLEST_BLOCK);
    return (upow(2, (max_k - min_k) + 1)) * sizeof(buddy_slot_t);
}

buddy_alloc_ref_t buddy_alloc_init(mem_map_region_t h_mem_reg, mem_map_region_t c_mem_reg) {
    BUG_ON(c_mem_reg.length != flp2(c_mem_reg.length));

    __print_mem_region(h_mem_reg);
    __print_mem_region(c_mem_reg);

    uint8_t max_pow_order = ilog2(c_mem_reg.length);

    buddy_alloc_ref_t ref = {
                .header_mem_reg = h_mem_reg,
                .content_mem_reg = c_mem_reg,
                .max_pow_order = max_pow_order,
                .min_pow_order = ilog2(BUDDY_ALLOC_SMALLEST_BLOCK)
        };

    memzero((void*)h_mem_reg.base_addr, h_mem_reg.length);

    buddy_slot_t *ptr = (buddy_slot_t*)(buddy_alloc_goto_porder_idx(&ref, max_pow_order));
    buddy_slot_t largest_slot = {
            .base_addr = 0,
            .pow_order = max_pow_order,
            .used = 0
    };
    *ptr = largest_slot;

    return ref;
}

