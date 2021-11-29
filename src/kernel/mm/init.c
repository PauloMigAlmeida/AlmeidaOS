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

static void print_mem_alloc(char *desc, mem_map_region_t *region) {
    printk_info("[%s]: start: 0x%llx end: 0x%llx length (Kb): %llu", desc,
            region->base_addr,
            region->base_addr + region->length,
            region->length / 1024);
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

static mem_map_region_t buddy_allocator_setup(uint64_t k_mem_content_space) {
    uint64_t k_mem_header_space = buddy_calc_header_space(k_mem_content_space);

    /* reserve memory area to be used by the buddy memory allocator */
    mem_map_region_t k_mem_header_rg = mem_alloc_amount(k_mem_header_space, PAGE_SIZE);
    print_mem_alloc("K_BUDDY_H", &k_mem_header_rg);

    /*
     * Notes to myself:
     *  I'm attempting to place the entire memory available under a single buddy allocation
     *  system, so it will be easier to ask for mem space when setting up user mm (umem).
     */
    mem_map_region_t k_mem_content_rg = { .base_addr = 0x0, .length = k_mem_content_space };
    print_mem_alloc("K_BUDDY_C", &k_mem_content_rg);

    mem_print_entries();

    /* initialise kernel memory && kmalloc */
    k_mem_header_rg.base_addr = va(k_mem_header_rg.base_addr);
    kmem_init(k_mem_header_rg, k_mem_content_rg);
    printk_info("Memory allocation system initialised");

    return k_mem_header_rg;
}

static void paging_setup(uint64_t total_kern_space) {
    /* Calculate space required to hold page table struct to accomodate the entire kernel space */
    uint64_t paging_mem = paging_calc_space_needed(total_kern_space);

    /* linked list that store references of free/used pages */
    uint64_t pfdb_mem = pageframe_calc_space_needed(paging_mem);

    /* The PML4 table must be aligned on a 4-Kbyte base address - AMD manual section 5.3.2  */
    mem_map_region_t k_pages_struct_rg = {
            .base_addr = (uintptr_t) kmalloc(paging_mem, KMEM_RAW_ALLOC),
            .length = paging_mem
    };
    print_mem_alloc("K_PAGE_STR", &k_pages_struct_rg);
    mem_map_region_t k_pfdb_struct_rg = {
            .base_addr = (uintptr_t) kmalloc(pfdb_mem, KMEM_RAW_ALLOC),
            .length = pfdb_mem
    };
    print_mem_alloc("K_PFDB_STR", &k_pfdb_struct_rg);

    paging_init(kernel_pagetable(), k_pages_struct_rg, k_pfdb_struct_rg);

    /* identity-map all the way to the end kernel text */
    paging_contiguous_map(kernel_pagetable(),
            0,
            pa((uint64_t) &kernel_virt_end_addr),
            K_VIRT_START_ADDR,
            PAGE_PRESENT_BIT | PAGE_READ_WRITE_BIT | PAGE_GLOBAL_BIT);

    /* identity-map area in which the page tables sit, so we can access it later */
    paging_contiguous_map(kernel_pagetable(),
            k_pages_struct_rg.base_addr,
            k_pages_struct_rg.base_addr + k_pages_struct_rg.length,
            va(k_pages_struct_rg.base_addr),
            PAGE_PRESENT_BIT | PAGE_READ_WRITE_BIT | PAGE_GLOBAL_BIT);

    paging_contiguous_map(kernel_pagetable(),
            k_pfdb_struct_rg.base_addr,
            k_pfdb_struct_rg.base_addr + k_pfdb_struct_rg.length,
            va(k_pfdb_struct_rg.base_addr),
            PAGE_PRESENT_BIT | PAGE_READ_WRITE_BIT | PAGE_GLOBAL_BIT);

    printk_info("Paging initialised");
}

void mm_init(void) {
    /* read/prepare entries returned from BIOS 0xe820 */
    mem_init();

    /* reserve regions already used by kernel text and stack */
    reserve_kernel_sections();

    /* obtain how much physical memory is available */
    uint64_t total_kern_space = flp2(mem_stat().phys_avail_mem);

    /* buddy memory allocator */
    mem_map_region_t k_mem_header_rg = buddy_allocator_setup(total_kern_space);

    /* allocate and provisiong the paging space required to accomodate entire kernel virt space*/
    paging_setup(total_kern_space);

    /* identity-map area in which the page tables sit, so we can access it later */
    paging_contiguous_map(kernel_pagetable(),
            pa(k_mem_header_rg.base_addr),
            pa(k_mem_header_rg.base_addr) + k_mem_header_rg.length,
            k_mem_header_rg.base_addr,
            PAGE_PRESENT_BIT | PAGE_READ_WRITE_BIT | PAGE_GLOBAL_BIT);

    /* Reload CR3 with new Paging structure */
    paging_reload_cr3(kernel_pagetable());
}

