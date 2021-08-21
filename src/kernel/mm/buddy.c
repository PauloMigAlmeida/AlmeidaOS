/*
 * buddy.c
 *
 *  Created on: 18/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/buddy.h"
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

static void __print_mem_region(mem_map_region_t rg) {
    printk_info("%s: start: 0x%llx end: 0x%llx length (Kb): %llu", __func__, rg.base_addr,
            rg.base_addr + rg.length,
            rg.length / 1024);
}

static uint64_t goto_porder_idx(buddy_ref_t *ref, uint8_t pow_order) {
    uint64_t offset_addr = 0;
    int cond = ref->max_pow_order - pow_order;
    for (int i = (ref->max_pow_order - ref->min_pow_order); i > cond; i--) {
        offset_addr += upow(2, i);
    }
    return ref->header_mem_reg.base_addr + (offset_addr * sizeof(buddy_slot_t));
}

__force_inline static uint64_t n_of_entries(buddy_ref_t *ref, uint8_t pow_order) {
    return upow(2, ref->max_pow_order - pow_order);
}

__force_inline static bool is_entry_empty(buddy_slot_t *idx) {
    return idx->base_addr == 0 && idx->pow_order == 0 && idx->used == 0;
}

uint64_t buddy_calc_header_space(uint64_t mem_space) {
    BUG_ON(mem_space != flp2(mem_space));

    int max_k = ilog2(mem_space);
    int min_k = ilog2(BUDDY_ALLOC_SMALLEST_BLOCK);
    return (upow(2, (max_k - min_k) + 1) - 1) * sizeof(buddy_slot_t);
}

buddy_ref_t buddy_init(mem_map_region_t h_mem_reg, mem_map_region_t c_mem_reg) {
    BUG_ON(c_mem_reg.length != flp2(c_mem_reg.length));

    __print_mem_region(h_mem_reg);
    __print_mem_region(c_mem_reg);

    uint8_t max_pow_order = ilog2(c_mem_reg.length);

    buddy_ref_t ref = {
            .header_mem_reg = h_mem_reg,
            .content_mem_reg = c_mem_reg,
            .max_pow_order = max_pow_order,
            .min_pow_order = ilog2(BUDDY_ALLOC_SMALLEST_BLOCK)
    };

    memzero((void*) h_mem_reg.base_addr, h_mem_reg.length);

    buddy_slot_t *ptr = (buddy_slot_t*) goto_porder_idx(&ref, max_pow_order);
    buddy_slot_t largest_slot = {
            .base_addr = c_mem_reg.base_addr,
            .pow_order = max_pow_order,
            .used = 0
    };
    *ptr = largest_slot;

    return ref;
}

static buddy_slot_t* find_free_slot(buddy_ref_t *ref, uint8_t pow_order) {
    buddy_slot_t *ret;
    buddy_slot_t *idx = (buddy_slot_t*) goto_porder_idx(ref, pow_order);
    uint64_t n_entries = n_of_entries(ref, pow_order);

    for (size_t i = 0; i < n_entries; i++) {
        buddy_slot_t *tmp = (idx + i);
        if (is_entry_empty(tmp)) {
            return NULL;
        }
        else if (tmp->used == 0) {
            ret = tmp;
            break;
        }

    }
    return ret;
}

static void insert_slot(buddy_ref_t *ref, buddy_slot_t slot) {
    buddy_slot_t *idx = (buddy_slot_t*) goto_porder_idx(ref, slot.pow_order);
    uint64_t n_entries = n_of_entries(ref, slot.pow_order);

    for (size_t i = 0; i < n_entries; i++) {
        buddy_slot_t *tmp = (idx + i);
        if (is_entry_empty(tmp)) {
            *tmp = slot;
            break;
        }
    }
}

static void remove_slot(buddy_ref_t *ref, buddy_slot_t *slot) {
    buddy_slot_t *idx = (buddy_slot_t*) goto_porder_idx(ref, slot->pow_order);
    uint64_t n_entries = n_of_entries(ref, slot->pow_order);

    for (size_t i = 0; i < n_entries; i++) {
        buddy_slot_t *tmp = (idx + i);
        if (tmp->base_addr == slot->base_addr) {

            if (i == (n_entries - 1))
                memzero(tmp, sizeof(buddy_slot_t));
            else {
                memmove(tmp, tmp + 1, (n_entries - i) * sizeof(buddy_slot_t));
            }

            break;
        }
    }
}

void* buddy_alloc(buddy_ref_t *ref, uint64_t bytes) {
    BUG_ON(bytes == 0 || bytes > ref->content_mem_reg.length);

    uint8_t k_order = ilog2(clp2(bytes));

    /* sanity check */
    if (k_order < ref->min_pow_order)
        k_order = ref->min_pow_order;

    bool found = false;
    uintptr_t *ptr = NULL;
    uint8_t tmp_k_order = k_order;

    while (true) {
        buddy_slot_t *idx = find_free_slot(ref, tmp_k_order);

        if (!idx) {
            /* we've exausted all possibilities, simply there is no memory left */
            if (tmp_k_order == ref->max_pow_order)
                break;

            /* nothing found in this k_order, we need to split a higher order blk */
            tmp_k_order++;

        } else if (idx->pow_order > k_order) {
            /* split and insert */
            buddy_slot_t left = {
                    .base_addr = idx->base_addr,
                    .pow_order = idx->pow_order - 1,
                    .used = 0
            };

            buddy_slot_t right = {
                    .base_addr = idx->base_addr + upow(2, idx->pow_order - 1),
                    .pow_order = idx->pow_order - 1,
                    .used = 0
            };

            insert_slot(ref, left);
            insert_slot(ref, right);

            /* remove higher one (old) */
            remove_slot(ref, idx);

            /* now that the split happened, try to acquire the one we are after */
            tmp_k_order--;
        } else {
            /* we found the perfect fit \o/ */
            idx->used = 1;
            found = true;
            ptr = (uintptr_t*) idx->base_addr;
            break;
        }
    }

    BUG_ON(!found);
    return ptr;
}

