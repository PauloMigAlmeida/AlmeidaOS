/*
 * time.c
 *
 *  Created on: 16/12/2021
 *      Author: Paulo Almeida
 */

#include "libc/unistd.h"
#include "libc/internals/syscall.h"

time_t time(void) {
    return (time_t) syscall0(__NR_time);
}
