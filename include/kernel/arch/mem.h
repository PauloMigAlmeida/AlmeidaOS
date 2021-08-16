/*
 * mem.h
 *
 *  Created on: Jul 27, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_ARCH_MEM_H_
#define INCLUDE_KERNEL_ARCH_MEM_H_

#include "kernel/compiler/freestanding.h"

typedef struct {
   uint64_t phys_avail_mem;
   uint64_t phys_free_mem;
} mem_phys_stats_t;

void mem_init(void);
mem_phys_stats_t mem_phys_stat(void);

#endif /* INCLUDE_KERNEL_ARCH_MEM_H_ */
