/*
 * bit.h
 *
 *  Created on: Jul 5, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_LIB_BIT_H_
#define INCLUDE_KERNEL_LIB_BIT_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"

__force_inline bool test_bit(uint8_t bit_number, unsigned long long field) {
    return (field >> bit_number) & 1ULL;
}

__force_inline uint32_t extract_bit_chunk(uint8_t bit_from, uint8_t bit_to, uint32_t field) {
    return (field >> bit_from) & ( UINT32_MAX >> (sizeof(uint32_t) * CHAR_BIT - bit_to - bit_from + 1));
}

#endif /* INCLUDE_KERNEL_LIB_BIT_H_ */
