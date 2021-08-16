/*
 * mem.c
 *
 *  Created on: Jul 27, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/arch/mem.h"
#include "kernel/lib/printk.h"
#include "kernel/lib/string.h"
#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"
#include "kernel/compiler/bug.h"
#include "kernel/lib/qsort.h"

#define E820_MEM_TYPE_USABLE    1
#define E820_MEM_TYPE_RESERVED  2

extern uintptr_t e820_mem_start;
extern uintptr_t e820_mem_end;

/*
 *  Address Range Descriptor Structure
 *
 *  Offset in Bytes     Name            Description
 *  0                   BaseAddrLow     Low 32 Bits of Base Address
 *  4                   BaseAddrHigh    High 32 Bits of Base Address
 *  8                   LengthLow       Low 32 Bits of Length in Bytes
 *  12                  LengthHigh      High 32 Bits of Length in Bytes
 *  16                  Type            Address type of  this range.
 *
 */

typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
} __packed mem_map_region_t;

typedef struct {
    uint64_t num_entries;
    mem_map_region_t mem_region[];
} __packed mem_map_block_t;

static mem_map_block_t *mem_blocks;
static mem_phys_stats_t phys_mem_stat = { .phys_avail_mem = 0, .phys_free_mem = 0 };

/* calculates how much memory we have available so we can choose how to partition the phys mem later */
static void calc_phys_memory_stats(void) {
    uint64_t total_mem = 0;
    uint64_t total_free_mem = 0;

    for (size_t cur_pos = 0; cur_pos < mem_blocks->num_entries; cur_pos++) {
        mem_map_region_t mem_region = mem_blocks->mem_region[cur_pos];

        total_mem += mem_region.length;

        if (mem_region.type == E820_MEM_TYPE_USABLE)
            total_free_mem += mem_region.length;
    }

    phys_mem_stat.phys_avail_mem = total_mem;
    phys_mem_stat.phys_free_mem = total_free_mem;

    printk_info("Memory Length (kB): Total: %lu Free: %lu", total_mem / 1024, total_free_mem / 1024);
}

static void print_mem_regions(void) {
    for (size_t cur_pos = 0; cur_pos < mem_blocks->num_entries; cur_pos++) {
        mem_map_region_t mem_region = mem_blocks->mem_region[cur_pos];

        printk_info("Addr: 0x%.16llx - 0x%.16llx, Length (KB): %u, Type:%u", mem_region.base_addr,
                mem_region.base_addr + mem_region.length /*- 1*/,
                mem_region.length / 1024,
                mem_region.type);
    }
}

static int qsort_cmp_mem_region(const void *a, const void *b) {
    mem_map_region_t *a_cst = (mem_map_region_t*) a;
    mem_map_region_t *b_cst = (mem_map_region_t*) b;

    if (a_cst->base_addr > b_cst->base_addr)
        return 1;
    else if (a_cst->base_addr < b_cst->base_addr)
        return -1;
    else
        return 0;
}

/* I might regret this decision but as of now I only care whether the mem region is free or reserved. Let's see :) */
static void combine_mergeable_regions() {
    /* sanity checks */
    if (mem_blocks->num_entries < 2) return;

    size_t ptr_1 = 0, ptr_2 = 1;
    do {
        mem_map_region_t *mem_rg_1 = &mem_blocks->mem_region[ptr_1];
        mem_map_region_t *mem_rg_2 = &mem_blocks->mem_region[ptr_2];

        /* I only care whether regions are either usable or reserved for now */
        if (mem_rg_1->type > E820_MEM_TYPE_USABLE)
            mem_blocks->mem_region[ptr_1].type = E820_MEM_TYPE_RESERVED;

        bool merge = false;
        /* do the regions intersect ? */
        if ((mem_rg_1->base_addr + mem_rg_1->length) >= mem_rg_2->base_addr) {
            /* if they have the same type */
            if (mem_rg_1->type == mem_rg_2->type)
                merge = true;
            /* are both "reserved" then merge them */
            else if (mem_rg_1->type > E820_MEM_TYPE_USABLE && mem_rg_2->type > E820_MEM_TYPE_USABLE)
                merge = true;
        }

        if (merge) {
            /* combine regions */
            mem_rg_1->length += mem_rg_2->length;
            /* mark region to be removed/re-used */
            memzero(&mem_blocks->mem_region[ptr_2], sizeof(mem_map_region_t));
        } else {
            ptr_1 = ptr_2;
        }

        ptr_2++;

    } while (ptr_2 <= mem_blocks->num_entries);

}

static void squash_mem_regions() {
    /* sanity checks */
    if (mem_blocks->num_entries < 2) return;

    for (size_t ptr_1 = 0; ptr_1 < mem_blocks->num_entries - 1; ptr_1++) {

        mem_map_region_t *mem_rg = &mem_blocks->mem_region[ptr_1];

        if (mem_rg->length == 0 && mem_rg->type == 0) {

            bool found_next_entry = false;
            for (size_t next_ptr = ptr_1 + 1; next_ptr < mem_blocks->num_entries; next_ptr++) {
                mem_map_region_t *next_mem_rg = &mem_blocks->mem_region[next_ptr];

                if (next_mem_rg->length != 0 && next_mem_rg->type != 0) {
                    found_next_entry = true;

                    memcpy(mem_rg, next_mem_rg, sizeof(mem_map_region_t) * (mem_blocks->num_entries - next_ptr));
                    break;
                }
            }

            if (!found_next_entry)
                break;

            /* mark region to be removed/re-used */
            memzero(&mem_blocks->mem_region[mem_blocks->num_entries - 1], sizeof(mem_map_region_t));

            mem_blocks->num_entries--;

        }
    }

}

void mem_init(void) {
    printk_info("e820_mem_start: 0x%.8lx e820_mem_end: 0x%.8lx", e820_mem_start, e820_mem_end);

    mem_blocks = (mem_map_block_t*) e820_mem_start;

    /* BIOS returns those entries unsorted */
    qsort(mem_blocks->mem_region, mem_blocks->num_entries, sizeof(mem_map_region_t), qsort_cmp_mem_region);

    /* combine regions to 1) make our life simpler 2) free space on the dedicated space for that */
    combine_mergeable_regions();
    squash_mem_regions();

    /* Calculate available phys memory once so we don't have to do it again */
    calc_phys_memory_stats();

    /* print memory map */
    print_mem_regions();

    printk_info("read_bios_mem_map routine read %llu entries", mem_blocks->num_entries);

    /*
     * TODO: Next steps:
     * - Map kernel space in here so we don't end up offering that memory to something else that can override it
     * - Figure out what to do with the Kernel stack mem? (how to reserve that to avoid data corruption)
     * - Initiate a memory allocator (slab or buddy system)
     * - implement kmalloc
     * - implement vmalloc
     *
     * HOw am I gonna reserve space for the mem allocator? Seems like the chicken and the egg problem
     */
}

mem_phys_stats_t mem_phys_stat(void) {
    /* this assumes that calc_phys_memory_stats() was called during mem_init() */
    BUG_ON(phys_mem_stat.phys_avail_mem == 0);

    return phys_mem_stat;
}

