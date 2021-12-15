/*
 * getpid.c
 *
 *  Created on: 15/12/2021
 *      Author: Paulo Almeida
 */

#include "libc/unistd.h"
#include "libc/internals/syscall.h"

pid_t getpid(void) {
    return (pid_t)syscall0(__NR_getpid);
}

