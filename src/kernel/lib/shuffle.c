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

/* this function is used durimg memory set up, so kmalloc isn't an option here ;) */
void shuffle(void *arr, size_t arr_length, size_t item_width) {
    char tmp_buf[128];

    /* sanity checks for the edge cases */
    if (arr_length == 0 || item_width == 0) {
        //TODO create a mechanism that resembles Linux's BUG_ON macro
        printk_error("arr_length and/or item_width can't be zero");
        fatal();
    } else if (item_width > ARR_SIZE(tmp_buf)) {
        //TODO create a mechanism that resembles Linux's BUG_ON macro
        printk_error("item_width is larger than aux tmp space");
        fatal();
    } else if (arr_length < 2) return; /* nothing to shuffle here */

    /* C doesn't allow void pointer arithmetic as it doesn't know the size of 'increment' by */
    char *arr_cst = (char*) arr;

    for (size_t i = 0; i < arr_length; i++) {
        size_t r = (((size_t) rand()) % (arr_length - 1)) + 1;

        size_t i_pos = (i * item_width);
        size_t r_pos = (r * item_width);

        memcpy(tmp_buf, arr_cst + i_pos, item_width);
        memcpy(arr_cst + i_pos, arr_cst + r_pos, item_width);
        memcpy(arr_cst + r_pos, tmp_buf, item_width);
    }
}
