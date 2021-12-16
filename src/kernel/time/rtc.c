/*
 * rtc.c
 *
 *  Created on: 16/12/2021
 *      Author: Paulo Almeida
 */

#include "kernel/time/rtc.h"

static cmos_clock_t startup_time;

void rtc_init(void) {
    startup_time = cmos_read_rtc();
}

cmos_clock_t rtc_startup_time(void) {
    return startup_time;
}

