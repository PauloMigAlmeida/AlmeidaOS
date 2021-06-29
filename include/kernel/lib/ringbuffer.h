/*
 * ringbuffer.h
 *
 *  Created on: Jun 29, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_LIB_RINGBUFFER_H_
#define INCLUDE_KERNEL_LIB_RINGBUFFER_H_

#include <stddef.h>

/*
 * I first created that to hold the VGA messages printed by printk..that's why
 * the hard-coded values there.
 *
 * Ideally, I would like that to be a separate reusable ring-buff impl in which
 * users could specify the size of the data, however,
 * I have to implement a kmalloc-like functionality first. Right now I can only
 * have a play with stuff created in the stack during compile time
 *
 * TODO: create kmalloc functionality change this implementation to allow
 * arbitrary size
 */

typedef struct {
	int head;
	int tail;
	size_t size;
	char data[25][1024];
} ringbuffer_tp;

void ringbuffer_init(ringbuffer_tp *buf);
void ringbuffer_put(ringbuffer_tp *buf, const char *item, size_t size);
void ringbuffer_for_each(ringbuffer_tp *buf, void (*fn)(const char* item));

#endif /* INCLUDE_KERNEL_LIB_RINGBUFFER_H_ */
