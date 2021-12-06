/*
 * init.h
 *
 *  Created on: 13/11/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_SYSCALL_INIT_H_
#define INCLUDE_KERNEL_SYSCALL_INIT_H_

#include "kernel/compiler/freestanding.h"

void syscall_init(void);
extern void interrupt_jump_usermode(uint64_t start_addr);
extern void syscall_jump_usermode(uint64_t start_addr);


#endif /* INCLUDE_KERNEL_SYSCALL_INIT_H_ */
