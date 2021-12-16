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
    disable_interrupts();
    pid_t ret = this_rq()->curr->pid;
    enable_interrupts();
    return ret;
}
