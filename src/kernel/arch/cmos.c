/*
 * cmos.c
 *
 *  Created on: 15/12/2021
 *      Author: Paulo Almeida
 */

#include "kernel/arch/cmos.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/bit.h"

#define CMOS_INDEX_PORT      0x70
#define CMOS_DATA_PORT       0x71

/*
 * Register  Contents            Range
 *
 * 0x00      Seconds             0–59
 * 0x02      Minutes             0–59
 * 0x04      Hours               0–23 in 24-hour mode or 1–12 in 12-hour mode, highest bit set if pm
 * 0x06      Weekday             1–7, Sunday = 1
 * 0x07      Day of Month        1–31
 * 0x08      Month               1–12
 * 0x09      Year                0–99
 * 0x32      Century (maybe)     19–20? -> This is not a standard register
 * 0x0A      Status Register A
 * 0x0B      Status Register B
 */
#define CMOS_SECS_REG           0x00
#define CMOS_MINS_REG           0x02
#define CMOS_HOURS_REG          0x04
#define CMOS_WEEKDAY_REG        0x06
#define CMOS_DAY_REG            0x07
#define CMOS_MONTH_REG          0x08
#define CMOS_YEAR_REG           0x09
#define CMOS_CENTURY_REG        0x32
#define CMOS_STATUS_A_REG       0x0A
#define CMOS_STATUS_B_REG       0x0B

/* flags */
#define CMOS_UPD_IN_PROG_BIT    0x07
#define CMOS_HOUR_PM_MASK       (1 << 7)
#define CMOS_NMI_DISABLE_MASK   (1 << 7)

/* utility macros */
#define BCD_TO_BIN(bcd) ((bcd / 16) * 10) + (bcd & 0xf)

static uint8_t cmos_read_reg(int reg) {
    reg |= CMOS_NMI_DISABLE_MASK;
    outb(CMOS_INDEX_PORT, reg);
    return inb(CMOS_DATA_PORT);
}

cmos_clock_t cmos_read_rtc(void) {
    cmos_clock_t ret = { 0 };

    do {
        /* wait for next CMOS tick */
        while (test_bit(CMOS_UPD_IN_PROG_BIT, cmos_read_reg(CMOS_STATUS_A_REG)))
            /* nothing */;

        /* read registers */
        ret.second = cmos_read_reg(CMOS_SECS_REG);
        ret.minute = cmos_read_reg(CMOS_MINS_REG);
        ret.hour = cmos_read_reg(CMOS_HOURS_REG);
        ret.day = cmos_read_reg(CMOS_DAY_REG);
        ret.month = cmos_read_reg(CMOS_MONTH_REG);
        ret.year = cmos_read_reg(CMOS_YEAR_REG);

        /* ensure that we got that as part of a single tick */
    } while (ret.second != cmos_read_reg(CMOS_SECS_REG));

    uint8_t reg_b = cmos_read_reg(CMOS_STATUS_B_REG);
    bool bin_mod_set = test_bit(2, reg_b);
    bool fmt24_set = test_bit(1, reg_b);

    if (!bin_mod_set) {
        ret.second = BCD_TO_BIN(ret.second);
        ret.minute = BCD_TO_BIN(ret.minute);
        ret.hour = BCD_TO_BIN(ret.hour);
        ret.day = BCD_TO_BIN(ret.day);
        ret.month = BCD_TO_BIN(ret.month);
        ret.year = BCD_TO_BIN(ret.year);
    }

    /* convert 12 hour clock to 24 hour format if needed */
    if (!fmt24_set && (ret.hour & CMOS_HOUR_PM_MASK)) {
        ret.hour = ((ret.hour & ~CMOS_HOUR_PM_MASK) + 12) % 24;
    }

    return ret;
}

