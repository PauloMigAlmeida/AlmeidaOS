/*
 * rtc.c
 *
 *  Created on: 16/12/2021
 *      Author: Paulo Almeida
 */

#include "kernel/time/rtc.h"
#include "kernel/asm/generic.h"

#define RTC_MILLISEC_IN_SEC         1000
#define RTC_MILLISEC_IN_MIN         RTC_MILLISEC_IN_SEC  * 60
#define RTC_MILLISEC_IN_HOUR        RTC_MILLISEC_IN_MIN  * 60
#define RTC_MILLISEC_IN_DAY         RTC_MILLISEC_IN_HOUR * 24
#define RTC_MILLISEC_YEAR           31536000000
#define RTC_MILLISEC_LEAP_YEAR      31622400000

static long month[12] = {
        0,
        RTC_MILLISEC_IN_DAY * (31L),
        RTC_MILLISEC_IN_DAY * (31L + 28L),
        RTC_MILLISEC_IN_DAY * (31L + 28L + 31L),
        RTC_MILLISEC_IN_DAY * (31L + 28L + 31L + 30L),
        RTC_MILLISEC_IN_DAY * (31L + 28L + 31L + 30L + 31L),
        RTC_MILLISEC_IN_DAY * (31L + 28L + 31L + 30L + 31L + 30L),
        RTC_MILLISEC_IN_DAY * (31L + 28L + 31L + 30L + 31L + 30L + 31L),
        RTC_MILLISEC_IN_DAY * (31L + 28L + 31L + 30L + 31L + 30L + 31L + 31L),
        RTC_MILLISEC_IN_DAY * (31L + 28L + 31L + 30L + 31L + 30L + 31L + 31L + 30L),
        RTC_MILLISEC_IN_DAY * (31L + 28L + 31L + 30L + 31L + 30L + 31L + 31L + 30L + 31L),
        RTC_MILLISEC_IN_DAY * (31L + 28L + 31L + 30L + 31L + 30L + 31L + 31L + 30L + 31L + 30L)
};

static cmos_clock_t startup_time;
uint64_t rtc_curr_time;

void rtc_init(void) {
    /* meant to be called when inturrupts are enabled */
    startup_time = cmos_read_rtc();
}

cmos_clock_t rtc_startup_time(void) {
    return startup_time;
}

static bool is_leap_year(int year) {
    bool ret;
    if (year % 400 == 0) {
        ret = true;
    } else if (year % 100 == 0) {
        ret = false;
    } else if (year % 4 == 0) {
        ret = true;
    } else {
        ret = false;
    }
    return ret;
}

void rtc_init_curr_time(void) {
    /* pigback on already calculated stuff */
    rtc_curr_time = CMOS_ELAPSED_MSECS_UNTIL_20_CENTURY;

    /* critical path - start*/
    disable_interrupts();

    /* read from the RTC */
    cmos_clock_t now = cmos_read_rtc();

    /* critical path - end*/
    enable_interrupts();

    /* calculate the diff - previous years */
    for (uint8_t i = 0; i < now.year; i++) {
        if (is_leap_year(2000 + i))
            rtc_curr_time += RTC_MILLISEC_LEAP_YEAR;
        else
            rtc_curr_time += RTC_MILLISEC_YEAR;
    }

    /* calculate the diff - current year - month */
    rtc_curr_time += month[now.month - 1];

    /* adjust leap year calc if needed */
    if (now.month > 2 && is_leap_year(2000 + now.year))
        rtc_curr_time += RTC_MILLISEC_IN_DAY;

    /* days */
    rtc_curr_time += RTC_MILLISEC_IN_DAY * (now.day - 1);
    /* hours */
    rtc_curr_time += RTC_MILLISEC_IN_HOUR * now.hour;
    /* minutes */
    rtc_curr_time += RTC_MILLISEC_IN_MIN * now.minute;
    /* seconds */
    rtc_curr_time += RTC_MILLISEC_IN_SEC * now.second;

}

