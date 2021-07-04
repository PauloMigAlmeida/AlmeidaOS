/*
 * vsnprintf.h
 *
 *  Created on: Jul 3, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_LIB_VSNPRINTF_H_
#define INCLUDE_KERNEL_LIB_VSNPRINTF_H_

#include "kernel/compiler/freestanding.h"

size_t vsnprintf(char *buf, size_t buf_size, const char *fmt, va_list args);

#endif /* INCLUDE_KERNEL_LIB_VSNPRINTF_H_ */
