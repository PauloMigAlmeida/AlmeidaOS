/*
 * ringbuffer.c
 *
 *  Created on: Jun 29, 2021
 *      Author: Paulo Almeida
 */
#include "kernel/lib/ringbuffer.h"
#include "kernel/lib/string.h"

/*
 * this ringbuffer implementation is used by printk, which means that we can't
 * make use of printk here due to the circular dependency.
 */

void ringbuffer_init(ringbuffer_tp *buf) {
	buf->head = buf->tail = -1;
}

/* it's expected that char* item is NUL-terminated. */
void ringbuffer_put(ringbuffer_tp *buf, const char *item, size_t size) {
	if (buf->head == buf->tail && buf->head == -1) {
		//first time
		buf->head++;
		buf->tail++;
	} else if (buf->head == 0 && buf->tail < (buf->size -1)) {
		// until it gets full for the first time
		buf->tail++;
	} else {
		// regular case (assuming no deletions are made)
		buf->head = (buf->head + 1) % buf->size;
		buf->tail = (buf->tail + 1) % buf->size;
	}
	memcpy(buf->data[buf->tail], item, size);
}

void ringbuffer_for_each(ringbuffer_tp *buf, void (*fn)(const char *item)) {
	// check whether the ringbuffer is empty
	if ((buf->head == -1 || buf->tail == -1) || buf->size == 0)
		return;

	int idx = buf->head;
	do{
		idx = idx % buf->size;
		fn(buf->data[idx]);
		idx++;
	}while((buf->tail > buf->head && idx <= buf->tail)
			|| (buf->tail < buf->head && idx != buf->head));
	/* scratch pad
	 * h=1 : t=3 -> until gets to tail
	 * h=2 : t=1 -> until gets to head;
	 */
}
