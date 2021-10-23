/*
 * ringbuffer.h
 *
 *  Created on: Jun 29, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_LIB_RINGBUFFER_H_
#define INCLUDE_KERNEL_LIB_RINGBUFFER_H_

#include "kernel/compiler/freestanding.h"

/*
 * Notes:
 *
 * On 21/06/2021:
 *
 * I first created that to hold the VGA messages printed by printk..that's why
 * the hard-coded values there.
 *
 * Ideally, I would like that to be a separate reusable ring-buff impl in which
 * users could specify the size of the data, however,
 * I have to implement a kmalloc-like functionality first. Right now I can only
 * have a play with stuff created in the stack during compile time
 *
 * On 24/10/2021:
 *
 * UPDATE: I implemented the kmalloc functionality but I use printk for debugging
 * purposes as the mm/init.c is very critical. This creates a cyclic dependecy
 * in which I'm not willing to give up my only debugging method (printk) just to
 * not have this struct sitting on the stack.
 *
 */

typedef struct {
	int head;
	int tail;
	int size;
	char data[24][81]; //VGA DMA Max Columns - 1 + NUL-terminator
} ringbuffer_tp;

void ringbuffer_init(ringbuffer_tp *buf);
void ringbuffer_put(ringbuffer_tp *buf, const char *item, size_t size);
void ringbuffer_for_each(ringbuffer_tp *buf, void (*fn)(const char* item));

#endif /* INCLUDE_KERNEL_LIB_RINGBUFFER_H_ */
