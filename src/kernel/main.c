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
#include "kernel/syscall/init.h"
#include "kernel/device/serial.h"

/* Testing launch process */
#include "kernel/task/process.h"

void kmain(void) {
    /* printk init */
    printk_init(PRINTK_INFO_LEVEL);
    vga_console_init();

    /* Serial COM1 port - RS232 */
    init_serial();

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

    /* memory management module init */
    mm_init();

    /* enabled IRQs */
    spurious_irq_enable();
    keyboard_enable();
    pit_enable();

    /* enable syscalls */
    syscall_init();

    /* Temp: Launch first process */
    task_struct_t* task = create_process(0x1C000);
    syscall_jump_usermode(0x41000);


    /* don't let kmain finish. Among other things, this ensure that interrupts have to to occur */
    for (;;) {
        asm("hlt");
    }
}
