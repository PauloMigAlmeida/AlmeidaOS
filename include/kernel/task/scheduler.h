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


/* If we ever support SMP processors, this will have to tweaked */
typedef struct {

    /* current process running */
    task_struct_t *curr;

} sched_run_queue_t;

/* making it easier for SMP implementation :-) */
sched_run_queue_t* this_rq(void);

/* invoked every timer interrupt */
void scheduler_tick(void);

/* initialise scheduler */
void scheduler_init(task_struct_t* init_proc);

#endif /* INCLUDE_KERNEL_TASK_SCHEDULER_H_ */
