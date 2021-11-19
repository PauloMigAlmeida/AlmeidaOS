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
#include "kernel/lib/bit.h"

typedef enum {
    UNUSED,
    SPLIT,
    USED,
} slot_type_t;

typedef struct {
    uint64_t base_addr;
    uint8_t pow_order;
    slot_type_t type;
} buddy_slot_t;

uint64_t buddy_calc_header_space(uint64_t mem_space) {
    BUG_ON(mem_space != flp2(mem_space));

    int max_k = ilog2(mem_space);
    int min_k = ilog2(BUDDY_ALLOC_SMALLEST_BLOCK);
    return (upow(2, (max_k - min_k) + 1)) * sizeof(buddy_slot_t);
}

static uint64_t goto_porder_idx(buddy_ref_t *ref, uint8_t pow_order) {
    uint64_t offset_addr = upow(2, (ref->max_pow_order - (ref->min_pow_order - 1)))
            - upow(2, ref->max_pow_order - pow_order + 1);
    return ref->header_mem_reg.base_addr + (offset_addr * sizeof(buddy_slot_t));
}

__force_inline static buddy_slot_t* goto_parent(buddy_ref_t *ref, buddy_slot_t *child) {
    uint64_t arr_pos = ((uintptr_t) child - goto_porder_idx(ref, child->pow_order)) / sizeof(buddy_slot_t);
    clear_bit(0, arr_pos);
    return ((buddy_slot_t*) goto_porder_idx(ref, child->pow_order + 1)) + (arr_pos / 2);
}

__force_inline static buddy_slot_t* goto_left_child(buddy_ref_t *ref, buddy_slot_t *parent) {
    uint64_t arr_pos = ((uintptr_t) parent - goto_porder_idx(ref, parent->pow_order)) / sizeof(buddy_slot_t);
    return ((buddy_slot_t*) goto_porder_idx(ref, parent->pow_order - 1)) + (arr_pos * 2);
}

__force_inline static buddy_slot_t* goto_right_child(buddy_ref_t *ref, buddy_slot_t *parent) {
    uint64_t arr_pos = ((uintptr_t) parent - goto_porder_idx(ref, parent->pow_order)) / sizeof(buddy_slot_t);
    return ((buddy_slot_t*) goto_porder_idx(ref, parent->pow_order - 1)) + (arr_pos * 2) + 1;
}

__force_inline static buddy_slot_t* goto_sibling(buddy_ref_t *ref, buddy_slot_t *node) {
    uint64_t arr_pos = ((uintptr_t) node - goto_porder_idx(ref, node->pow_order)) / sizeof(buddy_slot_t);

    buddy_slot_t *ret = node;

    if (arr_pos % 2 == 0)
        ret++;
    else
        ret--;

    return ret;
}

__force_inline static uint64_t n_of_entries(buddy_ref_t *ref, uint8_t pow_order) {
    return upow(2, ref->max_pow_order) / upow(2, pow_order);
}

__force_inline static bool is_entry_empty(buddy_slot_t *idx) {
    return idx->base_addr == 0 && idx->pow_order == 0;
}

buddy_ref_t buddy_init(mem_map_region_t h_mem_reg, mem_map_region_t c_mem_reg) {
    BUG_ON(c_mem_reg.length != flp2(c_mem_reg.length));

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
            .type = UNUSED
    };
    *ptr = largest_slot;

    return ref;
}

static buddy_slot_t* find_free_slot(buddy_ref_t *ref, uint8_t pow_order) {
    buddy_slot_t *ret = NULL;
    buddy_slot_t *idx = (buddy_slot_t*) goto_porder_idx(ref, pow_order);
    uint64_t n_entries = n_of_entries(ref, pow_order);

    for (size_t i = 0; i < n_entries; i++) {
        buddy_slot_t *tmp = (idx + i);
        if (!is_entry_empty(tmp) && tmp->type == UNUSED) {
            ret = tmp;
            break;
        }

    }
    return ret;
}

static void insert_child_slot(buddy_ref_t *ref, buddy_slot_t *parent, buddy_slot_t *child) {
    buddy_slot_t *idx;

    if (parent->base_addr == child->base_addr) {
        idx = goto_left_child(ref, parent);
    } else {
        idx = goto_right_child(ref, parent);
    }

    memcpy(idx, child, sizeof(buddy_slot_t));
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
                    .type = UNUSED
            };

            buddy_slot_t right = {
                    .base_addr = idx->base_addr + upow(2, idx->pow_order - 1),
                    .pow_order = idx->pow_order - 1,
                    .type = UNUSED
            };

            insert_child_slot(ref, idx, &left);
            insert_child_slot(ref, idx, &right);

            /* remove higher one (old) */
            idx->type = SPLIT;

            /* now that the split happened, try to acquire the one we are after */
            tmp_k_order--;
        } else {
            /* we found the perfect fit \o/ */
            idx->type = USED;
            found = true;
            ptr = (uintptr_t*) idx->base_addr;
            break;
        }
    }

    BUG_ON(!found);
    return ptr;
}

