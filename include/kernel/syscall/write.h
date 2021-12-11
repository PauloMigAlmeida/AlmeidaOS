/*
 * write.h
 *
 *  Created on: 12/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_SYSCALL_WRITE_H_
#define INCLUDE_KERNEL_SYSCALL_WRITE_H_

#include "kernel/compiler/freestanding.h"

size_t sys_write(const char *string, size_t length);

#endif /* INCLUDE_KERNEL_SYSCALL_WRITE_H_ */
