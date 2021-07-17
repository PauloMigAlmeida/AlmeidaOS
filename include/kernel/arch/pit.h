/*
 * pit.h
 *
 *  Created on: Jul 13, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_ARCH_PIT_H_
#define INCLUDE_KERNEL_ARCH_PIT_H_

#include "kernel/compiler/freestanding.h"

void pit_init(uint32_t freq_hz);
void pit_enable(void);
void pit_timer_handle_irq(void);

#endif /* INCLUDE_KERNEL_ARCH_PIT_H_ */
