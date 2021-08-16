/*
 * math.h
 *
 *  Created on: Jun 27, 2021
 *      Author: Paulo Almeida
 */

#ifndef _KERNEL_LIB_MATH_H
#define _KERNEL_LIB_MATH_H

#include "kernel/compiler/freestanding.h"

int abs(int value);

void rand_init_seed(uint32_t new_seed);
int rand(void);
int rand_r(uint32_t seed);

/* Round up to the nearest known power of two - Credits Hacker's Delight book */
uint64_t round_up_po2(uint64_t value, uint64_t power_of_2);

#endif /* _KERNEL_LIB_MATH_H */
