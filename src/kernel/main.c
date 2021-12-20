#include "kernel/interrupt/idt.h"
#include "kernel/asm/generic.h"
#include "kernel/video/vga_console.h"
#include "kernel/lib/printk.h"
#include "kernel/arch/cpu.h"
#include "kernel/arch/pic.h"
#include "kernel/device/keyboard.h"
#include "kernel/interrupt/spurious.h"
#include "kernel/mm/init.h"
#include "kernel/time/jiffies.h"
#include "kernel/time/delay.h"
#include "kernel/arch/pit.h"
#include "kernel/syscall/init.h"
#include "kernel/device/serial.h"
#include "kernel/task/scheduler.h"
#include "kernel/time/rtc.h"

void kmain(void) {
    /* disable all IRQs */
    pic_disable_all_irq();

    /* init real-time clock */
    rtc_init();

    /* printk init */
    printk_init(PRINTK_INFO_LEVEL);
    vga_console_init();

    /* Serial COM1 port - RS232 */
    init_serial();

    /* CPU features initialisation */
    cpu_init();

    /* Programmable Interrupt Controller */
    pic_init();

    /* Programmable Interval Timerchip */
    pit_init(HZ);

    /* Interrupt Descriptor table */
    idt_init();

    /* Unleash all possible problems in the world */
    enable_interrupts();

    /* init current time */
    rtc_init_curr_time();

    /* memory management module init */
    mm_init();

    /* enabled IRQs */
    spurious_irq_enable();
    keyboard_enable();
    pit_enable();

    /* Calibrate (x)delay functions */
    calibrate_delay();
    printk_info("Calibrated loops_per_jiffy: %.16llu", loops_per_jiffy);

    /* enable syscalls */
    syscall_init();

    /* how long did the boot take until here? */
    printk_info("System boot completed in %.16llu ms", rtc_curr_unixtime - rtc_startup_unixtime);

    disable_interrupts();

    /* initialise scheduler */
    task_struct_t *init_proc = create_process(0x1C000);
    scheduler_init(init_proc);

    /* test scheduler's ability to switch between tasks */
    for (size_t i = 2; i <= 9; i++) {
        scheduler_add(create_process(0x1C000));
    }

    enable_interrupts();

    /* don't let kmain finish. Among other things, this ensure that interrupts have to to occur */
    for (;;) {
        asm("hlt");
    }
}
