#include "kernel/compiler/freestanding.h"
#include "kernel/asm/generic.h"
#include "kernel/video/vga_console.h"
#include "kernel/lib/string.h"
#include "kernel/lib/printk.h"
#include "kernel/arch/cpu.h"
#include "kernel/arch/interrupt.h"

int kmain(void) {
    vga_console_init();
    cpu_init();
    idt_init();

    enable_interrupts();
    // doing something stupid for testing
    int x = 1 / 0;

    // dummy test of the memcpy function
    int a = 10;
    int b = 2;
    memcpy(&b, &a, sizeof(int));

    printk("a: %d\n", a);
    x = 1 / 0;
    printk("Value of b is: %d\n", b);

// test row reset;
//  for(int i=0; i < 25; i++){
//	  printk("i: %d\n", i);
//  }

// test soft wrap
//    printk("abcdefghijklmnopqrstuvxzabcdefghijklmnopqrstuvxzabcdefghijklmnopqrstuvxzabcdefghijklmnopqrstuvxz\n");

//    for (int i = 0; i < 5; i++) {
//        printk("i: %d\n", i);
//    }
//    printk("Test negative numbers: %d", -2147483647);
//
//    printk("hex a: 0x%x\n", 4294967295);
//    printk("uint a: %u\n", 4294967295);
//    printk("octal a: %o\n", 511);
//    printk("char a: %c\n", 'p');
//    printk("My name is: %s\n", "AlmeidaOS");
//    printk("My name is: %s\n", "");
//    printk("My name is: %s\n", "AlmeidaOS");

//    int x = -100;
//    printk("0x%.16llx and %llu and %o %.15s %.5d %.75c", ia32_misc_enable, ia32_misc_enable, 9, "Paulo", x, 'a');
    return 0;
}

/*
 * Notes to myself:
 *
 * Next I will have to implement interrupts. My main goal is to grasp the concept properly this time but as an PoC, I
 * would like to get the timer interrupt working so I can add that to printk for funzzies
 *
 * Relevant notes:
 *  -> Interrupt Description Table:
 *      -> The long-mode interrupt-descriptor table (IDT) must contain 64-bit mode interrupt-gate or trap-gate
 *          descriptors for all interrupts or exceptions that can occur while the processor is running in long mode.
 *          Task gates cannot be used in the long-mode IDT, because control transfers through task gates are not
 *          supported in long mode. In long mode, the IDT index is formed by scaling the interrupt vector by 16.
 *
 *      -> An interrupt handler is privileged software designed to identify and respond to the cause of an
 *          interrupt or exception, and return control back to the interrupted software
 *
 *      -> IDT entries are selected using the interrupt vector number rather than a selector value.
 *
 *      -> Vectors 0 through 8, 10 through 14, and 16 through 19 are the predefined interrupts and exceptions;
 *         vectors 32 through 255 are for software-defined interrupts, which are for either software interrupts or
 *         maskable hardware interrupts.
 *
 */

/*
 System V AMD64 Calling Convention

 RDI, RSI, RDX, RCX, R8, R9 -> Others are on the stack
 RAX -> Return values
 RAX, RCX, RDX, RSI, RDI,R8, R9, R10, R11 ->  Caller Saved Registers
 RBX, RBP, R12, R13, R14, R15 ->  Calle Saved Registers
 */
