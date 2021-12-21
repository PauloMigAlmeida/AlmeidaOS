/*
 * process.c
 *
 *  Created on: 2/12/2021
 *      Author: Paulo Almeida
 */

#include "kernel/task/process.h"
#include "kernel/lib/string.h"
#include "kernel/mm/init.h"
#include "kernel/mm/kmem.h"
#include "kernel/mm/page.h"
#include "kernel/mm/addressconv.h"
#include "kernel/arch/tss.h"
#include "kernel/task/pid.h"
#include "kernel/syscall/init.h"
#include "kernel/lib/printk.h"
#include "kernel/arch/gdt_segments.h"

extern tss_t TSS64_Segment;

/**
 * text_phy_addr: should container the address of the start of the text section
 *          of the executable to be launched
 */
task_struct_t* create_process(uint64_t text_phy_addr) {
    /**
     * Notes to myself:
     *
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
     */

    task_struct_t *task = kmalloc(sizeof(task_struct_t), KMEM_DEFAULT);
    task->pid = find_free_pid();
    task->state = TASK_RUNNING;
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

    /* init paging structure for the process */
    printk_fine("u_pages_struct_rg: %.16llx u_pfdb_struct_rg: %.16llx", u_pages_struct_rg.base_addr, u_pfdb_struct_rg.base_addr);
    paging_init(&task->vm_area.pgtable, u_pages_struct_rg, u_pfdb_struct_rg);

    // TODO: this should be dynamic once we start loading files from disk
    uint64_t elf_prog_size = 0x8000;

    /* mapping program's physical location to process' pgtable */
    paging_contiguous_map(&task->vm_area.pgtable,
            text_phy_addr,
            text_phy_addr + elf_prog_size,
            0x40000,
            PAGE_STD_BITS | PAGE_USER_SUPERVISOR_BIT);

    /* mapping process' stack physical location to process' pgtable */
    task->task_stack_area.length = STACK_SIZE;
    task->task_stack_area.virt_addr = (uint64_t) kmalloc(task->task_stack_area.length, KMEM_DEFAULT | KMEM_ZERO);
    task->task_stack_area.phys_addr = pa(task->task_stack_area.virt_addr);

    paging_contiguous_map(&task->vm_area.pgtable,
            task->task_stack_area.phys_addr,
            task->task_stack_area.phys_addr + task->task_stack_area.length,
            0x3e000,
            PAGE_STD_BITS | PAGE_USER_SUPERVISOR_BIT);

    /* allocate stack for kernel  */
    task->kernel_stack_area.length = STACK_SIZE;
    task->kernel_stack_area.virt_addr = (uint64_t) kmalloc(task->kernel_stack_area.length, KMEM_DEFAULT | KMEM_ZERO);
    task->kernel_stack_area.phys_addr = pa(task->kernel_stack_area.virt_addr);

    /* in the future we should read this info from the ELF headers */
    task->rip = 0x41000;
    task->rsp = 0x40000;
    task->rflags = 0x202;
    task->cs = GDT64_SEGMENT_SELECTOR_USER_CODE | DPL_RING_3;
    task->ss = GDT64_SEGMENT_SELECTOR_USER_DATA | DPL_RING_3;
    memzero(&task->regs, sizeof(registers_64_t));

    /* Copy PML4  entries for kernel space (higher-half entries) to this process' page table */
    memcpy((uintptr_t*) (task->vm_area.pgtable.virt_root + (256 * sizeof(uint64_t))),
            (uintptr_t*) (kernel_pagetable()->virt_root + (256 * sizeof(uint64_t))),
            256 * sizeof(uint64_t));

    return task;
}

void launch_process(task_struct_t *task) {
    TSS64_Segment.rsp0 = task->kernel_stack_area.virt_addr + STACK_SIZE;
    paging_reload_cr3(&task->vm_area.pgtable);
}

void process_context_swtich(interrupt_stack_frame_t *int_frame, task_struct_t *curr, task_struct_t *next) {
    /* software context - save current task's context and load next task's context*/
    if (curr == NULL) {
        int_frame->regs = next->regs;

        int_frame->ss = next->ss;
        int_frame->cs = next->cs;
        int_frame->rflags = next->rflags;
        int_frame->rsp = next->rsp;
        int_frame->rip = next->rip;

        launch_process(next);
    } else {
        curr->rsp = int_frame->rsp;
        curr->rip = int_frame->rip;
        curr->ss = int_frame->ss;
        curr->cs = int_frame->cs;
        curr->regs = int_frame->regs;
        curr->rflags = int_frame->rflags;

        int_frame->rsp = next->rsp;
        int_frame->rip = next->rip;
        int_frame->regs = next->regs;
        int_frame->ss = next->ss;
        int_frame->cs = next->cs;
        int_frame->rflags = next->rflags;

        launch_process(next);
    }
}

