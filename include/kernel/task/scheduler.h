/*
 * scheduler.h
 *
 *  Created on: 15/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_TASK_SCHEDULER_H_
#define INCLUDE_KERNEL_TASK_SCHEDULER_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/task/process.h"
#include "kernel/interrupt/idt.h" // move trapframe to a separate file

struct task_list_t {
    /* a task in the linked list */
    task_struct_t *val;
    /* next node */
    struct task_list_t *next;
};

/* If we ever support SMP processors, this will have to tweaked */
typedef struct {

    /* current process running */
    task_struct_t *curr;

    /* linked-list of process that want to execute in this RQ */
    struct task_list_t *tasks;

    /* wether or not context switch is required soon */
    bool need_resched;

} sched_run_queue_t;

/* making it easier for SMP implementation :-) */
sched_run_queue_t* this_rq(void);

/* invoked every timer interrupt */
void scheduler_tick(void);

/* initialise scheduler */
void scheduler_init(task_struct_t *init_proc);

/* add new process to the scheduler */
void scheduler_add(task_struct_t *task);

/* choose which process to run next */
void schedule(interrupt_stack_frame_t *int_frame);

#endif /* INCLUDE_KERNEL_TASK_SCHEDULER_H_ */
