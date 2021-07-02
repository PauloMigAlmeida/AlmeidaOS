/*
 * display_console.h
 *
 *  Created on: Jun 28, 2021
 *      Author: Paulo Almeida
 */

#ifndef _KERNEL_LIB_DISPLAY_CONSOLE_H
#define _KERNEL_LIB_DISPLAY_CONSOLE_H

#include "kernel/compiler/freestanding.h"

void vga_console_init();
void clear_console();
void write_console(const char* buf, size_t buf_size);

#endif /* _KERNEL_LIB_DISPLAY_CONSOLE_H */
