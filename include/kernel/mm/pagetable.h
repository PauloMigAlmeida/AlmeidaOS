/*
 * pagetable.h
 *
 *  Created on: 31/10/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_MM_PAGETABLE_H_
#define INCLUDE_KERNEL_MM_PAGETABLE_H_

#include "kernel/compiler/freestanding.h"

typedef struct {
    uint64_t phys_root;     ///< Physical address of root page table (PML4T) entry
    uint64_t virt_root;     ///< Virtual address of root page table (PML4T) entry
} pagetable_t;

pagetable_t* kernel_pagetable(void);

#endif /* INCLUDE_KERNEL_MM_PAGETABLE_H_ */
