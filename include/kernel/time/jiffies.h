/*
 * jiffies.h
 *
 *  Created on: 14/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_TIME_JIFFIES_H_
#define INCLUDE_KERNEL_TIME_JIFFIES_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"

/*
 * From: Linux Device Drivers v3 -> Chapter 7, Pg 183
 * Most platforms run at 100 or 1000 interrupts per second;
 * the popular x86 PC defaults to 1000
 */
#define HZ  1000

/*
 * Variable that counts loop cycles since boot given HZ.
 * Assuming HZ to be 1000 then it should 'reset' every 6.77 years
 */
extern uint64_t jiffies;


#endif /* INCLUDE_KERNEL_TIME_JIFFIES_H_ */
