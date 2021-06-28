/*
 * vga_console.c
 *
 *  Created on: Jun 28, 2021
 *      Author: Paulo Almeida
 */
#include "kernel/lib/vga_console.h"
#include <stdint.h>

#define VIDEO_MEM_ADDR 0xb8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static int row = 0;
static int col = 0;

//TODO move this to a dedicated file
inline __attribute__((always_inline)) void outb(uint16_t port, uint8_t value)
{
    asm volatile (
        "outb  %[p],  %[v]"
        :
        : [p] "Nd" (port), [v] "a" (value)
        );
}

void update_cursor(int row, int col) {
	// ensures that caret position is always visible even on certain edge cases
	if(row == VGA_HEIGHT)
		row--;
	else if(row == 0)
		row++;

	uint16_t pos = row * VGA_WIDTH + col;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void clear_console() {
	int nchars = VGA_WIDTH * VGA_HEIGHT;
	volatile char *video_address = (volatile char*) VIDEO_MEM_ADDR;
	for(int i = 0; i < nchars; i++ ){
		video_address[0] = ' ';
		video_address[1] = 0xf;
		video_address += 2;
	}

	row = 0;
	col = 0;
	update_cursor(row,col);
}

void write_console(const char *buf) {
	//TODO implement a way to push the lines up to make space and give the visual effect that the content is scrolling
	char c = *buf;
	do {
		if (col == VGA_WIDTH || c == '\n') {
			col = 0;
			row++;

			if (c == '\n')
				continue;
		}

		if (row == VGA_HEIGHT) {
			col = 0;
			row = 0;
			buf--;
			continue;
		}

		char *video_address = (char*) VIDEO_MEM_ADDR;
		video_address += row * VGA_WIDTH * 2 + col * 2;

		video_address[0] = *buf;
		video_address[1] = 0xf;

		col++;

	} while ((c = *(++buf)) != '\0');

	update_cursor(row, col);
}
