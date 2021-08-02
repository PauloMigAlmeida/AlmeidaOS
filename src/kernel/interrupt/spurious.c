/*
 * spurious.c
 *
 *  Created on: Aug 2, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/interrupt/spurious.h"
#include "kernel/asm/generic.h"
#include "kernel/arch/pic.h"
#include "kernel/lib/printk.h"
#include "kernel/lib/bit.h"

void spurious_irq_enable(void) {
    /* PIC unmask is a critical path, interrupts must be disabled to avoid stack pointer data corruption */
    disable_interrupts();
    pic_unmask_irq(PIC_LPT1_OR_SPURIOUS_INTERRUPT);
    enable_interrupts();
    printk_info("Spurious IRQ enabled");
}

void spurious_handle_irq(void) {
    printk_info("Spurious irq happened");
    if (test_bit(7, pic_read_isr()))
        pic_send_eoi(PIC_LPT1_OR_SPURIOUS_INTERRUPT);
}