static buddy_slot_t* find_slot(buddy_ref_t *ref, void *ptr) {
    buddy_slot_t *slot_found = NULL;

    for (uint8_t i = ref->max_pow_order; i >= ref->min_pow_order; i--) {
        buddy_slot_t *idx = (buddy_slot_t*) goto_porder_idx(ref, i);
        uint64_t n_entries = n_of_entries(ref, i);

        for (size_t i = 0; i < n_entries; i++) {
            buddy_slot_t *tmp = (idx + i);
            if (is_entry_empty(tmp)) {
                break;
            } else if (((uintptr_t*)tmp->base_addr) == ptr) {
                slot_found = tmp;
                goto ret;
            }
        }
    }

    ret:
    BUG_ON(!slot_found);
    return slot_found;
}

void buddy_free(buddy_ref_t *ref, void *ptr) {
    /* sanity check */
    BUG_ON(!ptr);

    /* free the slot */
    buddy_slot_t *left_slot = find_slot(ref, ptr);
    left_slot->used = 0;

    /* check if its buddy is also free so we can merge them */
    for (uint8_t i = left_slot->pow_order; i < ref->max_pow_order; i++) {
        char *t_ptr = (char*) ptr;
        t_ptr += upow(2, left_slot->pow_order);
        buddy_slot_t *right_slot = find_slot(ref, t_ptr);

        /* if it's still used then party is over */
        if (right_slot->used)
            break;

        /* we can merge this one */
        buddy_slot_t large_slot = {
                .base_addr = left_slot->base_addr,
                .pow_order = left_slot->pow_order + 1,
                .used = 0
        };

        insert_slot(ref, large_slot);
        remove_slot(ref, left_slot);
        remove_slot(ref, right_slot);

        /* feed the logic for higher pow orders recurrence */
        memcpy(left_slot, &large_slot, sizeof(buddy_slot_t));

    }

}
