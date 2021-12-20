/*
 * scheduler.c
 *
 *  Created on: 15/12/2021
 *      Author: paulo
 */

#include "kernel/task/scheduler.h"
#include "kernel/compiler/bug.h"
#include "kernel/mm/kmem.h"
#include "kernel/time/jiffies.h"
#include "kernel/lib/string.h"

static bool initialised = false;
static sched_run_queue_t run_queue;

/* making it easier for SMP implementation :-) */
sched_run_queue_t* this_rq(void) {
    return &run_queue;
}

void scheduler_init(task_struct_t *init_proc) {
    /* sanity checks */
    BUG_ON(run_queue.tasks != NULL);

    /* alloc space for the linked list node */
    struct task_list_t *head = kmalloc(sizeof(struct task_list_t), KMEM_DEFAULT);
    head->val = init_proc;
    head->next = NULL;

    /* add node to the list of processes to be scheduled */
    run_queue.tasks = head;

    initialised = true;
}

void scheduler_tick(void) {
    /* PIT starts before scheduler during boot */
    if (!initialised)
        return;

    /* keep the CPU occupied if it's not yet so */
    if (this_rq()->curr == NULL || jiffies % 50 == 0)
        this_rq()->need_resched = true;

}

static void move_end_of_list(struct task_list_t *element) {
    struct task_list_t *head = this_rq()->tasks;

    if (head->next == NULL) {
        head->next = element;
        element->next = NULL;
    } else {
        struct task_list_t *tmp = this_rq()->tasks;
        /* find list's tail */
        while (tmp != NULL) {
            if (tmp->next == NULL)
                break;
            tmp = tmp->next;
        }

        tmp->next = element;
        this_rq()->tasks = element->next;
        element->next = NULL;
    }

}

void scheduler_add(task_struct_t *task) {
    /* alloc space for the linked list node */
    struct task_list_t *node = kmalloc(sizeof(struct task_list_t), KMEM_DEFAULT);
    node->val = task;
    node->next = NULL;

    /* add node to the list of processes to be scheduled */
    move_end_of_list(node);
}

void schedule(interrupt_stack_frame_t *int_frame) {
    /* sanity check */
    BUG_ON(!initialised);

    struct task_list_t *head = this_rq()->tasks;

    /* fail-fast if there is nothing else to run */
    if ((this_rq()->curr != NULL && head->next == NULL) || head == NULL)
        return;

    task_struct_t *curr = this_rq()->curr;
    task_struct_t *next = head->val;

    /* puts current process at the end of list to
     * give a change for the other tasks to run */
    move_end_of_list(head);

    /* select process to be executed */
    this_rq()->curr = next;
    this_rq()->need_resched = false;

    /* switch context */
    process_context_swtich(int_frame, curr, next);

}
