/*
 * string.h
 *
 *  Created on: Jun 30, 2021
 *      Author: Paulo Almeida
 */

#ifndef _KERNEL_LIB_STRING_H
#define _KERNEL_LIB_STRING_H

#include "kernel/compiler/freestanding.h"

/* C standard functions */
void* memcpy(void *dst, const void *src, size_t size);
void* memset(void *buf, char value, size_t size);
size_t strlen(const char *buf);

/* non-standard functions (although commonly used by other compilers) */
void* memzero(void *dst, size_t size);
char* lltoa(long long int value, char *str, int radix);
char* ulltoa(unsigned long long value, char *str, int radix);
void strrev(char *str, size_t length);

#endif /* _KERNEL_LIB_PRINTK_H */
