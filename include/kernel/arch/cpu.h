/*
 * cpu.h
 *
 *  Created on: Jul 5, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_ARCH_CPU_H_
#define INCLUDE_KERNEL_ARCH_CPU_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"

/* CPU Flags */
#define RFLAGS_CF       (1 << 0)
#define RFLAGS_PF       (1 << 2)
#define RFLAGS_AF       (1 << 4)
#define RFLAGS_ZF       (1 << 6)
#define RFLAGS_SF       (1 << 7)
#define RFLAGS_TF       (1 << 8)
#define RFLAGS_IF       (1 << 9)
#define RFLAGS_DF       (1 << 10)
#define RFLAGS_OF       (1 << 11)
#define RFLAGS_IOPL     (3 << 12)
#define RFLAGS_NT       (1 << 14)
#define RFLAGS_RF       (1 << 16)
#define RFLAGS_ID       (1 << 21)

/* functions */
void cpu_init();
void enable_intel_faststring();

#endif /* INCLUDE_KERNEL_ARCH_CPU_H_ */
