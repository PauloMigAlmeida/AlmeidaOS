/*
 * keyboard.c
 *
 *  Created on: Jul 13, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/device/keyboard.h"
#include "kernel/arch/pic.h"
#include "kernel/lib/printk.h"
#include "kernel/asm/generic.h"

void keyboard_init(void) {
    pic_unmask_irq(PIC_KEYBOARD_INTERRUPT);
    printk("Keyboard IRQ enabled");
}

void keyboard_handle_irq(void) {
    /*
     everything leads me to believe that bit 7 is high when key is released
     >>> bin(50)
     '0b110010'
     >>> bin(178)
     '0b10110010'
     >>> bin(38)
     '0b100110'
     >>> bin(166)
     '0b10100110'
     *
     */

    unsigned char scan_code = inb(0x60);
    printk("Keyboard pressed %u\n", (unsigned int) scan_code);

    /* Acknowledge that we've received the interrupt */
    pic_send_eoi(PIC_KEYBOARD_INTERRUPT);
}
