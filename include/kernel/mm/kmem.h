/*
 * kmem.h
 *
 *  Created on: 19/08/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_MM_KMEM_H_
#define INCLUDE_KERNEL_MM_KMEM_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/arch/mem.h"


#define KMEM_DEFAULT            (1 << 0)
#define KMEM_RAW_ALLOC          (1 << 1)
#define KMEM_ZERO               (1 << 2)

void kmem_init(mem_map_region_t k_mem_header_rg, mem_map_region_t k_mem_content_rg);
void* kmalloc(uint64_t bytes, int flags);
void kfree(void *ptr);

#endif /* INCLUDE_KERNEL_MM_KMEM_H_ */
