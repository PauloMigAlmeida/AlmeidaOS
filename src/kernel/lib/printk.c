/*
 * printk.c
 *
 *  Created on: Jun 27, 2021
 *      Author: Paulo Almeida
 */
#include "kernel/lib/printk.h"
#include "kernel/lib/string.h"
#include <stdarg.h>
#include <stdbool.h>

static int row = 0;
static int col = 0;

void write_to_console(const char* buf){
	//TODO implement a way to push the lines up to make space and give the visual effect that the content is scrolling
	char c = *buf;
	do{
		// 0xb8000 supports 80 x 25
		if(col == 80 || c == '\n'){
			col = 0;
			row++;

			if(c == '\n') continue;
		}

		if(row > 24){
			col = 0;
			row = 0;
			buf--;
			continue;
		}

		char* video_address = (char*)(0xb8000 + row * 80 * 2  + col * 2);
		video_address[0] = *buf;
		video_address[1] = 0x0a;

		col++;

	}while((c = *(++buf)) != '\0');
}

void printk(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	char buffer[1024];
	memset(buffer, '\0', sizeof(buffer)/sizeof(buffer[0]));
	size_t buf_pointer = 0;
	bool format_toggle = false;

	/*
	 * compiler add the NUL character for this statically defined
	 * strings as per the standard https://stackoverflow.com/a/4348188
	 *
	 * I need to be extra careful with dynamic ones
	 */
	for (int i = 0; fmt[i] != '\0'; i++) {
		char c = fmt[i];

		if (c == '%'){
			format_toggle = true;
			continue;
		}

		if (format_toggle){
			int value = 0;
			switch(c){
				case 'd':
					value = va_arg(args, int);
					itoa(value, buffer+buf_pointer, 10);
					break;
				//TODO test hex format
				case 'x':
					value = va_arg(args, int);
					itoa(value, buffer+buf_pointer, 16);
					break;
				//TODO implement other string format options
			}
			format_toggle = false;
		}else {
			buffer[buf_pointer] = c;
		}

		// fast forward to adjust buf_pointer
		for(;*(buffer +buf_pointer) != '\0'; buf_pointer++);
	}

	va_end(args);
	write_to_console(buffer);
}

