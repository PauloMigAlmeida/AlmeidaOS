/*
 * getpid.c
 *
 *  Created on: 15/12/2021
 *      Author: Paulo Almeida
 */

#include "kernel/syscall/getpid.h"
#include "kernel/task/scheduler.h"
#include "kernel/asm/generic.h"

pid_t sys_getpid(void) {
    pid_t ret = this_rq()->curr->pid;
    return ret;
}
