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
    /* custom values pushed onto stack to help kernel identify
     * from which isr vector this cam from */
    uint64_t error_code;
    uint64_t trap_number;
    /* 64-bits general purpose regiters*/
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
    /*Saved by the processor so it would know where to come back */
    uint64_t rip; // Getting Flags
    uint64_t cs; // Getting RIP
    uint64_t rflags; // Getting CS
    uint64_t rsp; // Getting FLAGS again? WTF?
    uint64_t ss; // Getting RSP

} __packed registers_64_t;
//TODO split this struct... I can't have this all under "registers_64_t".. I need additional structs for that.

void idt_init(void);

#endif /* INCLUDE_KERNEL_ARCH_INTERRUPT_H_ */
