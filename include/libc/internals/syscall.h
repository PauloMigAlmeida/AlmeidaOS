/*
 * syscall.h
 *
 *  Created on: 12/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_LIBC_INTERNAL_SYSCALL_H_
#define INCLUDE_LIBC_INTERNAL_SYSCALL_H_

/* functions */
long syscall(long nr_number, ...);

/* utility macros */
#define syscall1(nr_number, arg1)                               syscall(nr_number, arg1, 0, 0, 0, 0, 0)
#define syscall2(nr_number, arg1, arg2)                         syscall(nr_number, arg1, arg2, 0, 0, 0, 0)
#define syscall3(nr_number, arg1, arg2, arg3)                   syscall(nr_number, arg1, arg2, arg3, 0, 0, 0)
#define syscall4(nr_number, arg1, arg2, arg3, arg4)             syscall(nr_number, arg1, arg2, arg3, arg4, 0, 0)
#define syscall5(nr_number, arg1, arg2, arg3, arg4, arg5)       syscall(nr_number, arg1, arg2, arg3, arg4, arg5, 0)
#define syscall6(nr_number, arg1, arg2, arg3, arg4, arg5, arg6) syscall(nr_number, arg1, arg2, arg3, arg4, arg5, arg6)

/* syscall IDs */
#define NR_SYS_READ     0
#define NR_SYS_WRITE    1

#endif /* INCLUDE_LIBC_INTERNAL_SYSCALL_H_ */
