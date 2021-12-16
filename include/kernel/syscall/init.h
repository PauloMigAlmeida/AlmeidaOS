/*
 * init.h
 *
 *  Created on: 13/11/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_SYSCALL_INIT_H_
#define INCLUDE_KERNEL_SYSCALL_INIT_H_

#include "kernel/compiler/freestanding.h"

/* functions */
void syscall_init(void);
extern void syscall_jump_usermode(uint64_t start_addr);

/* syscall IDs */
#define __NR_read     0
#define __NR_write    1
#define __NR_getpid   39
#define __NR_time     201


#endif /* INCLUDE_KERNEL_SYSCALL_INIT_H_ */
