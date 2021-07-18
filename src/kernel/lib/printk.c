/*
 * printk.c
 *
 *  Created on: Jun 27, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/lib/printk.h"
#include "kernel/video/vga_console.h"
#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"
#include "kernel/lib/string.h"
#include "kernel/lib/vsnprintf.h"

/*
 * Things To Do:
 * - TODO tweak screen so the last line is reserved for keyboard to type
 */

static char *logging_level;
static char buffer[1024];

void printk_init(char *level) {
    //TODO add sanity checks
    logging_level = level;
}

static bool should_print(const char *fmt) {
    //TODO find a way to abort in case printk is used in its raw format
    return strlen(fmt) > 0 && fmt[0] >= logging_level[0];
}

void printk(const char *fmt, ...) {
    if (should_print(fmt)) {
        /* move beyond logging level */
        fmt++;

        size_t buffer_size = ARR_SIZE(buffer);
        memset(buffer, '\0', buffer_size);

        va_list args;
        va_start(args, fmt);
        size_t buf_pointer = vsnprintf(buffer, buffer_size - 1, fmt, args);
        va_end(args);

        write_console(buffer, buf_pointer + 1); // copy nul-terminator too
    }
}

