/*
 * init.h
 *
 *  Created on: 18/08/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_MM_INIT_H_
#define INCLUDE_KERNEL_MM_INIT_H_

#define ELF_TEXT_OFFSET         0x1000
#define PAGE_SIZE               0x1000

void mm_init(void);


#endif /* INCLUDE_KERNEL_MM_INIT_H_ */
