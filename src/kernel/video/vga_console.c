/*
 * vga_console.c
 *
 *  Created on: Jun 28, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/video/vga_console.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/ringbuffer.h"
#include "kernel/lib/string.h"
#include "kernel/compiler/macro.h"
#include "kernel/mm/addressconv.h"

#define VIDEO_MEM_ADDR 	UNSAFE_VA(0xb8000)

#define VGA_MAX_COLS 	80
#define VGA_MAX_ROWS 	25

/* Controls cursor caret and soft-wrap functionality */
static int row = 0;

/*
 * Notes:
 *
 * On 21/06/2021:
 *
 * Controls ring-buffer structure that holds a copy of recently printed msgs.
 * This should come in handy to analyze crashes which memory dumps.
 * Ideally, I would like that to be a separate reusable ring-buff impl, however,
 * I have to implement a kmalloc-like functionality... to I have to play with
 * stuff created in the stack during compile time
 *
 * On 24/10/2021:
 *
 * UPDATE: I implemented the kmalloc functionality but I use printk for debugging
 * purposes as the mm/init.c is very critical. This creates a cyclic dependecy
 * in which I'm not willing to give up my only debugging method (printk) just to
 * not have this struct sitting on the stack.
 */

static ringbuffer_tp msg_buffer = { 
	/* leave the last the row empty for typing */
	.size = VGA_MAX_ROWS - 1  
};

void vga_console_init() {
    // init ring buffer
    ringbuffer_init(&msg_buffer);
    clear_console();
}

void update_cursor(int row, int col) {
    // ensures that caret position is always visible even on certain edge cases
    if (row == VGA_MAX_ROWS)
        row--;
    else if (row == 0)
        row++;

    uint16_t pos = row * VGA_MAX_COLS + col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void clear_console() {
    int nchars = VGA_MAX_COLS * VGA_MAX_ROWS;
    volatile char *video_address = (volatile char*) VIDEO_MEM_ADDR;
    for (int i = 0; i < nchars; i++) {
        video_address[0] = ' ';
        video_address[1] = 0xf;
        video_address += 2;
    }

    row = 0;
    update_cursor(row, 0);
}

void write_line_to_dma(const char *buf) {
    volatile char *video_address = (volatile char*) VIDEO_MEM_ADDR;
    video_address += row * VGA_MAX_COLS * 2;

    uint8_t idx = 0;
    char c = *buf;
    /* write new content */
	do {
		video_address[0] = c;
		video_address[1] = 0xf;
		video_address += 2;
		idx++;
	} while ((c = *(++buf)) != '\0');

    /* previous content could've been bigger so clean up the rest of the line */
    for(; idx < VGA_MAX_COLS - 1; idx++) {
        video_address[0] = ' ';
        video_address[1] = 0xf;
        video_address += 2;
    }
    row++;
}

void write_console(const char *buf, size_t buf_size) {
    /*
     * Notes for myself:
     *  I'm pretty sure that once we start dealing with SMP, the ringbuffer will before a source of problems..
     *  I need to come up with some sorting of spinlock or other locking mechanisms to avoid chaos.
     */
    char line[VGA_MAX_COLS + 1];
    size_t line_p = 0;

    for (size_t i = 0; i < buf_size - 1; i++) {
        char c = *(buf + i);

        if (c != '\n')
            line[line_p++] = c;

        if (c == '\n' || line_p == VGA_MAX_COLS - 1) {
            line[line_p++] = '\0';
            ringbuffer_put(&msg_buffer, line, line_p);
            memset(line, '\0', line_p);
            line_p = 0;
        }
    }

    if (line_p > 0) {
        line[line_p++] = '\0';
        ringbuffer_put(&msg_buffer, line, line_p);
    }

    row = 0;
    ringbuffer_for_each(&msg_buffer, &write_line_to_dma);
    update_cursor(row, 0);
}
