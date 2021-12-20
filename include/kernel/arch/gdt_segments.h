/*
 * gdt_segments.h
 *
 *  Created on: 13/11/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_ARCH_GDT_SEGMENTS_H_
#define INCLUDE_KERNEL_ARCH_GDT_SEGMENTS_H_


/* Segment selector values for segment registers. */
#define GDT64_SEGMENT_SELECTOR_KERNEL_CODE  0x08
#define GDT64_SEGMENT_SELECTOR_KERNEL_DATA  0x10
#define GDT64_SEGMENT_SELECTOR_USER_DATA    0x18
#define GDT64_SEGMENT_SELECTOR_USER_CODE    0x20
#define GDT64_SEGMENT_SELECTOR_TSS          0x28

#define DPL_RING_3  0x03
#define DPL_RING_0  0x03


#endif /* INCLUDE_KERNEL_ARCH_GDT_SEGMENTS_H_ */
