/*
 * interrupt.h
 *
 *  Created on: Jul 7, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_ARCH_INTERRUPT_H_
#define INCLUDE_KERNEL_ARCH_INTERRUPT_H_

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
    /* 64-bits general purpose registers*/
    registers_64_t regs;

    /* which ISR vector this came from */
    uint64_t trap_number;
    uint64_t error_code;

    /*Saved by the processor so it would know where to come back */
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;

} __packed interrupt_stack_frame_t;

void idt_init(void);

#endif /* INCLUDE_KERNEL_ARCH_INTERRUPT_H_ */
