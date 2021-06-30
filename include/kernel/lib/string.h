#ifndef _KERNEL_LIB_STRING_H
#define _KERNEL_LIB_STRING_H

#include <stddef.h>

/* C standard functions */
void* memcpy(void* dst, void* src, size_t size);
void* memset(void* buf, char value, size_t size);
size_t strlen(const char* buf);

/* non-standard functions (although commonly used by other compilers) */
char* itoa(int value, char* buf, int radix);
void strrev(char* str, size_t length);

#endif /* _KERNEL_LIB_PRINTK_H */
