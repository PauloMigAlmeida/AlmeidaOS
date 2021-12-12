/*
 * serial.c
 *
 * Adapted from https://wiki.osdev.org/Serial_Ports
 *
 *  Created on: 12/12/2021
 *      Author: Paulo Almeida
 */

#include "kernel/device/serial.h"
#include "kernel/asm/generic.h"
#include "kernel/mm/addressconv.h"

#define COM1_PORT   UNSAFE_VA(0x3f8)

static bool initialised;

void init_serial(void) {
    outb(COM1_PORT + 1, 0x00);    // Disable all interrupts
    outb(COM1_PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(COM1_PORT + 0, 0x0C);    // Set divisor to 12 (lo byte) 9600 baud
    outb(COM1_PORT + 1, 0x00);    //                  (hi byte)
    outb(COM1_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(COM1_PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(COM1_PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outb(COM1_PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
    outb(COM1_PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if (inb(COM1_PORT + 0) != 0xAE) {
        initialised = false;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(COM1_PORT + 4, 0x0F);
    initialised = true;
}

static bool is_transmit_empty() {
    return inb(COM1_PORT + 5) & 0x20;
}

void write_char_serial(char a) {
    /* sanity check */
    if (!initialised)
        return;

    /* wait for the right conditions */
    while (is_transmit_empty() == 0)
        ;

    /* send char */
    outb(COM1_PORT, a);
}

void write_string_serial(char *arr, size_t length) {
    /* sanity check */
    if (!initialised)
        return;

    for (size_t i = 0; i < length; i++) {
        write_char_serial(*(arr + i));
    }
    write_char_serial('\n');
}
