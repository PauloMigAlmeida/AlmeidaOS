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

__force_inline uint8_t inb(uint64_t port) {
    uint8_t value = 0;
    asm volatile (
            "inb  %[v],  %[p]"
            : [v] "=a" (value)
            : [p] "Nd" (port)
    );
    return value;
}

__force_inline void outb(uint64_t port, uint8_t value) {
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

__force_inline void halt() {
    asm volatile ("hlt");
}

__force_inline void fatal() {
    asm volatile ("int 0xff");
}

__force_inline void load_cr3(uint64_t addr) {
    asm volatile(
            "mov cr3, %0 \n"
            :
            : "D" (addr)
            : "memory"
    );
}

#endif /* INCLUDE_KERNEL_ASM_GENERIC_H_ */
