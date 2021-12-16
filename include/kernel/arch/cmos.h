/*
 * cmos.h
 *
 *  Created on: 15/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_ARCH_CMOS_H_
#define INCLUDE_KERNEL_ARCH_CMOS_H_

#include "kernel/compiler/freestanding.h"

/*
 * CMOS century register isn't standard and I'm not
 * willing to implement ACPI now just to get
 * that info :-)
 */
#define CMOS_ELAPSED_MSECS_UNTIL_20_CENTURY   0xdc6acfac00

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
} cmos_clock_t;

cmos_clock_t cmos_read_rtc(void);

#endif /* INCLUDE_KERNEL_ARCH_CMOS_H_ */
