/*
 * init.c
 *
 *  Created on: 18/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/init.h"
#include "kernel/mm/page.h"
#include "kernel/mm/pageframe.h"
#include "kernel/mm/pagetable.h"
#include "kernel/mm/kmem.h"
#include "kernel/mm/addressconv.h"
#include "kernel/mm/buddy.h"
#include "kernel/arch/mem.h"
#include "kernel/lib/math.h"
#include "kernel/compiler/bug.h"

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

static void reserve_kernel_sections(void) {
    /* reserve memory area to hold kernel stack which is going to be 2x 4Kb PAGES (same as Linux) */
    mem_map_region_t kern_stack = mem_alloc_region(
            pa((uint64_t) &kernel_virt_start_addr) - ELF_TEXT_OFFSET - PAGE_SIZE * 2,
            round_up_po2(pa((uint64_t) &kernel_virt_start_addr) - ELF_TEXT_OFFSET, PAGE_SIZE));
    print_mem_alloc("K_STACK", &kern_stack);

    /* reserve memory area already used to hold the kernel */
    mem_map_region_t kern_elf_file = mem_alloc_region(
            pa((uint64_t) &kernel_virt_start_addr) - ELF_TEXT_OFFSET,
            round_up_po2(pa((uint64_t) &kernel_virt_end_addr), PAGE_SIZE));
    print_mem_alloc("K_ELF", &kern_elf_file);
}

static void paging_setup(uint64_t total_kern_space) {
    /* Calculate space required to hold page table struct to accomodate the entire kernel space */
    uint64_t paging_mem = paging_calc_space_needed(total_kern_space);
    uint64_t pfdb_mem = pageframe_calc_space_needed(paging_mem);

    /* The PML4 table must be aligned on a 4-Kbyte base address - AMD manual section 5.3.2  */
    mem_map_region_t k_pages_struct_rg = mem_alloc_amount(paging_mem, PAGE_SIZE);
    print_mem_alloc("K_PAGE_STR", &k_pages_struct_rg);
    mem_map_region_t k_pfdb_struct_rg = mem_alloc_amount(pfdb_mem, PAGE_SIZE);
    print_mem_alloc("K_PFDB_STR", &k_pfdb_struct_rg);

    /*
     * We have to make sure that the paging structure sits within the first 16MB
     * identity-mapped pages set up before we moved to long mode, otherwise we can't
     * access these locations. Luckily for us, we sorted the e820 return content
     * so that we are likely to find a space that fits in the begining of the
     * physical memory.
     */
    BUG_ON((k_pages_struct_rg.base_addr + k_pages_struct_rg.length) > (16 * 1024 * 1024));
    BUG_ON((k_pfdb_struct_rg.base_addr + k_pfdb_struct_rg.length) > (16 * 1024 * 1024));
    paging_init(kernel_pagetable(), k_pages_struct_rg, k_pfdb_struct_rg);

    /* identity-map all the way to the end kernel text */
    paging_contiguous_map(kernel_pagetable(),
            0,
            pa((uint64_t) &kernel_virt_end_addr),
            K_VIRT_TEXT_ADDR,
            PAGE_PRESENT_BIT | PAGE_READ_WRITE_BIT | PAGE_GLOBAL_BIT);

    /* identity-map area in which the page tables sit, so we can access it later */
    paging_contiguous_map(kernel_pagetable(),
            k_pages_struct_rg.base_addr,
            k_pages_struct_rg.base_addr + k_pages_struct_rg.length,
            va(k_pages_struct_rg.base_addr),
            PAGE_PRESENT_BIT | PAGE_READ_WRITE_BIT | PAGE_GLOBAL_BIT);

}

static void buddy_allocator_setup(void) {
    uint64_t k_mem_content_space = calc_kernel_mem_space();
    uint64_t k_mem_header_space = buddy_calc_header_space(k_mem_content_space);

    /* reserve memory area to be used by the buddy memory allocator */
    mem_map_region_t k_mem_header_rg = mem_alloc_amount(k_mem_header_space, PAGE_SIZE);
    print_mem_alloc("K_BUDDY_H", &k_mem_header_rg);
    mem_map_region_t k_mem_content_rg = mem_alloc_amount(k_mem_content_space, PAGE_SIZE);
    print_mem_alloc("K_BUDDY_C", &k_mem_content_rg);

//    mem_print_entries();

    /* anticipatory paging for kernel space. Demand paging will be used for user space only */
    paging_contiguous_map(kernel_pagetable(),
            k_mem_header_rg.base_addr,
            k_mem_header_rg.base_addr + k_mem_header_rg.length,
            K_VIRT_MEM_HEADER_ADDR,
            PAGE_PRESENT_BIT | PAGE_READ_WRITE_BIT | PAGE_GLOBAL_BIT);

    paging_contiguous_map(kernel_pagetable(),
            k_mem_content_rg.base_addr,
            k_mem_content_rg.base_addr + k_mem_content_rg.length,
            K_VIRT_MEM_CONTENT_ADDR,
            PAGE_PRESENT_BIT | PAGE_READ_WRITE_BIT | PAGE_GLOBAL_BIT);

    /* Reload CR3 with new Paging structure */
    paging_reload_cr3(kernel_pagetable());

    /* initialise kernel memory && kmalloc */
    k_mem_header_rg.base_addr = K_VIRT_MEM_HEADER_ADDR;
    k_mem_content_rg.base_addr = K_VIRT_MEM_CONTENT_ADDR;
    kmem_init(k_mem_header_rg, k_mem_content_rg);
}

void mm_init(void) {
    /* read/prepare entries returned from BIOS 0xe820 */
    mem_init();

    /* reserve regions already used by kernel text and stack */
    reserve_kernel_sections();

    /* allocate and provisiong the paging space required to accomodate entire virt space*/
    paging_setup(K_VIRT_END_ADDR - K_VIRT_START_ADDR);

    /* buddy memory allocator */
    buddy_allocator_setup();

}

