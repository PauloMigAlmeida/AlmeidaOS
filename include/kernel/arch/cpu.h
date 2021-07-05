/*
 * cpu.h
 *
 *  Created on: Jul 5, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_ARCH_CPU_H_
#define INCLUDE_KERNEL_ARCH_CPU_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"

void cpu_init();
void enable_intel_faststring();

#endif /* INCLUDE_KERNEL_ARCH_CPU_H_ */
