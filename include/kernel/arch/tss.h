/*
 * tss.h
 *
 *  Created on: 18/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_ARCH_TSS_H_
#define INCLUDE_KERNEL_ARCH_TSS_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"

typedef struct {
    uint32_t res0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t res1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t res2;
    uint16_t res3;
    uint16_t iopb;
} __packed tss_t;

#endif /* INCLUDE_KERNEL_ARCH_TSS_H_ */
