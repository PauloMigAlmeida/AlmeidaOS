/*
 * rand.c
 *
 * Based on the Linear Congruential Generator algorithm
 *  -> rand() conforms to SVr4, 4.3BSD, C89, C99, POSIX.1-2001.
 *  -> rand_r() is from POSIX.1-2001. POSIX.1-2008 marks rand_r() as obsolete.
 *
 * I've used the GLIBC's LCGs parameters (which happens to be the same as ANSI C - and some C(%.2d) variations):
 *
 *  - modulus m           ->  2^31 (0x7fffffff)
 *  - multiplier a        ->  1103515245
 *  - increment c         ->  12345
 *  - output bits of seed ->  bits 30..16
 *
 *  Created on: 12/12/2021
 *      Author: Paulo Almeida
 *
 */

#include "libc/stdlib.h"

#define RAND_MAX    0x7fffffff  /* hex(pow(2,32-1)-1) */
#define RAND_MOD    RAND_MAX + 1
#define RAND_MUL    1103515245
#define RAND_INC    12345

static uint64_t rand_seed = 1;

void srand(unsigned int s) {
    rand_seed = s - 1;
}

int rand(void) {
    rand_seed = (RAND_MUL * rand_seed + RAND_INC) % RAND_MOD;
    return (rand_seed >> 16) & 0x3fff;
}

int rand_r(unsigned int *seedp) {
    return (((RAND_MUL * (*seedp) + RAND_INC) % RAND_MOD) >> 16) & 0x3fff;
}
