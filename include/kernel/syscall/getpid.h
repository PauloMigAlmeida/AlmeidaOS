/*
 * getpid.h
 *
 *  Created on: 15/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_SYSCALL_GETPID_H_
#define INCLUDE_KERNEL_SYSCALL_GETPID_H_

#include "kernel/sys/types.h"

/* returns the process ID of the current process */
pid_t sys_getpid(void);

#endif /* INCLUDE_KERNEL_SYSCALL_GETPID_H_ */
