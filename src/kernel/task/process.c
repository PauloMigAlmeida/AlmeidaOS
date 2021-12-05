/*
 * process.c
 *
 *  Created on: 2/12/2021
 *      Author: Paulo Almeida
 */

#include "kernel/task/process.h"
#include "kernel/lib/string.h"
#include "kernel/mm/kmem.h"
#include "kernel/mm/page.h"
#include "kernel/mm/addressconv.h"

/**
 * text_phy_addr: should container the address of the start of the text section
 *          of the executable to be launched
 */
task_struct_t* create_process(uint64_t text_phy_addr) {
    /**
     * Allocate Space:
     *  -> text code :: The stack references will be relative to the ini_addr, so these pages must also be mapped
     *              The problem here is that it should work differently depending on whether this is already in memory
     *              or if we should load it first.
     *
     *              In the case of an executable already in memory (using IO blocks) then we need to map it to the existing
     *              utilised space. (Most likely the first approach/attempt)
     *
     *              In the case of a new file, we need to allocate the space dynamically and then load it from the filesystem/disk
     *              (Most likely the right way of doing things but it requires implementing a FS and interfacing with the HD)
     *
     *  -> pagetable :: where the pages will be stored really...no biggie
     *      -> I think I need a pageframedb struct here so I can initialise the paging structure properly -> Done
     *      -> ALERT: The paging_init() function will fail as it is because it tries to memzero the area using the phys_addr which
     *          isn't mapped in kernel's space..... I need to figure out an alternative here -> Done
     *      -> Temp: Map video dma 0xb8000 to test whether user process is running
     *      -> Need to pave the way to jump to ring 3 in C
     *      -> ALERT: Revist how pageframe requirements is calculated...I keep getting low numbers
     *
     *  ->
     */

    task_struct_t *task = kmalloc(sizeof(task_struct_t), KMEM_DEFAULT);
    task->pid = 1; // TODO: create routine to get free PID
    task->vm_area.ini_addr = 0x0;
    task->vm_area.fini_addr = 0x100000 * 10;

    /* calculate space required to accomodate user's process memory area */
    uint64_t pgt_spc_req = paging_calc_space_needed(task->vm_area.fini_addr - task->vm_area.ini_addr);
    uint64_t pfdb_spc_req = pageframe_calc_space_needed(pgt_spc_req);

    /* init pagetable and pageframe database structures */
    mem_map_region_t u_pages_struct_rg = {
            .base_addr = pa((uint64_t) kmalloc(pgt_spc_req, KMEM_DEFAULT)),
            .length = pgt_spc_req
    };
    mem_map_region_t u_pfdb_struct_rg = {
            .base_addr = pa((uint64_t) kmalloc(pfdb_spc_req, KMEM_DEFAULT)),
            .length = pfdb_spc_req
    };

    paging_init(&task->vm_area.pgtable, u_pages_struct_rg, u_pfdb_struct_rg);
    paging_contiguous_map(&task->vm_area.pgtable,
            text_phy_addr, text_phy_addr + 0x8000, 0x40000, PAGE_STD_BITS | PAGE_USER_SUPERVISOR_BIT);

    /* Temp: Map video DMA so I can test user program */
    paging_contiguous_map(&task->vm_area.pgtable,
                0xb8000, 0xb8000 + 0x1000, 0xb8000, PAGE_STD_BITS | PAGE_USER_SUPERVISOR_BIT);

    /* Copy kernel space PML4 entries to this process' page table */

    memcpy((uintptr_t*) (task->vm_area.pgtable.virt_root + (256 * sizeof(uint64_t))),
            (uintptr_t*) (kernel_pagetable()->virt_root + (256 * sizeof(uint64_t))),
            256 * sizeof(uint64_t));

    paging_reload_cr3(&task->vm_area.pgtable);

    return task;
}
