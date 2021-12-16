/*
 * memcpy.c
 *
 *  Created on: 16/12/2021
 *      Author: Paulo Almeida
 */

#include "libc/string.h"

void* memcpy(void *dst, const void *src, size_t size) {

    long d0, d1, d2;
    asm volatile(
            "rep movsq \n\t"
            "movq rcx, %[remainder] \n\t"
            "rep movsb \n\t"
            : "=&c" (d0), "=&D" (d1), "=&S" (d2)
            : "0" (size >> 3), [remainder] "g" (size & 7), "1" (dst), "2" (src)
            : "memory"
    );

    return dst;
}
