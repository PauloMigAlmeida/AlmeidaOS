/*
 * kmem.c
 *
 *  Created on: 19/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/kmem.h"
#include "kernel/mm/init.h"
#include "kernel/mm/buddy_alloc.h"
#include "kernel/mm/addressconv.h"
#include "kernel/arch/mem.h"
#include "kernel/lib/printk.h"
#include "kernel/lib/math.h"

extern volatile void kernel_virt_start_addr;
extern volatile void kernel_virt_end_addr;

static buddy_alloc_ref_t k_mem_alloc;

uint64_t kmem_calc_kernel_mem_space() {
    mem_phys_stats_t stats = mem_stat();
    uint64_t k_mem_space = stats.phys_avail_mem / 3;

    /* we cap kernel mem at 1Gb for high memory systems */
    if (k_mem_space > (1 * 1024 * 1024 * 1024))
        k_mem_space = (1 * 1024 * 1024 * 1024);

    printk_info("kernel space is: %llu bytes", k_mem_space);

    return flp2(k_mem_space);
}

void kmem_init(void) {
    /* calc memory space the kernel is entitled to */
    uint64_t k_mem_space = kmem_calc_kernel_mem_space();

    /* reserve memory area to hold kernel stack which is going to be 2x 4Kb PAGES (same as Linux) */
    mem_map_region_t k_stack_rg = mem_alloc_region(
            pa((uint64_t) &kernel_virt_start_addr) - ELF_TEXT_OFFSET - PAGE_SIZE * 2,
            round_up_po2(pa((uint64_t) &kernel_virt_start_addr) - ELF_TEXT_OFFSET, PAGE_SIZE));

    /* reserve memory area already used to hold the kernel */
    mem_map_region_t k_text_rg = mem_alloc_region(
            pa((uint64_t) &kernel_virt_start_addr) - ELF_TEXT_OFFSET,
            round_up_po2(pa((uint64_t) &kernel_virt_end_addr), PAGE_SIZE));

    /* subtract the memory that the kernel is already using */
//    k_mem_space -= 1 * 1024 * 1024;
//    k_mem_space -= k_stack_rg.length;
//    k_mem_space -= k_text_rg.length;

    /* reserve memory area to be used by the buddy memory allocator */
    mem_map_region_t k_mem_rg = mem_alloc_amount(k_mem_space);
    k_mem_alloc = buddy_alloc_init(k_mem_rg);

    mem_print_entries();

}

