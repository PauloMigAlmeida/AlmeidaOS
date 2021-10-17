/*
 * kmem.c
 *
 *  Created on: 19/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/kmem.h"
#include "kernel/mm/init.h"
#include "kernel/mm/buddy.h"

static buddy_ref_t k_mem_alloc;

void kmem_init(mem_map_region_t k_mem_header_rg, mem_map_region_t k_mem_content_rg) {
    k_mem_alloc = buddy_init(k_mem_header_rg, k_mem_content_rg);
}

void* kmalloc(uint64_t bytes) {
    //TODO we have to deal with Virtual addresses here too
    return buddy_alloc(&k_mem_alloc, bytes);
}

void kfree(void *ptr) {
    buddy_free(&k_mem_alloc, (uintptr_t)ptr);
}

