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
#include "kernel/arch/cpu_registers.h"

typedef struct {
    /* 64-bits general purpose registers*/
    sys_ctrl_regs_t sys_ctrl_regs;

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
