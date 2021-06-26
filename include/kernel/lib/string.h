#ifndef _KERNEL_LIB_STRING_H
#define _KERNEL_LIB_STRING_H

#include <stddef.h>

void* memcpy(void* dst, void* src, size_t size);
void* memset(void* buf, char value, size_t size);

#endif
