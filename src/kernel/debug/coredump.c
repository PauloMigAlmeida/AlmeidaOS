/*
 * coredump.c
 *
 *  Created on: Jul 8, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/debug/coredump.h"
#include "kernel/lib/printk.h"

struct stackframe {
    struct stackframe *ebp;
    uint64_t rip;
};

void coredump(size_t max_frames) {
    //todo you should definitely blog about it...

    //#0  divide_by_zero_handler (frame=0x1013000000000008) at interrupt.c:111
    //#1  0x000000000020107d in kmain () at main.c:16
    //#2  0x000000000020102c in kernel_start ()

    struct stackframe *stack;
    asm volatile("movq %0, rbp" :"=r"(stack)::);
    printk("===============================================================================");
    //TODO add regs in the stack frame

    printk("Stack");
    for (size_t i = 0; stack && i < max_frames; i++) {
        printk("\t#%llu:  0x%.16x", i, stack->rip);
        stack = stack->ebp;
    }
    printk("===============================================================================");
}
