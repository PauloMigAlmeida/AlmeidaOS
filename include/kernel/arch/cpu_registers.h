/*
 * cpu_registers.h
 *
 *  Created on: 12/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_ARCH_CPU_REGISTERS_H_
#define INCLUDE_KERNEL_ARCH_CPU_REGISTERS_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"

typedef struct {
    /* 64-bits general purpose registers*/
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
} __packed registers_64_t;

typedef struct {
    /* System control registers: AMD manual 3.1, Page: 41 */
    uint64_t cr8;
    uint64_t cr4;
    uint64_t cr3;
    uint64_t cr2;
    uint64_t cr0;
} __packed sys_ctrl_regs_t;

#endif /* INCLUDE_KERNEL_ARCH_CPU_REGISTERS_H_ */
