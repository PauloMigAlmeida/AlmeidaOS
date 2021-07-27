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

extern uint32_t e820_mem_start;
extern uint32_t e820_mem_end;

typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
} __packed sys_addr_map_t;

static sys_addr_map_t mem_entries[128];
static size_t entry_amount;

static void read_bios_mem_map(void) {
    size_t max_pos = ARR_SIZE(mem_entries);
    sys_addr_map_t *reserved_mem_space = (sys_addr_map_t*) e820_mem_start;

    for (size_t cur_pos = 0; cur_pos < max_pos; cur_pos++) {
        memcpy(mem_entries + cur_pos, reserved_mem_space, sizeof(sys_addr_map_t));

        /* check if we've read all entries as they are zero'ed out during the initialization */
        if (reserved_mem_space->type == 0) {
            entry_amount = cur_pos;
            printk_info("read_bios_mem_map routine read %u entries", entry_amount);
            break;
        }
        reserved_mem_space++;
    }
}

static void print_mem_regions(void) {
    for (size_t cur_pos = 0; cur_pos < entry_amount; cur_pos++) {
        printk_info("Addr: 0x%.16llx - 0x%.16llx, Length (KB): %u, Type:%u", mem_entries[cur_pos].base_addr,
                mem_entries[cur_pos].base_addr + mem_entries[cur_pos].length - 1, mem_entries[cur_pos].length / 1024,
                mem_entries[cur_pos].type);
    }
}

void mem_init(void) {
    printk_info("e820_mem_start: 0x%.8x e820_mem_end: 0x%.8x", e820_mem_start, e820_mem_end);

    /* read the memory map generated in real mode */
    read_bios_mem_map();

    /* print memory map */
    print_mem_regions();
}

