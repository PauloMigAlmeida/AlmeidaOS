/*
 * mem.c
 *
 *  Created on: Jul 27, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/arch/mem.h"
#include "kernel/lib/printk.h"
#include "kernel/lib/string.h"
#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"

extern uintptr_t e820_mem_start;
extern uintptr_t e820_mem_end;

/*
 *  Address Range Descriptor Structure
 *
 *  Offset in Bytes     Name        Description
 *  0                   BaseAddrLow     Low 32 Bits of Base Address
 *  4                   BaseAddrHigh    High 32 Bits of Base Address
 *  8                   LengthLow       Low 32 Bits of Length in Bytes
 *  12                  LengthHigh      High 32 Bits of Length in Bytes
 *  16                  Type        Address type of  this range.
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

static volatile mem_map_block_t *sys_addr_mem_regions;

static void print_mem_regions(void) {
    for (size_t cur_pos = 0; cur_pos < sys_addr_mem_regions->num_entries; cur_pos++) {
        mem_map_region_t mem_region = sys_addr_mem_regions->mem_region[cur_pos];

        printk_info("Addr: 0x%.16llx - 0x%.16llx, Length (KB): %u, Type:%u", mem_region.base_addr,
                mem_region.base_addr + mem_region.length - 1,
                mem_region.length / 1024,
                mem_region.type);
    }
}

void mem_init(void) {
    printk_info("e820_mem_start: 0x%.8lx e820_mem_end: 0x%.8lx", e820_mem_start, e820_mem_end);
    sys_addr_mem_regions = (volatile mem_map_block_t*) e820_mem_start;
    printk_info("read_bios_mem_map routine read %llu entries", sys_addr_mem_regions->num_entries);

    /* print memory map */
    print_mem_regions();
}
