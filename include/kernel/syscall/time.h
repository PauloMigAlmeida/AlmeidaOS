/*
 * time.h
 *
 *  Created on: 16/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_SYSCALL_TIME_H_
#define INCLUDE_KERNEL_SYSCALL_TIME_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/sys/types.h"

/*
 * POSIX defines this function's signature as:
 *
 *      time_t time(time_t *tloc);
 *
 * It turns out that the tloc argument is obsolescent and should always
 * be NULL in new code. When tloc is NULL, the call cannot fail.
 *
 * Given that fact that I don't have to support old application nor
 * POSIX for what it matters, I won't stick to this rule this time.
 */

time_t sys_time(void);

#endif /* INCLUDE_KERNEL_SYSCALL_TIME_H_ */
