/*
 * shuffle.c
 *
 *  Created on: Jul 30, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/lib/shuffle.h"
#include "kernel/lib/printk.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/math.h"
#include "kernel/lib/string.h"
#include "kernel/compiler/macro.h"
#include "kernel/compiler/bug.h"

/* this function is used during paging set up, so kmalloc isn't an option here ;) */
void shuffle(void *arr, size_t arr_length, size_t item_width) {

    /* sanity checks for the edge cases */
    BUG_ON(item_width == 0);

    if (arr_length < 2) return; /* nothing to shuffle here */

    /* C doesn't allow void pointer arithmetic as it doesn't know the size of 'increment' by */
    char *arr_cst = (char*) arr;

    for (size_t i = 0; i < arr_length; i++) {
        size_t r = (((size_t) rand()) % (arr_length - 1)) + 1;

        EXCH(arr_cst + i * item_width, arr_cst + r * item_width, item_width);
    }
}
