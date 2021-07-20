/*
 * printk.h
 *
 *  Created on: Jun 27, 2021
 *      Author: Paulo Almeida
 */

#ifndef _KERNEL_LIB_PRINTK_H
#define _KERNEL_LIB_PRINTK_H

#include "kernel/compiler/freestanding.h"

void printk_init(const uint8_t level);
void printk(const uint8_t level, const char *format, ...);

#define PRINTK_DEBUG_LEVEL  2
#define PRINTK_INFO_LEVEL   1
#define PRINTK_ERR_LEVEL    0

#define printk_debug(fmt, ...)   printk(PRINTK_DEBUG_LEVEL, fmt, ##__VA_ARGS__)
#define printk_info(fmt, ...)    printk(PRINTK_INFO_LEVEL, fmt, ##__VA_ARGS__)
#define printk_error(fmt, ...)   printk(PRINTK_ERR_LEVEL, fmt, ##__VA_ARGS__)

#endif /* _KERNEL_LIB_PRINTK_H */
