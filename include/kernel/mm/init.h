/*
 * init.h
 *
 *  Created on: 18/08/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_MM_INIT_H_
#define INCLUDE_KERNEL_MM_INIT_H_

/* Size contants */

#define ELF_TEXT_OFFSET         0x1000
#define PAGE_SIZE               0x1000
#define PAGEFRAME_SIZE          0x1000
#define STACK_SIZE              PAGE_SIZE * 2

/* Virtual Memory Mapping -> More at: /include/boot/global/mem.asm */
#define K_VIRT_START_ADDR           0xffff800000000000

/* Functions */

void mm_init(void);

#endif /* INCLUDE_KERNEL_MM_INIT_H_ */
