/*
 * generic.h
 *
 *  Created on: Jun 29, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_ASM_GENERIC_H_
#define INCLUDE_KERNEL_ASM_GENERIC_H_

#include <stdint.h>

inline __attribute__((always_inline)) void outb(uint16_t port, uint8_t value)
{
    asm volatile (
        "outb  %[p],  %[v]"
        :
        : [p] "Nd" (port), [v] "a" (value)
        );
}


#endif /* INCLUDE_KERNEL_ASM_GENERIC_H_ */
