/*
 * kmem.c
 *
 *  Created on: 19/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/kmem.h"
#include "kernel/mm/init.h"
#include "kernel/mm/buddy.h"
#include "kernel/mm/addressconv.h"
#include "kernel/arch/mem.h"
#include "kernel/lib/printk.h"
#include "kernel/lib/math.h"

#define KMEM_MAX_KERNEL_SPACE   (1 * 1024 * 1024 * 1024) // 1 GB

extern volatile void kernel_virt_start_addr;
extern volatile void kernel_virt_end_addr;

static buddy_ref_t k_mem_alloc;

uint64_t kmem_calc_kernel_mem_space() {
    mem_phys_stats_t stats = mem_stat();
    uint64_t k_mem_space = stats.phys_avail_mem / 3;

    /* we cap kernel mem at 1Gb for high memory systems */
    if (k_mem_space > KMEM_MAX_KERNEL_SPACE)
        k_mem_space = KMEM_MAX_KERNEL_SPACE;

    printk_info("kernel space is: %llu bytes", k_mem_space);

    return flp2(k_mem_space);
}

void kmem_init(void) {

    /* reserve memory area to hold kernel stack which is going to be 2x 4Kb PAGES (same as Linux) */
    mem_alloc_region(
            pa((uint64_t) &kernel_virt_start_addr) - ELF_TEXT_OFFSET - PAGE_SIZE * 2,
            round_up_po2(pa((uint64_t) &kernel_virt_start_addr) - ELF_TEXT_OFFSET, PAGE_SIZE));

    /* reserve memory area already used to hold the kernel */
    mem_alloc_region(
            pa((uint64_t) &kernel_virt_start_addr) - ELF_TEXT_OFFSET,
            round_up_po2(pa((uint64_t) &kernel_virt_end_addr), PAGE_SIZE));

    uint64_t k_mem_content_space = kmem_calc_kernel_mem_space();

    /* reserve memory area to be used by the buddy memory allocator */
    /*
     * TODO: I have no guarantee that this will be mapped within the 10 MB identity-mapped pages
     * (most likely not)..
     *
     * We have to ensure that this get mapped as well when initiating the paging mechanism
     */
    uint64_t k_mem_header_space = buddy_calc_header_space(k_mem_content_space);
    mem_map_region_t k_mem_header_rg = mem_alloc_amount(k_mem_header_space);
    mem_map_region_t k_mem_content_rg = mem_alloc_amount(k_mem_content_space);
    k_mem_alloc = buddy_init(k_mem_header_rg, k_mem_content_rg);

//    mem_print_entries();

}

void* kmalloc(uint64_t bytes) {
    return buddy_alloc(&k_mem_alloc, bytes);
}

//void kfree(void *ptr) {
//    buddy_free(&k_mem_alloc, ptr);
//}

