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
			"rdmsr \n\t"
			: "=a" (retl),"=d"(reth)
			: "c" (addr)
	);

	return ((unsigned long long) retl) | ((reth) << 32);
}

#endif /* INCLUDE_KERNEL_ASM_GENERIC_H_ */
