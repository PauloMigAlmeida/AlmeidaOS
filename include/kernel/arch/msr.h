/*
 * msr.h
 *
 *  Created on: Jul 5, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_ARCH_MSR_H_
#define INCLUDE_KERNEL_ARCH_MSR_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"

/* MSR Addresses */
#define MSR_IA32_MISC_ENABLE 0x1A0

/* Model-specific registers used to set up system calls. */
#define MSR_IA32_EFER   0xC0000080
#define MSR_IA32_STAR   0xC0000081
#define MSR_IA32_LSTAR  0xC0000082
#define MSR_IA32_FMASK  0xC0000084

// MSR Addresses/Features
#define MSR_IA32_MISC_ENABLE_FAST_STRING_BIT (1ULL << 0)

bool is_msr_supported();

/* Read MSR specified by ECX into EDX:EAX. */
__force_inline unsigned long long rdmsr(uint32_t addr) {
    /*
     Reads the contents of a 64-bit model specific register (MSR) specified in
     the ECX register into registers EDX:EAX. (On processors that support the
     Intel 64 architecture, the high-order 32 bits of RCX are ignored.)

     The EDX register is loaded with the high-order 32 bits of the MSR and the
     EAX register is loaded with the low-order 32 bits. (On processors that
     support the Intel 64 architecture, the high-order 32 bits of each of RAX
     and RDX are cleared.)

     This instruction must be executed at privilege level 0 or in real-address mode;
     otherwise, a general protection exception #GP(0) will be generated.

     The CPUID instruction should be used to determine whether MSRs are supported
     (CPUID.01H:EDX[5] = 1) before using this instruction.
     */

    unsigned long long retl, reth;
    asm volatile (
            "rdmsr \n"
            : "=a" (retl),"=d"(reth)
            : "c" (addr)
    );

    return ((unsigned long long) retl) | ((reth) << 32);
}

__force_inline void wrmsr(uint32_t addr, unsigned long long value) {
    /*
     Writes the contents of registers EDX:EAX into the 64-bit model specific
     register (MSR) specified in the ECX register. (On processors that support
     the Intel 64 architecture, the high-order 32 bits of RCX are ignored.)

     The contents of the EDX register are copied to high-order 32 bits of the
     selected MSR and the contents of the EAX register are copied to low-order
     32 bits of the MSR. (On processors that support the Intel 64 architecture,
     the high-order 32 bits of each of RAX and RDX are ignored.)

     Undefined or reserved bits in an MSR should be set to values previously read.

     This instruction must be executed at privilege level 0 or in real-address mode;
     otherwise, a general protection exception #GP(0) is generated.

     The CPUID instruction should be used to determine whether MSRs are supported
     (CPUID.01H:EDX[5] = 1) before using this instruction.
     */

    uint32_t eax, edx;
    edx = (uint32_t) (value >> 32);
    eax = (uint32_t) value;

    asm volatile (
            "wrmsr \n"
            :
            : "a" (eax), "d" (edx), "c" (addr)
            : "memory"
    );

}

#endif /* INCLUDE_KERNEL_ARCH_MSR_H_ */
