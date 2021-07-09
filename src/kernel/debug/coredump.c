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

void coredump(registers_64_t *regs, size_t max_frames) {
    //todo you should definitely blog about it...

    //#0  divide_by_zero_handler (frame=0x1013000000000008) at interrupt.c:111
    //#1  0x000000000020107d in kmain () at main.c:16
    //#2  0x000000000020102c in kernel_start ()

    struct stackframe *stack;
    asm volatile("movq %0, rbp" :"=r"(stack)::);
    printk("===============================================================================");
    //TODO add regs in the stack frame
    printk("Registers:");
    printk("\tRAX: 0x%.16x, RBX: 0x%.16x, RCX: 0x%.16x", regs->rax, regs->rbx, regs->rcx);
    printk("\tRDX: 0x%.16x, RSP: 0x%.16x, RSI: 0x%.16x", regs->rdx, regs->rsp, regs->rsi);
    printk("\tRDI: 0x%.16x, R8:  0x%.16x, R9:  0x%.16x", regs->rdi, regs->r8, regs->r9);
    printk("\tR10: 0x%.16x, R11: 0x%.16x, R12: 0x%.16x", regs->r10, regs->r11, regs->r12);
    printk("\tR13: 0x%.16x, R14: 0x%.16x, R15: 0x%.16x", regs->r13, regs->r14, regs->r15);

    printk("Segments:");
    printk("\tRIP: 0x%.16x, RFLAGS: 0x%.16x", regs->rip, regs->rflags);
    printk("\tSS:  0x%.16x, CS:     0x%.16x", regs->ss, regs->cs);

    printk("Call stack:");
    for (size_t i = 0; stack && i < max_frames; i++) {
        printk("\t#%llu:  0x%.16x", i, stack->rip);
        stack = stack->ebp;
    }
    printk("===============================================================================");
}
