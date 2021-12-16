/*
 * ltoa.c
 *
 *  Created on: 16/12/2021
 *      Author: Paulo Almeida
 */

#include "libc/stdlib.h"
#include "libc/string.h"

char* ltoa(long value, char *str, int radix) {
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
