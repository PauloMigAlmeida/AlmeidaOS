/*
 * cmos.h
 *
 *  Created on: 15/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_ARCH_CMOS_H_
#define INCLUDE_KERNEL_ARCH_CMOS_H_

#include "kernel/compiler/freestanding.h"

typedef struct {
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char day;
    unsigned char month;
    unsigned char year;
} cmos_clock_t;

cmos_clock_t cmos_read_rtc(void);

#endif /* INCLUDE_KERNEL_ARCH_CMOS_H_ */
