/*
 * round.c
 *
 *  Created on: Aug 17, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/lib/math.h"

/* Round up to a multiple of a known power of two - Credits Hacker's Delight book. Pg 60 */
uint64_t round_up_po2(uint64_t value, uint64_t power_of_2) {
    return (value + power_of_2 - 1) & -power_of_2;
}

/* Round down to the nearest power of two - Credits Hacker's Delight book. Pg 61 */
uint64_t flp2(uint64_t value) {
    value |= (value >> 1);
    value |= (value >> 2);
    value |= (value >> 4);
    value |= (value >> 8);
    value |= (value >> 16);
    value |= (value >> 32);
    return value - (value >> 1);
}

/* Round up to the nearest power of two - Credits Hacker's Delight book. Pg 62 */
uint64_t clp2(uint64_t value) {
    value -= 1;
    value |= (value >> 1);
    value |= (value >> 2);
    value |= (value >> 4);
    value |= (value >> 8);
    value |= (value >> 16);
    value |= (value >> 32);
    return value + 1;
}
