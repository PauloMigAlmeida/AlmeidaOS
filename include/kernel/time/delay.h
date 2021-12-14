/*
 * delay.h
 *
 * Software timers are useless when kernel needs to wait for a short-period
 * of time. When we have to wait for <HZ (default to <1ms) that udelay and
 * ndelay can become handy
 *
 * For this OS since I will be using PIT for now then (x)delay functions will
 * be implemented using tight loops which require calibration during boot time
 *
 * This implementation was inspired by early Linux implementations ( +- 2.6.x)
 *
 *  Created on: 14/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_TIME_DELAY_H_
#define INCLUDE_KERNEL_TIME_DELAY_H_

#include "kernel/compiler/freestanding.h"

extern unsigned long loops_per_jiffy;

void calibrate_delay(void);
void ndelay(unsigned long nsecs);
void udelay(unsigned long usecs);


#endif /* INCLUDE_KERNEL_TIME_DELAY_H_ */
