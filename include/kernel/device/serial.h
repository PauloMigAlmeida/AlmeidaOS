/*
 * serial.h
 *
 *  Created on: 12/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_DEVICE_SERIAL_H_
#define INCLUDE_KERNEL_DEVICE_SERIAL_H_

#include "kernel/compiler/freestanding.h"

void init_serial(void);
void write_char_serial(char a);
void write_string_serial(char* arr, size_t length);

#endif /* INCLUDE_KERNEL_DEVICE_SERIAL_H_ */
