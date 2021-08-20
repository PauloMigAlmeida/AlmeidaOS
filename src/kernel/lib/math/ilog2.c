/*
 * ilog2.c
 *
 *  Created on: 20/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/lib/math.h"

#define ROUND(v, a, w)                      \
    do {                                    \
        if (v & (((1 << w) - 1) << w)) {    \
            a  += w;                        \
            v >>= w;                        \
        }                                   \
    } while (0)

/*
 * log2 is essentially about counting the number of leading zeros
 * Credits: https://github.com/ncoden/NASM/blob/master/ilog2.c
 */
int ilog2(uint64_t vv) {
    int p = 0;
    uint32_t v;

    v = vv >> 32;
    if (v)
        p += 32;
    else
        v = vv;

    ROUND(v, p, 16);
    ROUND(v, p, 8);
    ROUND(v, p, 4);
    ROUND(v, p, 2);
    ROUND(v, p, 1);

    return p;
}

