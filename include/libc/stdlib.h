/*
 * stdlib.h
 *
 *  Created on: 12/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_LIBC_STDLIB_H_
#define INCLUDE_LIBC_STDLIB_H_

#include "libc/compiler/freestanding.h"

int rand(void);
int rand_r(unsigned int *seedp);
void srand(unsigned int s);

char* ltoa(long value, char *str, int radix);

int abs(int value);


#endif /* INCLUDE_LIBC_STDLIB_H_ */
