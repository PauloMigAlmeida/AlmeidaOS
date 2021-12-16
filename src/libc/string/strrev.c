/*
 * strrev.c
 *
 *  Created on: 16/12/2021
 *      Author: Paulo Almeida
 */

#include "libc/string.h"

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
