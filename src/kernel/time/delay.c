/*
 * delay.c
 *
 *  Created on: 14/12/2021
 *      Author: Paulo Almeida
 */

#include "kernel/time/delay.h"
#include "kernel/time/jiffies.h"
#include "kernel/lib/printk.h"

unsigned long loops_per_jiffy = (1 << 12);

static void __delay(unsigned long loops_num) {

    asm volatile(
            ".loop: \n"
            "   test %[loop_idx], %[loop_idx] \n"
            "   jz .done \n"
            "   dec %[loop_idx] \n"
            "   jmp .loop \n"
            ".done: \n"
            :
            : [loop_idx] "r" (loops_num)
            :
    );
}

void calibrate_delay(void) {
    unsigned long ticks;

    while ((loops_per_jiffy <<= 1) != 0) {
        ticks = jiffies;

        /* wait for "start of" clock tick */
        while (ticks == jiffies)
            /* nothing */;

        /* Go .. */
        ticks = jiffies;
        __delay(loops_per_jiffy);
        ticks = jiffies - ticks;
        if (ticks)
            break;
    }
}

void ndelay(unsigned long nsecs) {
    unsigned long loops = (nsecs * HZ * loops_per_jiffy) / 1000000000;
    __delay(loops);
}

void udelay(unsigned long usecs) {
    unsigned long loops = (usecs * HZ * loops_per_jiffy) / 1000000;
    __delay(loops);
}



