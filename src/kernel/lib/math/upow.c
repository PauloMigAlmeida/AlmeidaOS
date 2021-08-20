/*
 * upow.c
 *
 *  Created on: 20/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/lib/math.h"

uint64_t upow(uint64_t base, uint64_t exp) {
    uint64_t p, y;
    y = 1;
    p = base;
    while (true) {
        if (exp & 1)
            y = p * y;
        exp = exp >> 1;
        if (exp == 0)
            return y;
        p = p * p;
    }
}

