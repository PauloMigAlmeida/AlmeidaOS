/*
 * write.c
 *
 *  Created on: 11/12/2021
 *      Author: Paulo Almeida
 */

#include "libc/unistd.h"
#include "libc/internals/syscall.h"

long write(const char *string, size_t length) {
    return syscall2(1, string, length);
}
