/*
 * mem.h
 *
 *  Created on: Jul 27, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_ARCH_MEM_H_
#define INCLUDE_KERNEL_ARCH_MEM_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"

/*
 *  Address Range Descriptor Structure
 *
 *  Offset in Bytes     Name            Description
 *  0                   BaseAddrLow     Low 32 Bits of Base Address
 *  4                   BaseAddrHigh    High 32 Bits of Base Address
 *  8                   LengthLow       Low 32 Bits of Length in Bytes
 *  12                  LengthHigh      High 32 Bits of Length in Bytes
 *  16                  Type            Address type of  this range.
 *
 */

typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
} __packed mem_map_region_t;

typedef struct {
    uint64_t num_entries;
    mem_map_region_t mem_region[];
} __packed mem_map_block_t;

typedef struct {
   uint64_t phys_avail_mem;
   uint64_t phys_free_mem;
} mem_phys_stats_t;

void mem_init(void);
void mem_print_entries(void);
mem_phys_stats_t mem_stat(void);
mem_map_region_t mem_alloc_region(uint64_t phys_start_addr, uint64_t phys_end_addr);


#endif /* INCLUDE_KERNEL_ARCH_MEM_H_ */
