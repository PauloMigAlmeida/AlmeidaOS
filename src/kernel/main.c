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
//temp
#include "kernel/arch/mem.h"
#include "kernel/lib/math.h"
#include "kernel/lib/shuffle.h"
#include "kernel/lib/qsort.h"


int qsort_cmp_int(const void *a, const void *b){
//    printk_info("qsort_cmp_int: a(%p):%d, b(%p):%d = %d", a, *((int*)a), b, *((int*)b), *((int*)a) - *((int*)b));
    return *((int*)a) - *((int*)b);
}

void kmain(void) {
    printk_init(PRINTK_INFO_LEVEL);
    vga_console_init();
    cpu_init();
    //tmp
    mem_init();

//    /* test LCG rand() */
//    for (int i = 0; i < 15000; i++) {
//        printk_info("next rand(): %d", rand());
//    }


//    for (int i = 0; i < 1500; i++) {
//        int arr[] = {1,2,3,4,5,6,7,8,9,10};
//        shuffle(arr, ARR_SIZE(arr), sizeof(int));
////        printk_info("before: %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", arr[0],arr[1],arr[2],arr[3],arr[4],arr[5],arr[6],arr[7],arr[8],arr[9]);
//        qsort(arr, ARR_SIZE(arr), sizeof(int), &qsort_cmp_int);
//        printk_info("after : %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", arr[0],arr[1],arr[2],arr[3],arr[4],arr[5],arr[6],arr[7],arr[8],arr[9]);
//    }

    idt_init();

    pic_init();
    pit_init(100);

    enable_interrupts();

    /* enabled IRQs */
    spurious_irq_enable();
    keyboard_enable();
    pit_enable();

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

    /* do let kmain finish. Among other things, this ensure that interrupts have to to occur */
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
