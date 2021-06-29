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
 *
 * TODO: Maybe I can think of an implementation that can push that to VGA DMA for
 * debugging purposes but it doesn't get added to the buffer
 */

void ringbuffer_init(ringbuffer_t *buf) {
	buf->head= buf->tail = -1;
}

/* it's expected that char* item is NUL-terminated. */
void ringbuffer_put(ringbuffer_t *buf, const char *item, size_t size) {
	if (buf->head == buf->tail && buf->head == -1) {
		//first time
		buf->head++;
		buf->tail++;
	}else if(buf->head == 0 && buf->tail < (int)size) {
		// until it gets full for the first time
		buf->tail++;
	}else {
		// regular case (assuming no deletions are made)
		buf->head = (buf->head + 1) % buf-> size;
		buf->tail = (buf->tail + 1) % buf-> size;
	}
	memcpy(buf->data[buf->tail], (void*)item, size);
}

