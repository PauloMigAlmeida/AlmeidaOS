/*
 * pic.h
 *
 *  Created on: Jul 11, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_ARCH_PIC_H_
#define INCLUDE_KERNEL_ARCH_PIC_H_

#include "kernel/compiler/freestanding.h"

/*  General IBM-PC Compatible Interrupt Information - Standard ISA IRQs  */
#define PIC_PROG_INT_TIMER_INTERRUPT        0
#define PIC_KEYBOARD_INTERRUPT              1
#define PIC_COM2_INTERRUPT                  3
#define PIC_COM1_INTERRUPT                  4
#define PIC_LPT2_INTERRUPT                  5
#define PIC_FLOPPY_DISK_INTERRUPT           6
#define PIC_LPT1_OR_SPURIOUS_INTERRUPT      7
#define PIC_CMOS_REALTIME_CLOCK_INTERRUPT   8
#define PIC_PS2_MOUSE_INTERRUPT             12
#define PIC_FPU_COPROC_INTERPROC_INTERRUPT  13
#define PIC_PRIMARY_ATA_HD_INTERRUPT        14
#define PIC_SECONDARY_ATA_HD_INTERRUPT      15

void pic_init(void);
void pic_send_eoi(uint8_t isa_irq);
void pic_unmask_irq(uint8_t isa_irq);
void pic_mask_irq(uint8_t isa_irq);
uint16_t pic_read_isr(void);

void enable_keyboard_irq(void);

#endif /* INCLUDE_KERNEL_ARCH_PIC_H_ */
