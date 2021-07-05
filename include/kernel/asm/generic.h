/*
 * generic.h
 *
 *  Created on: Jun 29, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_ASM_GENERIC_H_
#define INCLUDE_KERNEL_ASM_GENERIC_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"

__force_inline void outb(uint16_t port, uint8_t value) {
    asm volatile (
            "outb  %[p],  %[v]"
            :
            : [p] "Nd" (port), [v] "a" (value)
    );
}

__force_inline void cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
    asm volatile (
            "cpuid \n"
            : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
            : "a" (*eax)
            : "memory"
    );
}

__force_inline void enable_interrupts() {
    asm volatile ("sti");
}

__force_inline void disable_interrupts() {
    asm volatile ("cli");
}

#endif /* INCLUDE_KERNEL_ASM_GENERIC_H_ */
