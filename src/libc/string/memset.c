/*
 * memset.c
 *
 *  Created on: 16/12/2021
 *      Author: Paulo Almeida
 */

#include "libc/string.h"

void* memset(void *buf, char value, size_t size) {
    asm volatile(
            "rep stosb \n\t"
            : "=&D" (buf)
            : "a" (value), "c"(size)
            : "memory"
    );
    return buf;
}
