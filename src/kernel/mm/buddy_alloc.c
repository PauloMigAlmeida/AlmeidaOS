/*
 * buddy_alloc.c
 *
 *  Created on: 18/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/buddy_alloc.h"

static mem_map_region_t mem_reg;

void buddy_alloc_init(mem_map_region_t t_mem_reg) {
    mem_reg = t_mem_reg;
}

