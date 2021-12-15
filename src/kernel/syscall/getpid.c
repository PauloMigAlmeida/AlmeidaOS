/*
 * getpid.c
 *
 *  Created on: 15/12/2021
 *      Author: Paulo Almeida
 */

#include "kernel/syscall/getpid.h"
#include "kernel/task/scheduler.h"

pid_t sys_getpid(void) {
    /* TODO: I wonder if I should disable interrupts before get it */
    return this_rq()->curr->pid;
}
