/*
 * vga_console.c
 *
 *  Created on: Jun 28, 2021
 *      Author: Paulo Almeida
 */
#include "kernel/lib/vga_console.h"

static int row = 0;
static int col = 0;

void clear_console() {
	// 0xb8000 supports 80 x 25
	int nchars = 80*25;
	char *video_address = (char*) 0xb8000;
	for(int i = 0; i < nchars; i++ ){
		video_address[0] = ' ';
		video_address[1] = 0;
		video_address += 2;
	}
	row = 0;
	col = 0;
	//TODO add ability to move caret position like I did for protected/long modes
}

void write_console(const char *buf) {
	//TODO implement a way to push the lines up to make space and give the visual effect that the content is scrolling
	//TODO add ability to move caret position like I did for protected/long modes
	char c = *buf;
	do {
		// 0xb8000 supports 80 x 25
		if (col == 80 || c == '\n') {
			col = 0;
			row++;

			if (c == '\n')
				continue;
		}

		if (row > 24) {
			col = 0;
			row = 0;
			buf--;
			continue;
		}

		char *video_address = (char*) (0xb8000 + row * 80 * 2 + col * 2);
		video_address[0] = *buf;
		video_address[1] = 0x0a;

		col++;

	} while ((c = *(++buf)) != '\0');
}
