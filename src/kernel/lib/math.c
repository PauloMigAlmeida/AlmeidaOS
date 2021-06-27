/*
 * math.c
 *
 *  Created on: Jun 27, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/lib/math.h"
#include <limits.h>

unsigned int abs(int value){
	//TODO implement abs function (maybe inline this function if needed)
	int v;
	unsigned int r;
	int const mask = v >> sizeof(int) * CHAR_BIT - 1;

	r = (v + mask) ^ mask;
	return r;
}