static buddy_slot_t* find_slot_by_addr(buddy_ref_t *ref, uintptr_t ptr) {
    buddy_slot_t *slot_found = NULL;

    buddy_slot_t *idx = (buddy_slot_t*) goto_porder_idx(ref, ref->max_pow_order);

    while (idx->pow_order >= ref->min_pow_order) {
        if (idx->type == UNUSED)
            break; // fail-fast
        else if (ptr >= idx->base_addr && ptr <= (idx->base_addr + (upow(2, idx->pow_order) / 2))) {
            if (idx->type == SPLIT) {
                idx = goto_left_child(ref, idx);
            } else if (idx->type == USED) {
                slot_found = idx;
                break;
            }
        } else {
            if (idx->type == SPLIT) {
                idx = goto_right_child(ref, idx);
            } else if (idx->type == USED) {
                slot_found = idx;
                break;
            }
        }
    }

    BUG_ON(!slot_found);
    return slot_found;
}

void buddy_free(buddy_ref_t *ref, uintptr_t ptr) {
    /* sanity check */
    BUG_ON(!ptr);

    /* free the slot */
    buddy_slot_t *ptr_slot = find_slot_by_addr(ref, ptr);
    ptr_slot->type = UNUSED;

    /* check if its buddy is also free so we can merge them */
    while (ptr_slot->pow_order < ref->max_pow_order) {

        /* find the buddy */
        buddy_slot_t *sibling_slot = goto_sibling(ref, ptr_slot);

        /* if it's still used then party is over */
        if (sibling_slot->type == USED)
            break;

        /* we can merge this one */
        buddy_slot_t *large_slot = goto_parent(ref, ptr_slot);
        large_slot->type = UNUSED;

        memzero(ptr_slot, sizeof(buddy_slot_t));
        memzero(sibling_slot, sizeof(buddy_slot_t));

        /* feed the logic for higher pow orders recurrence */
        ptr_slot = large_slot;

    }

}

//TODO optimise this fuction..there are several references to upow that can be replaced with a variable
void buddy_pre_alloc(buddy_ref_t *ref, mem_map_region_t mem_rg) {
    /* sanity checks */
    BUG_ON(mem_rg.length == 0
            || mem_rg.length > ref->content_mem_reg.length
            || mem_rg.base_addr < ref->content_mem_reg.base_addr);

    uint8_t k_order = ilog2(clp2(mem_rg.length));

    if (k_order < ref->min_pow_order)
        k_order = ref->min_pow_order;

    uint64_t ptr = mem_rg.base_addr;
    uint64_t length = clp2(mem_rg.length);

    buddy_slot_t *idx = (buddy_slot_t*) goto_porder_idx(ref, ref->max_pow_order);
    while (true) {

        if (ptr >= idx->base_addr && (ptr + length) <= (idx->base_addr + upow(2, idx->pow_order))) {

            if (idx->pow_order > k_order) {

                if (idx->type == UNUSED) {

                    /* split and insert */
                    buddy_slot_t left = {
                            .base_addr = idx->base_addr,
                            .pow_order = idx->pow_order - 1,
                            .type = UNUSED
                    };

                    buddy_slot_t right = {
                            .base_addr = idx->base_addr + upow(2, idx->pow_order - 1),
                            .pow_order = idx->pow_order - 1,
                            .type = UNUSED
                    };

                    insert_child_slot(ref, idx, &left);
                    insert_child_slot(ref, idx, &right);

                    /* remove higher one (old) */
                    idx->type = SPLIT;

                    if (ptr >= idx->base_addr && (ptr + length) <= (idx->base_addr + (upow(2, idx->pow_order) / 2))) {
                        idx = goto_left_child(ref, idx);
                    } else {
                        idx = goto_right_child(ref, idx);
                    }

                } else if (idx->type == SPLIT) {

                    if ((ptr + length) <= (idx->base_addr + (upow(2, idx->pow_order) / 2))) {
                        idx = goto_left_child(ref, idx);
                    } else if (ptr >= (idx->base_addr + (upow(2, idx->pow_order) / 2))) {
                        idx = goto_right_child(ref, idx);
                    } else {
                        /*
                         * Darn it... there is an overlapping between requested and allocated blocks
                         *
                         * rather than make it a complex tree traversal, I will split that into 2 simpler requests
                         * this will ensure that we mark as used only the blocks that we really need
                         */

                        mem_map_region_t req_1 = {
                                .base_addr = mem_rg.base_addr,
                                .length = (upow(2, idx->pow_order) / 2) - mem_rg.base_addr,
                                .type = E820_MEM_TYPE_RESERVED
                        };
                        buddy_pre_alloc(ref, req_1);

                        mem_map_region_t req_2 = {
                                .base_addr = (upow(2, idx->pow_order) / 2),
                                .length = mem_rg.base_addr - ((upow(2, idx->pow_order) / 2) - mem_rg.base_addr),
                                .type = E820_MEM_TYPE_RESERVED
                        };
                        buddy_pre_alloc(ref, req_2);

                        // work should be done by previous 2 calls
                        break;

                    }

                } else if (idx->type == USED) {
                    //job seems to be done already...so don't bother
                    break;
                }
            } else if (idx->pow_order == k_order) {

                if (idx->type == UNUSED) {
                    /* perfect fit (that's rare.. go celebrate it) */
                    idx->type = USED;
                    break;

                } else {
                    fatal();
                }
            }

        }
    }

}

