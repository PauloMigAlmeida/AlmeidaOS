/*
 * round.c
 *
 *  Created on: Aug 17, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/lib/math.h"

/* Round up to the nearest known power of two - Credits Hacker's Delight book */
uint64_t round_up_po2(uint64_t value, uint64_t power_of_2) {
    return (value + power_of_2 - 1) & -power_of_2;
}
