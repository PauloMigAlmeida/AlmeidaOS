/*
 * printk.c
 *
 *  Created on: Jun 27, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/lib/printk.h"
#include "kernel/video/vga_console.h"
#include "kernel/compiler/macro.h"
#include "kernel/lib/string.h"
#include "kernel/lib/vsnprintf.h"

/*
 * Things To Do:
 * - TODO Implement multiple debugging levels. Be able to specify the debug level at boot
 * - TODO tweak screen so the last line is reserved for keyboard to type
 */

void printk(const char *fmt, ...) {
    char buffer[1024];
    size_t buffer_size = ARR_SIZE(buffer);
    memset(buffer, '\0', buffer_size);

    va_list args;
    va_start(args, fmt);
    size_t buf_pointer = vsnprintf(buffer, buffer_size - 1, fmt, args);
    va_end(args);

    write_console(buffer, buf_pointer + 1); // copy nul-terminator too
}

