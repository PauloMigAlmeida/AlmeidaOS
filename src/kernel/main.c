#include "kernel/interrupt/idt.h"
#include "kernel/compiler/freestanding.h"
#include "kernel/asm/generic.h"
#include "kernel/video/vga_console.h"
#include "kernel/lib/string.h"
#include "kernel/lib/printk.h"
#include "kernel/arch/cpu.h"
#include "kernel/arch/pic.h"
#include "kernel/arch/pit.h"
#include "kernel/device/keyboard.h"
#include "kernel/interrupt/spurious.h"
#include "kernel/mm/init.h"

//temp
#include "kernel/mm/page.h"
#include "kernel/compiler/bug.h"

void kmain(void) {
    /* printk init */
    printk_init(PRINTK_INFO_LEVEL);
    vga_console_init();

    /* memory management module init */
    mm_init();

    /* CPU features initialisation */
    cpu_init();

    /* Interrupt Descriptor table */
    idt_init();

    /* Programmable Interrupt Controller */
    pic_init();

    /* Programmable Interval Timerchip */
    pit_init(100);

    /* Unleash all possible problems in the world */
    enable_interrupts();

    /* enabled IRQs */
    spurious_irq_enable();
    keyboard_enable();
    pit_enable();

    /* Test page alloc and page free */
    page_alloc(kernel_pagetable(),
    K_VIRT_END_ADDR + 1,
            0x10000,
            PAGE_PRESENT_BIT | PAGE_READ_WRITE_BIT);
    int *x = (int*) (K_VIRT_END_ADDR + 1);
    *x = 10;
    BUG_ON(*x != 10);
    printk_info("&x: 0x%.16llx *x: 0x%.16llx", x, *x);

    //TODO something isn't working right for the page_free operation..it needs more investigation

//    page_free(kernel_pagetable(),
//            K_VIRT_END_ADDR + 1);

//        // force page fault
//        memzero((void*)0x20000, 1);

//    /* Testing kmalloc and kmem */
//    #include "kernel/lib/math.h"
//    #include "kernel/mm/kmem.h"
//    for (int i = 0; i < 8192; i++) {
//        int rnd = (rand() % (8192)) + 1;
//        int *ptr = kmalloc(rnd);
//        if (ptr) {
//            *ptr = 10;
//        }
//        printk_info("i: %d ptr: %p rnd: %d content: %d", i, ptr, rnd, *ptr);
//        if(ptr)
//            kfree(ptr);
//    }

//    mem_phys_stats_t t = mem_phys_stat();
//    printk_info("Original t.avail: %llu", t.phys_free_mem);
//    t.phys_free_mem *= 2;
//    printk_info("Mod t.avail: %llu", t.phys_free_mem);
//    t = mem_phys_stat();
//    printk_info("New t.avail: %llu", t.phys_free_mem);

//    BUG_ON(true);

    /* We should get a Page fault here to confirm that we fully moved to the higher-half memory */
//    volatile pml4e_t* p = (volatile pml4e_t*)0x00020000;
//    printk_info("pml4e_t: 0x%.16llx", *p);
//    printk_info("nx: 0x%.1x, avai_gh: 0x%lx, pdp_ba: 0x%.16llx, flags: 0x%.4lx",
//            p->no_execute_bit, p->available_guardhole, p->pdpe_base_addr << 12,
//            p->flags);
// doing something stupid for testing
//    int x = 1 / 0;
//
//    // dummy test of the memcpy function
//    int a = 10;
//    int b = 2;
//    memcpy(&b, &a, sizeof(int));
//
//    printk("a: %d\n", a);
//    x = 1 / 0;
//    printk("Value of b is: %d\n", b);
// test row reset;
//  for(int i=0; i < 12; i++){
//	  printk("i: %d\n", i);
//  }
//    test soft wrap
//    printk("abcde/fghijklmnopqrstuvxzabcdefghijklmnopqrstuvxzabcdefghijklmnopqrstuvxzabcdefghijklmnopqrstuvxz\n");
//
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
//
//    int x = -100;
//    uint64_t ia32_misc_enable = 0x00ff0000000000009;
//    printk("0x%.16llx and %llu and %o %.15s %.5d %.75c", ia32_misc_enable, ia32_misc_enable, 9, "Paulo", x, 'a');
    /* don't let kmain finish. Among other things, this ensure that interrupts have to to occur */
    for (;;) {
        asm("hlt");
    }
}

/*
 System V AMD64 Calling Convention

 RDI, RSI, RDX, RCX, R8, R9 -> Others are on the stack
 RAX -> Return values
 RAX, RCX, RDX, RSI, RDI,R8, R9, R10, R11 ->  Caller Saved Registers
 RBX, RBP, R12, R13, R14, R15 ->  Calle Saved Registers
 */
