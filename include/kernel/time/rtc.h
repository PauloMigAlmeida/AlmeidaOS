/*
 * rtc.h
 *
 *  Created on: 16/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_TIME_RTC_H_
#define INCLUDE_KERNEL_TIME_RTC_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/arch/cmos.h"

void rtc_init(void);
cmos_clock_t rtc_startup_time(void);
void rtc_init_curr_time(void);

extern uint64_t rtc_curr_time;

#endif /* INCLUDE_KERNEL_TIME_RTC_H_ */
