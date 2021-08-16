/*
 * abs.c
 *
 *  Created on: Aug 17, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/lib/math.h"

int abs(int value) {
    /*
     * C99 abs:"if the result cannot be represented, the behavior is undefined."
     * So I decided not to handle 2-complement's edge case such as -2147483648
     */
    int const mask = value >> (sizeof(int) * CHAR_BIT - 1);
    return (value + mask) ^ mask;
}
