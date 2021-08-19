/*
 * init.c
 *
 *  Created on: 18/08/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/init.h"
#include "kernel/mm/page.h"
#include "kernel/arch/mem.h"
#include "kernel/mm/kmem.h"


/*
 * TODO: Next steps:
 * - Map kernel space in here so we don't end up offering that memory to something else that can override it
 * - Figure out what to do with the Kernel stack mem? (how to reserve that to avoid data corruption)
 * - Initiate a memory allocator (slab or buddy system)
 * - implement kmalloc
 * - implement vmalloc
 *
 * HOw am I gonna reserve space for the mem allocator? Seems like the chicken and the egg problem
 */

void mm_init(void) {
    /* read/prepare entries returned from BIOS 0xe820 */
    mem_init();

    kmem_init();

    paging_init();
}

