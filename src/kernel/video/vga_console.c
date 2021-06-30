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

#define VIDEO_MEM_ADDR 	0xb8000
#define VGA_MAX_COLS 	80
#define VGA_MAX_ROWS 	25

/* Controls cursor caret and soft-wrap functionality */
static int row = 0;

/*
 * Controls ring-buffer structure that holds a copy of recently printed msgs.
 * This should come in handy to analyze crashes which memory dumps.
 * Ideally, I would like that to be a separate reusable ring-buff impl, however,
 * I have to implement a kmalloc-like functionality... to I have to play with
 * stuff created in the stack during compile time
 *
 * TODO: create kmalloc functionality and move this ring buffer to a re-useable
 * mod
 */

static ringbuffer_tp msg_buffer = { .size = VGA_MAX_ROWS };

void vga_console_init(){
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

	char c;
	do {
		video_address[0] = *buf;
		video_address[1] = 0xf;
		video_address += 2;

	} while ((c = *(++buf)) != '\0');
	row++;
}

void write_console(const char *buf, size_t buf_size) {
	char line[VGA_MAX_COLS+1];
	memset(line, '\0', sizeof(line)/sizeof(line[0]));
	size_t line_p = 0;

	for(size_t i = 0; i < buf_size-1 ; i++){
		char c = *(buf + i);

		if (c != '\n')
			line[line_p++] = c;

		if (c == '\n' || line_p == VGA_MAX_COLS - 1){
			line[++line_p] = '\0';
			ringbuffer_put(&msg_buffer, line, line_p);
			memset(line, '\0', line_p);
			line_p = 0;
		}
	}

	if (line_p > 0)
		ringbuffer_put(&msg_buffer, line, line_p);

	clear_console();
	ringbuffer_for_each(&msg_buffer, &write_line_to_dma);

	update_cursor(row, 0);
}
