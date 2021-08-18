/*
 * init.c
 *
 *  Created on: 18/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/init.h"
#include "kernel/mm/page.h"
#include "kernel/mm/addressconv.h"
#include "kernel/mm/buddy_alloc.h"
#include "kernel/arch/mem.h"
#include "kernel/lib/printk.h"
#include "kernel/lib/math.h"

extern volatile void kernel_virt_start_addr;
extern volatile void kernel_virt_end_addr;

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

void mm_init(void) {
    /* read/prepare entries returned from BIOS 0xe820 */
    mem_init();

    /* reserve memory area to hold kernel stack which is going to be 2x 4Kb PAGES (same as Linux) */
    mem_map_region_t k_stack_rg = mem_alloc_region(
            pa((uint64_t) &kernel_virt_start_addr) - ELF_TEXT_OFFSET - PAGE_SIZE * 2,
            round_up_po2(pa((uint64_t) &kernel_virt_start_addr) - ELF_TEXT_OFFSET, PAGE_SIZE));

    printk_info("start: 0x%llx end: 0x%llx length (Kb): %llu", k_stack_rg.base_addr,
            k_stack_rg.base_addr + k_stack_rg.length,
            k_stack_rg.length / 1024);

    /* reserve memory area already used to hold the kernel */
    mem_map_region_t k_text_rg = mem_alloc_region(
            pa((uint64_t) &kernel_virt_start_addr) - ELF_TEXT_OFFSET,
            round_up_po2(pa((uint64_t) &kernel_virt_end_addr), PAGE_SIZE));

    printk_info("start: 0x%llx end: 0x%llx length (Kb): %llu", k_text_rg.base_addr,
            k_text_rg.base_addr + k_text_rg.length,
            k_text_rg.length / 1024);

    /* reserve memory area to be used by the buddy memory allocator */
    mem_map_region_t buddy_mem_rg = mem_alloc_amount(3*1024*1024);
    buddy_alloc_init(buddy_mem_rg);

    mem_print_entries();
    paging_init();
}

