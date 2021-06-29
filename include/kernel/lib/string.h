#ifndef _KERNEL_LIB_STRING_H
#define _KERNEL_LIB_STRING_H

#include <stddef.h>

void* memcpy(void* dst, void* src, size_t size);
void* memset(void* buf, char value, size_t size);

/* these are both non-standard functions (although pretty
 * commonly used by compilers on other platforms)
 * */
char* itoa(int value, char* buf, int radix);

size_t strlen(const char* buf);
#endif /* _KERNEL_LIB_PRINTK_H */
