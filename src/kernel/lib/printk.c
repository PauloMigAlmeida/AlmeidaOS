/*
 * printk.c
 *
 *  Created on: Jun 27, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/lib/printk.h"
#include "kernel/lib/string.h"
#include "kernel/video/vga_console.h"
#include <stdarg.h>
#include <stdbool.h>


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
			int v_int;
			unsigned int v_uint;
			const char* v_carr;

			switch(c){
				case 'd':
				case 'i':
					v_int = va_arg(args, int);
					itoa(v_int, buffer+buf_pointer, 10);
					break;
				case 'u':
					v_uint = va_arg(args, unsigned int);
					utoa(v_uint, buffer+buf_pointer, 10);
					break;
				case 'x':
					v_uint = va_arg(args, unsigned int);
					utoa(v_uint, buffer+buf_pointer, 16);
					break;
				case 'o':
					v_uint = va_arg(args, unsigned int);
					utoa(v_uint, buffer+buf_pointer, 8);
					break;
				case 'c':
					v_int = va_arg(args, int);
					/* 'char' is promoted to 'int' when passed through '...' */
					buffer[buf_pointer] = (char)v_int;
					break;
				case 's':
					v_carr = va_arg(args, const char*);
					memcpy(buffer+buf_pointer, v_carr, strlen(v_carr)+1);
					break;
			}
			format_toggle = false;
		}else {
			buffer[buf_pointer] = c;
		}

		// fast forward to adjust buf_pointer
		for(;*(buffer +buf_pointer) != '\0'; buf_pointer++);
	}

	va_end(args);
	write_console(buffer, buf_pointer + 1); // copy nul-terminator too
}

