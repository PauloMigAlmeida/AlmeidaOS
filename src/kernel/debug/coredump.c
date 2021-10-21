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
    //TODO write user-space utility to parse these entries. Take a look at crash dump

    registers_64_t regs = int_frame->regs;
    sys_ctrl_regs_t ctrl_regs = int_frame->sys_ctrl_regs;

    struct stackframe *stack;
    asm volatile("movq %0, rbp" :"=r"(stack)::);

    printk_error("===============================================================================");
    printk_error("Registers:");
    printk_error("\tRAX: 0x%.16llx, RBX: 0x%.16llx, RCX: 0x%.16llx", regs.rax, regs.rbx, regs.rcx);
    printk_error("\tRDX: 0x%.16llx, RSP: 0x%.16llx, RSI: 0x%.16llx", regs.rdx, int_frame->rsp, regs.rsi);
    printk_error("\tRDI: 0x%.16llx, R8:  0x%.16llx, R9:  0x%.16llx", regs.rdi, regs.r8, regs.r9);
    printk_error("\tR10: 0x%.16llx, R11: 0x%.16llx, R12: 0x%.16llx", regs.r10, regs.r11, regs.r12);
    printk_error("\tR13: 0x%.16llx, R14: 0x%.16llx, R15: 0x%.16llx", regs.r13, regs.r14, regs.r15);

    printk_error("System Control Registers:");
    printk_error("\tCR0: 0x%.16llx, CR2: 0x%.16llx, CR3: 0x%.16llx", ctrl_regs.cr0, ctrl_regs.cr2, ctrl_regs.cr3);
    printk_error("\tCR4: 0x%.16llx, CR8: 0x%.16llx", ctrl_regs.cr4, ctrl_regs.cr8);

    printk_error("Segments:");
    printk_error("\tRIP: 0x%.16llx, RFLAGS: 0x%.16llx", int_frame->rip, int_frame->rflags);
    printk_error("\tSS:  0x%.16llx, CS:     0x%.16llx", int_frame->ss, int_frame->cs);

    printk_error("Call stack:");
    for (size_t i = 0; stack && i < max_frames; i++) {
        printk_error("\t#%llu:  0x%.16llx", i, stack->rip);
        stack = stack->ebp;
    }
    printk_error("===============================================================================");
}
