/*
 * msr.c
 *
 *  Created on: Jul 5, 2021
 *      Author: Paulo Almeida
 */
#include "kernel/arch/msr.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/bit.h"

bool is_msr_supported() {
    /* If CPUID.01H:EDX[5] = 1 -> MSR is supported */
    uint32_t eax = 0x1, ebx = 0, ecx = 0, edx = 0;
    cpuid(&eax, &ebx, &ecx, &edx);
    return test_bit(5, edx);
}

