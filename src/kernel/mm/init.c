/*
 * init.c
 *
 *  Created on: 18/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/init.h"
#include "kernel/mm/page.h"
#include "kernel/mm/kmem.h"
#include "kernel/mm/addressconv.h"
#include "kernel/mm/buddy.h"
#include "kernel/arch/mem.h"
#include "kernel/lib/math.h"
#include "kernel/compiler/bug.h"

/*
 * TODO: Next steps:
 * - Map kernel space in here so we don't end up offering that memory to something else that can override it - Done
 * - Figure out what to do with the Kernel stack mem? (how to reserve that to avoid data corruption) - Done
 * - Initiate a memory allocator (slab or buddy system) - Done (Buddy)
 * - implement kmalloc - Done
 * - implement vmalloc - TBD
 *
 * HOw am I gonna reserve space for the mem allocator? Seems like the chicken and the egg problem
 */

extern volatile void kernel_virt_start_addr;
extern volatile void kernel_virt_end_addr;

#define KMEM_MAX_KERNEL_SPACE   (1 * 1024 * 1024 * 1024) // 1 GB

static uint64_t calc_kernel_mem_space() {
    mem_phys_stats_t stats = mem_stat();
    uint64_t k_mem_space = stats.phys_avail_mem / 3;

    /* we cap kernel mem at 1Gb for high memory systems */
    if (k_mem_space > KMEM_MAX_KERNEL_SPACE)
        k_mem_space = KMEM_MAX_KERNEL_SPACE;

    /* round it down if not a power of 2*/
    k_mem_space = flp2(k_mem_space);
    printk_info("kernel space is: %llu bytes", k_mem_space);
    return k_mem_space;
}

void mm_init(void) {
    /* read/prepare entries returned from BIOS 0xe820 */
    mem_init();

    /* reserve memory area to hold kernel stack which is going to be 2x 4Kb PAGES (same as Linux) */
    mem_alloc_region(
            pa((uint64_t) &kernel_virt_start_addr) - ELF_TEXT_OFFSET - PAGE_SIZE * 2,
            round_up_po2(pa((uint64_t) &kernel_virt_start_addr) - ELF_TEXT_OFFSET, PAGE_SIZE));

    /* reserve memory area already used to hold the kernel */
    mem_alloc_region(
            pa((uint64_t) &kernel_virt_start_addr) - ELF_TEXT_OFFSET,
            round_up_po2(pa((uint64_t) &kernel_virt_end_addr), PAGE_SIZE));

    uint64_t k_mem_content_space = calc_kernel_mem_space();
    uint64_t k_mem_header_space = buddy_calc_header_space(k_mem_content_space);

    /* Calculate space required to hold page table struct to accomodate the entire kernel space */
    uint64_t paging_mem = paging_calc_space_needed(
            pa((uint64_t) &kernel_virt_end_addr) + k_mem_content_space + k_mem_header_space);
    mem_map_region_t k_pages_struct_rg = mem_alloc_amount(paging_mem);

    /*
     * We have to make sure that the paging structure sits within the first 10MB
     * identity-mapped pages set up before we moved to long mode, otherwise we can't
     * access these locations. Luckily for us, we sorted the e820 return content
     * so that we are likely to find a space that fits in the begining of the
     * physical memory.
     */
    BUG_ON((k_pages_struct_rg.base_addr + k_pages_struct_rg.length) > (10*1024*1024));
    paging_init(k_pages_struct_rg);
    paging_identity_map(0, pa((uint64_t) &kernel_virt_end_addr), PHYS_ADDR_KERNEL_START);
    paging_reload_cr3();


    /* reserve memory area to be used by the buddy memory allocator */
    mem_map_region_t k_mem_header_rg = mem_alloc_amount(k_mem_header_space);
    mem_map_region_t k_mem_content_rg = mem_alloc_amount(k_mem_content_space);

    mem_print_entries();

    //TEMP disabled until I finish testing the paging stuff
    /* initialise kernel memory && kmalloc */
//    kmem_init(k_mem_header_rg, k_mem_content_rg);

}

