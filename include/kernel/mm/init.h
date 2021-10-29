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

/* Virtual Memory Mapping -> More at: /include/boot/global/mem.asm */
#define K_VIRT_START_ADDR           0xffff800000000000
#define K_VIRT_END_ADDR             0xffff80009fffffff
#define K_VIRT_TEXT_ADDR            0xffff800000000000
#define K_VIRT_MEM_HEADER_ADDR      0xffff800040000000
#define K_VIRT_MEM_CONTENT_ADDR     0xffff800060000000

/* Functions */

void mm_init(void);

#endif /* INCLUDE_KERNEL_MM_INIT_H_ */
