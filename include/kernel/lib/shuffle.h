/*
 * shuffle.h
 *
 *  Created on: Jul 30, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_LIB_SHUFFLE_H_
#define INCLUDE_KERNEL_LIB_SHUFFLE_H_

#include "kernel/compiler/freestanding.h"

void shuffle(void *arr, size_t arr_length, size_t item_width);

#endif /* INCLUDE_KERNEL_LIB_SHUFFLE_H_ */
