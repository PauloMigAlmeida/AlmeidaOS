/*
 * pit.c
 *
 *  Created on: Jul 13, 2021
 *      Author: Paulo Almeida
 */
#include "kernel/arch/pit.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/printk.h"

void pit_init(void) {
    outb(0x43, (uint8_t) ((1 << 2) | (3 << 4)));
    outb(0x40, 0x9b);
    outb(0x40, 0x2e);
    printk("PIT enabled");
}
