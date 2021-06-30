/*
 * math.h
 *
 *  Created on: Jun 27, 2021
 *      Author: Paulo Almeida
 */

#ifndef _KERNEL_LIB_MATH_H
#define _KERNEL_LIB_MATH_H

#include <limits.h>
#include "kernel/compiler/macro.h"

__force_inline unsigned int abs(int value) {
	//TODO think about what to do for INT_MIN case (-2147483648) - maybe check how glibc does it.
	unsigned int r;
	int const mask = value >> (sizeof(int) * CHAR_BIT - 1);
	r = (value + mask) ^ mask;
	return r;
}

#endif /* _KERNEL_LIB_MATH_H */
