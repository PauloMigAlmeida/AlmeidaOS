/*
 * math.h
 *
 *  Created on: Jun 27, 2021
 *      Author: Paulo Almeida
 */

#ifndef _KERNEL_LIB_MATH_H
#define _KERNEL_LIB_MATH_H

#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"

__force_inline int abs(int value) {
	/*
	 * C99 abs:"if the result cannot be represented, the behavior is undefined."
	 * So I decided not to handle 2-complement's edge case such as -2147483648
	 */
	int const mask = value >> (sizeof(int) * CHAR_BIT - 1);
	return (value + mask) ^ mask;
}

void rand_init_seed(uint32_t new_seed);
int rand(void);
int rand_r(uint32_t seed);

#endif /* _KERNEL_LIB_MATH_H */
