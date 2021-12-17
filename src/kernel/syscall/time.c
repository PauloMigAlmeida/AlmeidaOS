/*
 * time.c
 *
 *  Created on: 16/12/2021
 *      Author: Paulo Almeida
 */

#include "kernel/syscall/time.h"
#include "kernel/arch/cmos.h"
#include "kernel/asm/generic.h"
#include "kernel/time/rtc.h"

time_t sys_time(void) {
    time_t ret = rtc_curr_unixtime;

    /* convert it to UNIX epoch time (seconds) */
    return ret / 1000;
}
