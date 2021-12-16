/*
 * memmove.c
 *
 *  Created on: 16/12/2021
 *      Author: Paulo Almeida
 */

#include "libc/string.h"

void* memmove(void *dst, void *src, size_t size) {
    char *t_dst = dst;
    char *t_src = src;

    /* sanity check */
    if (t_src == t_dst)
        return dst;

    memcpy(t_dst, t_src, size);

    if ((t_dst > t_src && (t_src + size) < t_dst) || (t_src > t_dst && (t_dst + size) < t_src)) {
        /* they don't interset */
        memset(t_src, 0, size);
    } else if (t_dst > t_src) {
        memset(t_src, 0, size - (t_dst - t_src));
    } else {
        memset(t_src + ((t_src + size) - (t_dst + size)), 0, (t_src + size) - (t_dst + size));
    }

    return dst;
}
