/*
 * string.h
 *
 *  Created on: 16/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_LIBC_STRING_H_
#define INCLUDE_LIBC_STRING_H_

#include "libc/compiler/freestanding.h"

void* memcpy(void *dst, const void *src, size_t size);
void* memset(void *buf, char value, size_t size);
void* memmove(void *dst, void *src, size_t size);
size_t strlen(const char *buf);
void strrev(char *str, size_t length);

#endif /* INCLUDE_LIBC_STRING_H_ */
