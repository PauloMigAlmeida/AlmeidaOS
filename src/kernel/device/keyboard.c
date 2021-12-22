/*
 * keyboard.c
 *
 *  Created on: Jul 13, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/device/keyboard.h"
#include "kernel/arch/pic.h"
#include "kernel/lib/printk.h"
#include "kernel/lib/bit.h"
#include "kernel/asm/generic.h"

/*
 * Things To Do:
 * - TODO Implement a better handling for key pressed and key released events
 * - TODO Implement ability to write on the screen (as if it was a bash / tty)
 *  -> After reading a lot, I got to the conclusion that I can only do this right after a few other things are in place
 *      such as:
 *      -> Userspace
 *      -> Syscalls
 *      -> Signal handling
 *      -> A rudimentary shell (arguable)
 *
 *      I could of course implement something quick and dirty but I wouldn't be able to rip the results of this effort
 *      When I'm ready to take on this, I have to read again: http://www.linusakesson.net/programming/tty/index.php
 */

void keyboard_enable(void) {
    /* PIC unmask is a critical path, interrupts must be disabled to avoid stack pointer data corruption */
    disable_interrupts();
    pic_unmask_irq(PIC_KEYBOARD_INTERRUPT);
    enable_interrupts();
    printk_info("Keyboard IRQ enabled");
}

void keyboard_handle_irq(void) {

    /* bit 7 is high when key is released */
    char* event;
    uint8_t scan_code = inb(0x60);
    if (!test_bit(7, scan_code)) {
        event = "pressed";
    } else {
        event = "released";
    }
    printk_info("Keyboard %s %u", event, scan_code);

    /* Acknowledge that we've received the interrupt */
    pic_send_eoi(PIC_KEYBOARD_INTERRUPT);
}
