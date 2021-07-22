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

static uint8_t logging_level = PRINTK_INFO_LEVEL;
static char buffer[1024];

void printk_init(const uint8_t level) {
    /* sanity checks */
    if (level > PRINTK_DEBUG_LEVEL) {
        printk_error("Debug level doesn't exist, setting PRINTK_INFO_LEVEL");
        logging_level = PRINTK_INFO_LEVEL;
    } else {
        logging_level = level;
    }
}

void printk(const uint8_t level, const char *fmt, ...) {
    /* check the configured logging level */
    if (level > logging_level)
        return;

    size_t buffer_size = ARR_SIZE(buffer);
    memset(buffer, '\0', buffer_size);

    va_list args;
    va_start(args, fmt);
    size_t buf_pointer = vsnprintf(buffer, buffer_size - 1, fmt, args);
    va_end(args);

    write_console(buffer, buf_pointer + 1); // copy nul-terminator too
}

