/*
 * process.h
 *
 *  Created on: 2/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_TASK_PROCESS_H_
#define INCLUDE_KERNEL_TASK_PROCESS_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/sys/types.h"
#include "kernel/mm/pagetable.h"

#include "kernel/arch/cpu_registers.h"
#include "kernel/interrupt/idt.h"

/* Task states */
#define TASK_RUNNING            0
#define TASK_INTERRUPTIBLE      1
#define TASK_UNINTERRUPTIBLE    2
#define TASK_ZOMBIE             3
#define TASK_STOPPED            4

typedef struct {

    /* intial virtual address */
    uint64_t ini_addr;

    /* final virtual address */
    uint64_t fini_addr;

    /* reference to process' page table */
    pagetable_t pgtable;

} mm_vm_area_t;

typedef struct {

    /* physical address */
    uint64_t phys_addr;

    /* virtual address */
    uint64_t virt_addr;

    /* length */
    uint64_t length;

} stack_area_t;

typedef struct {

    /* process identification */
    pid_t pid;

    /* process scheduling state */
    int state;

    /* virtual memory related info */
    mm_vm_area_t vm_area;

    /* address of the stack used by process */
    stack_area_t task_stack_area;

    /* address of the stack used by kernel */
    stack_area_t kernel_stack_area;

    /* save/restore data to return from a context switch*/
    registers_64_t regs;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;

} task_struct_t;

task_struct_t* create_process(uint64_t text_phy_addr);
void launch_process(task_struct_t *task);
void process_context_swtich(interrupt_stack_frame_t *int_frame, task_struct_t *curr, task_struct_t *next);

#endif /* INCLUDE_KERNEL_TASK_PROCESS_H_ */
