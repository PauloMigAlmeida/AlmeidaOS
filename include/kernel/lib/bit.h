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

__force_inline bool test_bit(uint8_t bit_number, uint64_t field) {
    return (field >> bit_number) & 1ULL;
}

__force_inline uint64_t extract_bit_chunk(uint8_t bit_from, uint8_t bit_to, uint64_t field) {
    return (field >> bit_from) & ( UINT64_MAX >> (sizeof(uint64_t) * CHAR_BIT - bit_to - bit_from + 1));
}

__force_inline uint64_t set_bit(uint8_t bit_number, uint64_t field) {
    return field | 1ULL << bit_number;
}

__force_inline uint64_t clear_bit(uint8_t bit_number, uint64_t field) {
    return field & ~(1UL << bit_number);
}

__force_inline uint64_t toggle_bit(uint8_t bit_number, uint64_t field) {
    return field ^ (1UL << bit_number);
}

#endif /* INCLUDE_KERNEL_LIB_BIT_H_ */
