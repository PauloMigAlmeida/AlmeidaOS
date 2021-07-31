/*
 * qsort.c
 *
 *  Created on: Jul 30, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/lib/qsort.h"
#include "kernel/compiler/macro.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/shuffle.h"
#include "kernel/lib/printk.h"
#include "kernel/lib/string.h"


static size_t partition(void *base, size_t item_width, qsort_cmp_fun cmp, size_t lo, size_t hi) {
    /* repeat until i and j pointer cross */
    size_t i = lo, j = hi + 1;
    char *base_ch = (char*) base;

    while (true) {

        /* scan i from left to right so long as (a[i] < a[lo]) */
        while (cmp(base_ch + (++i * item_width), base_ch + (lo * item_width)) < 0)
            if (i == hi) break;

        /* scan j from right to left so long as (a[j] > a[lo]) */
        while (cmp(base_ch + (lo * item_width), base_ch + (--j * item_width)) < 0)
            if (j == lo) break;

        if (i >= j) break;
        EXCH(base_ch + i * item_width, base_ch + j * item_width, item_width);
    }

    /* when pointers cross -> exchange a[lo] with a[j] */
    EXCH(base_ch + lo * item_width, base_ch + j * item_width, item_width);
    return j;
}

static void sort(void *base, size_t item_width, qsort_cmp_fun cmp, size_t lo, size_t hi) {
    /* recursion base case (also a sanity check tbh */
    if (hi <= lo) return;

    size_t j = partition(base, item_width, cmp, lo, hi);

    /* sanity check as we are using size_t instead of a signed int type */
    if (j > 0)
        sort(base, item_width, cmp, lo, j - 1);

    sort(base, item_width, cmp, j + 1, hi);
}

void qsort(void *base, size_t num_elems, size_t item_width, qsort_cmp_fun cmp) {
    /* sanity checks */
    if (num_elems < 2) return;

    /* shuffle - avoids the worst case runtime to a negligible likelihood */
    shuffle(base, num_elems, item_width);

    sort(base, item_width, cmp, 0, num_elems - 1);
}
