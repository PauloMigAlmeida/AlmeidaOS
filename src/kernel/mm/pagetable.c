/*
 * pagetable.c
 *
 *  Created on: 31/10/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/pagetable.h"

/* kernel space pagetable */
static pagetable_t k_root_pgt;

pagetable_t* kernel_pagetable(void){
    return &k_root_pgt;
}
