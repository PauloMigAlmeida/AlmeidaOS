/*
 * coredump.c
 *
 *  Created on: Jul 8, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/debug/coredump.h"
#include "kernel/lib/printk.h"
#include "kernel/compiler/macro.h"

struct stackframe {
    struct stackframe *ebp;
    uint64_t rip;
} __packed;

void coredump(interrupt_stack_frame_t *int_frame, size_t max_frames) {
    //TODO is it possible at this stage to core dump all cores? Or is it unnecessary?
    //TODO you should definitely blog about it...
    //TODO write user-space utility to parse these entries. Take a look at crash dump

    registers_64_t regs = int_frame->regs;

    struct stackframe *stack;
    asm volatile("movq %0, rbp" :"=r"(stack)::);
    printk_error("===============================================================================");
    printk_error("Registers:");
    printk_error("\tRAX: 0x%.16x, RBX: 0x%.16x, RCX: 0x%.16x", regs.rax, regs.rbx, regs.rcx);
    printk_error("\tRDX: 0x%.16x, RSP: 0x%.16x, RSI: 0x%.16x", regs.rdx, int_frame->rsp, regs.rsi);
    printk_error("\tRDI: 0x%.16x, R8:  0x%.16x, R9:  0x%.16x", regs.rdi, regs.r8, regs.r9);
    printk_error("\tR10: 0x%.16x, R11: 0x%.16x, R12: 0x%.16x", regs.r10, regs.r11, regs.r12);
    printk_error("\tR13: 0x%.16x, R14: 0x%.16x, R15: 0x%.16x", regs.r13, regs.r14, regs.r15);

    printk_error("Segments:");
    printk_error("\tRIP: 0x%.16x, RFLAGS: 0x%.16x", int_frame->rip, int_frame->rflags);
    printk_error("\tSS:  0x%.16x, CS:     0x%.16x", int_frame->ss, int_frame->cs);

    printk_error("Call stack:");
    for (size_t i = 0; stack && i < max_frames; i++) {
        printk_error("\t#%llu:  0x%.16x", i, stack->rip);
        stack = stack->ebp;
    }
    printk_error("===============================================================================");
}
