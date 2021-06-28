/*
 * printk.c
 *
 *  Created on: Jun 27, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/lib/printk.h"
#include "kernel/lib/string.h"
#include "kernel/lib/vga_console.h"
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
	write_console(buffer);
}

