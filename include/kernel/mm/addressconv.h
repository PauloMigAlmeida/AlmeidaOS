/*
 * addressconv.h
 *
 *  Created on: Aug 13, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_MM_ADDRESSCONV_H_
#define INCLUDE_KERNEL_MM_ADDRESSCONV_H_

#include "kernel/compiler/freestanding.h"

/* Kernel is mapped to the first canonical address of the higher-half which points to physical address 0  */
#define VIRT_ADDR_KERNEL_START      0xffff800000000000

/* this macro doesn't perform sanity checks when converting but this does allows for .text reduction during
 * pre-compile phase. This is meant to be used only for a few reasonable cases, everything else should go
 * through the va() function */
#define UNSAFE_VA(phys_add)         (VIRT_ADDR_KERNEL_START + phys_add)

/* this macro doesn't perform sanity checks when converting but this does allows for .text reduction during
 * pre-compile phase. This is meant to be used only for a few reasonable cases, everything else should go
 * through the pa() function */
#define UNSAFE_PA(virt_add)         (virt_add - VIRT_ADDR_KERNEL_START)

uint64_t va(uint64_t phys_addr);
uint64_t pa(uint64_t virt_addr);

#endif /* INCLUDE_KERNEL_MM_ADDRESSCONV_H_ */
