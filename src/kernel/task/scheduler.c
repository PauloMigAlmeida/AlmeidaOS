/*
 * scheduler.c
 *
 *  Created on: 15/12/2021
 *      Author: paulo
 */

#include "kernel/task/scheduler.h"
#include "kernel/compiler/bug.h"

static bool initialised = false;
static sched_run_queue_t run_queue = { 0 };

sched_run_queue_t* this_rq(void) {
    /* sanity check */
    BUG_ON(!initialised);

    return &run_queue;
}

void scheduler_init(task_struct_t* init_proc) {
    initialised = true;
    run_queue.curr = init_proc;
}

void scheduler_tick(void) {
    /* PIT starts before scheduler during boot */
    if(!initialised)
        return;
}
