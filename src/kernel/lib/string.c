#include "kernel/lib/string.h"
#include "kernel/lib/math.h"

void strrev(char *str, size_t length);

void* memcpy(void *dst, const void *src, size_t size) {

    /*
     objdump -D -M intel  build/kernel/lib/string.o | awk -v RS= '/^[[:xdigit:]]+ <memcpy>/'
     0000000000000000 <memcpy>:
     0:		55                   	push   rbp
     1:		48 89 e5             	mov    rbp,rsp
     4:		48 83 ec 38          	sub    rsp,0x38
     8:		48 89 7d d8          	mov    QWORD PTR [rbp-0x28],rdi
     c:		48 89 75 d0          	mov    QWORD PTR [rbp-0x30],rsi
     10:	48 89 55 c8          	mov    QWORD PTR [rbp-0x38],rdx
     14:	48 8b 45 c8          	mov    rax,QWORD PTR [rbp-0x38]
     18:	48 c1 e8 03          	shr    rax,0x3
     1c:	48 89 c1             	mov    rcx,rax
     1f:	48 8b 45 c8          	mov    rax,QWORD PTR [rbp-0x38]
     23:	83 e0 07             	and    eax,0x7
     26:	49 89 c0             	mov    r8,rax
     29:	48 8b 55 d8          	mov    rdx,QWORD PTR [rbp-0x28]
     2d:	48 8b 45 d0          	mov    rax,QWORD PTR [rbp-0x30]
     31:	48 89 d7             	mov    rdi,rdx
     34:	48 89 c6             	mov    rsi,rax
     37:	f3 48 a5             	rep movs QWORD PTR es:[rdi],QWORD PTR ds:[rsi]
     3a:	49 89 c8             	mov    r8,rcx
     3d:	f3 a4                	rep movs BYTE PTR es:[rdi],BYTE PTR ds:[rsi]
     3f:	48 89 f0             	mov    rax,rsi
     42:	48 89 fa             	mov    rdx,rdi
     45:	48 89 4d f8          	mov    QWORD PTR [rbp-0x8],rcx
     49:	48 89 55 f0          	mov    QWORD PTR [rbp-0x10],rdx
     4d:	48 89 45 e8          	mov    QWORD PTR [rbp-0x18],rax
     51:	48 8b 45 d8          	mov    rax,QWORD PTR [rbp-0x28]
     55:	c9                   	leave
     56:	c3                   	ret

     */

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

void* memset(void *buf, char value, size_t size) {
    asm volatile(
            "rep stosb \n\t"
            : "=&D" (buf)
            : "a" (value), "c"(size)
            : "memory"
    );
    return buf;
}

/**
 * the radix values can be OCTAL, DECIMAL, or HEX
 */
char* lltoa(long long int value, char *str, int radix) {
    // Check for supported base.
    if (radix != 8 && radix != 10 && radix != 16) {
        *str = '\0';
        return str;
    }

    int i = 0;
    bool is_negative = false;

    /* handle edge case */
    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    /* negative numbers are handled only with base 10 */
    if (value < 0 && radix == 10) {
        is_negative = true;
        value = abs(value);
    }

    while (value != 0) {
        int rem = value % radix;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        value = value / radix;
    }

    if (is_negative)
        str[i++] = '-';

    str[i] = '\0';
    strrev(str, i);

    return str;
}

char* ulltoa(unsigned long long value, char *str, int radix) {
    // check for supported base.
    if (radix != 8 && radix != 10 && radix != 16) {
        *str = '\0';
        return str;
    }

    int i = 0;

    /* handle edge case */
    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    while (value != 0) {
        int rem = value % radix;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        value = value / radix;
    }

    str[i] = '\0';
    strrev(str, i);

    return str;
}

size_t strlen(const char *buf) {
    /* like libC strlen, we don't count NUL-terminator */
    size_t len = 0;
    for (; *(buf + len) != '\0'; len++)
        ;
    return len;
}

void strrev(char *str, size_t length) {
    /* str is expected to be NUL-terminated */
    if (length < 2)
        return;

    for (size_t start = 0, end = length - 1; start < length / 2; start++, end--) {
        char tmp = *(str + start);
        *(str + start) = *(str + end);
        *(str + end) = tmp;
    }
}

