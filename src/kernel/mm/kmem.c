/*
 * kmem.c
 *
 *  Created on: 19/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/kmem.h"
#include "kernel/mm/init.h"
#include "kernel/mm/buddy.h"
#include "kernel/mm/page.h"
#include "kernel/mm/addressconv.h"
#include "kernel/arch/mem.h"
#include "kernel/lib/string.h"

static buddy_ref_t k_mem_alloc;

static void mem_proc_handler(const mem_map_region_t *mem_rg) {

    /* sanity check as some BIOSes can return regions outside of the physical memory range*/
    //TODO research a bit more about it Paulo...that sounds very weird tbh
    if ((mem_rg->base_addr + mem_rg->length) <= k_mem_alloc.content_mem_reg.length) {
        buddy_pre_alloc(&k_mem_alloc, mem_rg->base_addr, mem_rg->length);
    }
}

void kmem_init(mem_map_region_t k_mem_header_rg, mem_map_region_t k_mem_content_rg) {
    k_mem_alloc = buddy_init(k_mem_header_rg, k_mem_content_rg);

    /* pre-allocate buddy entries with reserved spaced from e820 to avoid double alloc */
    mem_list_entries(E820_MEM_TYPE_RESERVED, &mem_proc_handler);
}

void* kmalloc(uint64_t bytes, int flags) {
    uintptr_t phy_addr = buddy_alloc(&k_mem_alloc, bytes);
    uintptr_t va_addr = va(phy_addr);

    if (!(flags & KMEM_RAW_ALLOC)) {
        //TODO Switch that to active_pagetable to account for user space stuff?
        paging_contiguous_map(kernel_pagetable(),
                phy_addr,
                phy_addr + bytes,
                va_addr,
                PAGE_PRESENT_BIT | PAGE_READ_WRITE_BIT);

        if (flags & KMEM_ZERO) {
            memzero((uintptr_t*) va_addr, bytes);
        }
    }

    return (uintptr_t*) va_addr;
}

void kfree(void *ptr) {
    buddy_free(&k_mem_alloc, pa((uintptr_t) ptr));
}

