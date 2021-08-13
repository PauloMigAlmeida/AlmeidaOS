/*
 * addressconv.h
 *
 *  Created on: Aug 13, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_MM_PHYSICALADDRESS_H_
#define INCLUDE_KERNEL_MM_PHYSICALADDRESS_H_

#include "kernel/compiler/freestanding.h"

/* Kernel is mapped to the first canonical address of the higher-half which points to physical address 0  */
#define PHYS_ADDR_KERNEL_START      0xffff800000000000

/* this macro doesn't perform sanity checks when converting but this does allows for .text reduction during
 * pre-compile phase. This is meant to be used only for a few reasonable cases, everything else should go
 * through the va() function */
#define UNSAFE_VA(phys_add)         PHYS_ADDR_KERNEL_START + phys_add

uint64_t va(uint64_t phys_addr);

#endif /* INCLUDE_KERNEL_MM_PHYSICALADDRESS_H_ */
