/*
 * syscall.c
 *
 *  Created on: 12/12/2021
 *      Author: Paulo Almeida
 */

#include "libc/internals/syscall.h"
#include "libc/compiler/freestanding.h"

long syscall(long nr_number, ...) {
    va_list args;
    va_start(args, nr_number);

    uint64_t rax = nr_number;
    uint64_t rdi = va_arg(args, uint64_t);
    uint64_t rsi = va_arg(args, uint64_t);
    uint64_t rdx = va_arg(args, uint64_t);
    register uint64_t r10 asm("r10") = va_arg(args, uint64_t);
    register uint64_t r8 asm("r8") = va_arg(args, uint64_t);
    register uint64_t r9 asm("r9") = va_arg(args, uint64_t);

    va_end(args);

    asm volatile (
            "syscall"
            : "=a" (rax)
            : "a"(rax), "D"(rdi), "S"(rsi), "d"(rdx), "r"(r10), "r"(r8), "r"(r9)
            : "memory"
    );

    return rax;

}
