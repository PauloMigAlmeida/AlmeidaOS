/*
 * pic.c
 *
 *  Created on: Jul 11, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/arch/pic.h"
#include "kernel/compiler/freestanding.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/bit.h"
#include "kernel/lib/printk.h"
#include "kernel/mm/addressconv.h"

/*
 *  PIC 8259A Datasheet: https://pdos.csail.mit.edu/6.828/2005/readings/hardware/8259A.pdf
 *
 *  Pic Notes:
 *      -> There are 2 of them; Each has 8 inputs
 *      -> When the system first starts up, IRQs 0 to 7 are set to interrupts 08h to 0Fh, and IRQs 8 to 15 are set to
 *          interrupts 70h to 77h
 *      -> Every time PIC sends an interrupt, the CPU must acknowledge it (take into consideration Master and Slave pics)
 *      -> Fun fact: CPU has reserved interrupts 0-31, yet IRQs 0-7 are set to interrupts 08-0Fh. This conflicts with each other
 *          -> The best way to do it is to tell the PIC to map the IRQs to different interrupts
 *          -> you probably want to use 20h-27h, or greater, since 00h-1Fh are reserved by the CPU.
 *          -> More: https://wiki.osdev.org/Interrupt#From_the_PIC.27s_perspective
 *      -> ICW3
 *          -> Active PIC -> Each bit represents which IRQ will be used to cascade that to Passive PIC
 *          -> Passive PIC -> There is a table on page 12 of the datasheet
 *      -> PIC is sufficient for single processor machines. APIC is recommended to SMP
 *
 */

#define PIC1            0x20        /* IO base address for Active PIC */
#define PIC2            0xA0        /* IO base address for Passive PIC */
#define PIC1_COMMAND    UNSAFE_VA(PIC1)
#define PIC1_DATA       UNSAFE_VA(PIC1 + 1)
#define PIC2_COMMAND    UNSAFE_VA(PIC2)
#define PIC2_DATA       UNSAFE_VA(PIC2 + 1)
#define PIC_EOI         0x20        /* End-of-interrupt command code */
#define PIC_READ_IRR    0x0a        /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR    0x0b        /* OCW3 irq service next CMD read */

#define ICW1_INIT               (0 << 8) | (1 << 4)
#define ICW1_ICW4_NEEDED        1
#define ICW1_CALLADDR_4         1 << 2

void pic_init(void) {
    /* Send ICW1 to both PIC chips */
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_CALLADDR_4 | ICW1_ICW4_NEEDED);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_CALLADDR_4 | ICW1_ICW4_NEEDED);

    /* Determine the offset */
    outb(PIC1_DATA, 32);
    outb(PIC2_DATA, 40);

    /* Configure active/passive device cascade */
    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);

    /* Set operation mode - 8086 mode on*/
    outb(PIC1_DATA, 1);
    outb(PIC2_DATA, 1);

    /* mask (disable) all PIC interrupts */
    pic_disable_all_irq();

    printk_info("PIC initialised");
}

void pic_send_eoi(uint8_t isa_irq) {
    /* Passive PIC */
    if (isa_irq >= 8)
        outb(PIC2_COMMAND, PIC_EOI);

    /* Active PIC */
    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_unmask_irq(uint8_t isa_irq) {
    uint64_t pic_selector;
    if (isa_irq < 8)
        pic_selector = PIC1_DATA;
    else {
        pic_selector = PIC2_DATA;
        isa_irq -= 8;
    }

    uint8_t value = inb(pic_selector);
    printk_debug("PIC Addr: 0x%x mask state: %u\n", pic_selector, value);
    if (test_bit(isa_irq, value)) {
        value = clear_bit(isa_irq, value);
        printk_debug("PIC Addr: 0x%x new mask State: %u\n", pic_selector, value);
        outb(pic_selector, value);
        printk_debug("IRQ %u unmasked\n", isa_irq);
    }
}

void pic_mask_irq(uint8_t isa_irq) {
    uint64_t pic_selector;
    if (isa_irq < 8)
        pic_selector = PIC1_DATA;
    else {
        pic_selector = PIC2_DATA;
        isa_irq -= 8;
    }

    uint8_t value = inb(pic_selector);
    printk_debug("PIC Addr: 0x%x mask state: %u\n", pic_selector, value);
    if (!test_bit(isa_irq, value)) {
        value = set_bit(isa_irq, value);
        printk_debug("PIC Addr: 0x%x new mask State: %u\n", pic_selector, value);
        outb(pic_selector, value);
        printk_debug("IRQ %u masked\n", isa_irq);
    }
}

static uint16_t pic_get_irq_reg(uint8_t ocw3) {
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

uint16_t pic_read_isr(void) {
    return pic_get_irq_reg(PIC_READ_ISR);
}

void pic_disable_all_irq(void) {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}
