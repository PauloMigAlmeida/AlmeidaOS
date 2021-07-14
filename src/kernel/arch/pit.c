/*
 * pit.c
 *
 *  Created on: Jul 13, 2021
 *      Author: Paulo Almeida
 */
#include "kernel/arch/pit.h"
#include "kernel/arch/pic.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/printk.h"

/*
 * Notes for myself:
 *
 * -> Channel 0 is connected directly to IRQ0, so it is best to use it only for purposes that should generate
 *      interrupts. Channel 1 is unusable, and may not even exist. Channel 2 is connected to the PC speaker, but can be
 *      used for other purposes without producing audible speaker tones. (I will only use Channel 0 -> IRQ 0 -> PIC)
 *
 * -> The PIT has only 16 bits that are used as frequency divider, which can represent the values from 0 to 65535.
 *       (or 10000 when programmed in BCD mode).
 *
 * -> The "BCD/Binary" bit determines if the PIT channel will operate in binary mode or BCD mode (where each 4 bits of
 *      the counter represent a decimal digit, and the counter holds values from 0000 to 9999). 80x86 PCs only use
 *      binary mode (BCD mode is ugly and limits the range of counts/frequencies possible)
 *
 * -> For reference:
 *              Hertz   Seconds       Cycles Per Second
 *              1 hz      1 second    1 cycle/sec
 *              2 hz    0.5 seconds  2 cycles/sec
 *              ...
 *
 *      PIT Mode/Command register details
 *      Bits         Usage
 *      6 and 7      Select channel :
 *                      0 0 = Channel 0
 *                      0 1 = Channel 1
 *                      1 0 = Channel 2
 *                      1 1 = Read-back command (8254 only)
 *      4 and 5      Access mode :
 *                      0 0 = Latch count value command
 *                      0 1 = Access mode: lobyte only
 *                      1 0 = Access mode: hibyte only
 *                      1 1 = Access mode: lobyte/hibyte
 *      1 to 3       Operating mode :
 *                      0 0 0 = Mode 0 (interrupt on terminal count)
 *                      0 0 1 = Mode 1 (hardware re-triggerable one-shot)
 *                      0 1 0 = Mode 2 (rate generator)
 *                      0 1 1 = Mode 3 (square wave generator)
 *                      1 0 0 = Mode 4 (software triggered strobe)
 *                      1 0 1 = Mode 5 (hardware triggered strobe)
 *                      1 1 0 = Mode 2 (rate generator, same as 010b)
 *                      1 1 1 = Mode 3 (square wave generator, same as 011b)
 *      0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
 */

#define PIT_CHANNEL_0_DATA_PORT 0x40        /* read/write */
#define PIT_CHANNEL_1_DATA_PORT 0x41        /* read/write */
#define PIT_CHANNEL_2_DATA_PORT 0x42        /* read/write */
#define PIT_MODE_CMD_REG        0x43        /* write only / read is ignored */
#define PIT_CHIP_FREQ           1_193_182   /* Hz (+-) */

#define PIT_BINARY_MODE         0           /* 16-bit binary */
#define PIT_OP_MODE_2           1 << 2      /* Mode 2 (rate generator) */
#define PIT_ACCESS_MODE_LO_HI   3 << 4      /* Access mode: lobyte/hibyte */

void pit_init(uint16_t freq_hz) {
    outb(PIT_MODE_CMD_REG, (uint8_t) (PIT_ACCESS_MODE_LO_HI | PIT_OP_MODE_2 | PIT_BINARY_MODE));
    //TODO calculate desired freq to obtain divider value
    outb(PIT_CHANNEL_0_DATA_PORT, 0x9b);
    outb(PIT_CHANNEL_0_DATA_PORT, 0x2e);
    printk("PIT enabled");

    /* unmask timer interrupt so we can start processing it */
    pic_unmask_irq(PIC_PROG_INT_TIMER_INTERRUPT);

}

void pit_timer_handle_irq(void) {
    //TODO do something useful with that.. I could only write part of this impl - Lunch time at work =S
    pic_send_eoi(PIC_PROG_INT_TIMER_INTERRUPT);
}
