/*
 * pic.c
 *
 *  Created on: Jul 11, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/arch/pic.h"
#include "kernel/compiler/freestanding.h"
#include "kernel/arch/pit.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/bit.h"
#include "kernel/lib/printk.h"

/*
 *  TODO: study about PIC and APIC -> So we can get the timer working
 *
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
 *
 */

#define PIC1            0x20        /* IO base address for Active PIC */
#define PIC2            0xA0        /* IO base address for Passive PIC */
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1+1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2+1)

#define ICW1_INIT               (0 << 8) | (1 << 4)
#define ICW1_ICW4_NEEDED        1
#define ICW1_CALLADDR_4         1 << 2

void pic_init(void) {
    /* enable timer */
    //TODO maybe move this somewhere else ?
    pit_init();

    /* Send ICW1 to both PIC chips */
    outb(PIC1, ICW1_INIT | ICW1_CALLADDR_4 | ICW1_ICW4_NEEDED);
    outb(PIC2, ICW1_INIT | ICW1_CALLADDR_4 | ICW1_ICW4_NEEDED);

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
    outb(PIC1_DATA,0xff);
    outb(PIC2_DATA,0xff);

    printk("PIC initialised");
}

void enable_keyboard_irq(void) {
    /* keyboard IRQ is on the active PIC */
    uint8_t value = inb(PIC1_DATA);
    printk("PIC1 mask state: %u\n", value);
    /* Keyboard is IRQ 1 according to Standard ISA IRQs */
    if(test_bit(1, value)){
        value = clear_bit(1, value);
        printk("PIC1 new mask State: %u\n", value);
        outb(PIC1_DATA, value);
        printk("Keyboard IRQ enabled");
    }
}

