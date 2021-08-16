/*
 * rand.c
 *
 *  Created on: Jul 30, 2021
 *      Author: Paulo Almeida
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
 *  Notes to myself:
 *     - I don't particularly care much about "complying" with POSIX at kernel space for obvious reasons. I'm listing
 *          the method's compliance for learning purposes as I plan to write my rudimentary libc in the future
 *     - There is a lot of info out there around how rand() should no longer be used given its lack of true
 *          "randomness" in comparison to more recent algorithms. I'm sure that for security purposes this must mean
 *          a lot but that's not the case in which I will apply this method.
 *     - I created this rand() function so I can shuffle the array to be sorted by my quicksort implementation to avoid
 *           the worst case runtime (n^2) statistically which rand() should be sufficient for now. If in the future I
 *           need something more random than this PRGN, I will invest more time in choosing a more sophisticated algo
 *
 */

#include "kernel/lib/math.h"
#include "kernel/lib/bit.h"

#define RAND_MAX    0x7fffffff  /* hex(pow(2,32-1)-1) */
#define RAND_MOD    RAND_MAX + 1
#define RAND_MUL    1103515245
#define RAND_INC    12345

static uint32_t rand_seed = 1;

void rand_init_seed(uint32_t new_seed) {
    /* sanity check */
    if (new_seed < 1) {
        new_seed = 1;
    }
    rand_seed = new_seed;
}

int rand(void) {
    rand_seed = (RAND_MUL * rand_seed + RAND_INC) % RAND_MOD;
    return extract_bit_chunk(16, 30, rand_seed);
}

int rand_r(uint32_t seed) {
    return extract_bit_chunk(16, 30, (RAND_MUL * seed + RAND_INC) % RAND_MOD);
}
