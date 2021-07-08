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

typedef struct {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
} __packed registers_64_t;

__force_inline void dump_registers(registers_64_t *regs) {
    //TODO this crap is all wrong... I have to rewrite this in assembly to avoid values gettings messed up
    asm volatile(
            "mov %0, rax \n"
            "mov %1, rbx \n"
            "mov %2, rcx \n"
            "mov %3, rdx \n"
            "mov %4, rsp \n"
            "mov %5, rsi \n"
            "mov %6, rdi \n"
            "mov %7, r8 \n"
            "mov %8, r9 \n"
            "mov %10, r10 \n"
            "mov %10, r11 \n"
//            "mov %12, r12 \n"
//            "mov %13, r13 \n"
//            "mov %14, r14 \n"
//            "mov %15, r15 \n"
            : "=r"(regs->rax),"=r"(regs->rbx),"=r"(regs->rcx),"=r"(regs->rdx),"=r"(regs->rsp),
            "=r"(regs->rsi),"=r"(regs->rdi),"=r"(regs->r8),"=r"(regs->r9),"=r"(regs->r10),"=r"(regs->r11)
//              "=r"(regs->r12),"=r"(regs->r13),"=r"(regs->r14),"=r"(regs->r15)
    );
}

void coredump(size_t max_frames) {
    //todo you should definitely blog about it...

    //#0  divide_by_zero_handler (frame=0x1013000000000008) at interrupt.c:111
    //#1  0x000000000020107d in kmain () at main.c:16
    //#2  0x000000000020102c in kernel_start ()

    registers_64_t regs;
    dump_registers(&regs);

    struct stackframe *stack;
    asm volatile("movq %0, rbp" :"=r"(stack)::);
    printk("===============================================================================");
    //TODO add regs in the stack frame
    printk("Registers:");
    printk("\tRAX: 0x%.16x, RBX: 0x%.16x, RCX: 0x%.16x", regs.rax, regs.rbx, regs.rcx);
    printk("\tRDX: 0x%.16x, RSP: 0x%.16x, RSI: 0x%.16x", regs.rdx, regs.rsp, regs.rsi);
    printk("\tRDI: 0x%.16x, R8:  0x%.16x, R9:  0x%.16x", regs.rdi, regs.r8, regs.r9);
    printk("\tR10: 0x%.16x, R11: 0x%.16x", regs.r10, regs.r11);

    printk("Call stack:");
    for (size_t i = 0; stack && i < max_frames; i++) {
        printk("\t#%llu:  0x%.16x", i, stack->rip);
        stack = stack->ebp;
    }
    printk("===============================================================================");
}
