/*
 * coredump.h
 *
 *  Created on: Jul 8, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_DEBUG_COREDUMP_H_
#define INCLUDE_KERNEL_DEBUG_COREDUMP_H_

#include "../interrupt/idt.h"
#include "kernel/compiler/freestanding.h"

void coredump(interrupt_stack_frame_t *regs, size_t max_frames);

#endif /* INCLUDE_KERNEL_DEBUG_COREDUMP_H_ */
