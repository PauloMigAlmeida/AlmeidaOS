/*
 * addressconv.c
 *
 *  Created on: Aug 13, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/addressconv.h"
#include "kernel/compiler/bug.h"

/* For the time being I will only deal with 4-level page tables so 2^48 is the memory frontier */
#define PHYS_ADDR_MAX_BOUNDARY      (UINT64_MAX >> (sizeof(uint64_t) * CHAR_BIT - 48))

/* convert physical address to virtual address */
uint64_t va(uint64_t phys_addr) {
    BUG_ON(phys_addr > PHYS_ADDR_MAX_BOUNDARY);
    return PHYS_ADDR_KERNEL_START + phys_addr;
}

/* convert virtual address to physical address */
uint64_t pa(uint64_t virt_addr) {
    BUG_ON(virt_addr < PHYS_ADDR_MAX_BOUNDARY);
    return virt_addr - PHYS_ADDR_KERNEL_START;
}
