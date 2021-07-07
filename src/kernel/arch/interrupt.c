/*
 * interrupt.c
 *
 *  Created on: Jul 7, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/arch/interrupt.h"
#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/printk.h"
#include "kernel/lib/bit.h"

struct idt_entry {
    uint16_t low_offset;            // offset bits 0..15
    uint16_t target_selector;       // a code segment selector in GDT or LDT
    uint8_t ist_ign_res;            // IST, Reserver, IGN
    uint8_t attr;                   // attributes ( P=1, DPL=00b, S=0, type=1110b )
    uint16_t mid_offset;            // offset bits 16..31
    uint32_t high_offset;           // offset bits 32..63
    uint32_t reserved;              //reserved / unused
} __packed;
typedef struct idt_entry idt_entry_t;

struct idt_pointer {
    uint16_t limit;
    uint64_t addr;
} __packed;
typedef struct idt_pointer idt_pointer_t;

typedef unsigned long long int uword_t;

struct interrupt_frame
{
    uword_t ip;
    uword_t cs;
    uword_t flags;
    uword_t sp;
    uword_t ss;
};


// external references (from the start.asm file)
extern idt_entry_t idt64_table[256];
extern idt_pointer_t idt64_table_pointer;

// function declarations
static void load_idt(idt_pointer_t *idt_ptr);
static void config_idt_vector(uint8_t vector_id, void(*fn)(struct interrupt_frame *frame));
__interrupt static void divide_by_zero_handler(struct interrupt_frame *frame);

void idt_init(void) {
    config_idt_vector(0, &divide_by_zero_handler);
    printk("Loading IDT");
    load_idt(&idt64_table_pointer);
    printk("Enabling interruptions");
}

static void config_idt_vector(uint8_t vector_id, void(*fn)(struct interrupt_frame *frame)) {
    idt_entry_t entry = {
          .low_offset = extract_bit_chunk(0, 15, (uint64_t)fn),
          .target_selector = 8, // 0000100
          .ist_ign_res = 0,
          .attr = 0x8e , // attributes ( P=1, DPL=00b, S=0, type=1110b )
          .mid_offset = extract_bit_chunk(16, 31, (uint64_t)fn),
          .high_offset = extract_bit_chunk(32, 63, (uint64_t)fn),
          .reserved = 0
    };
    idt64_table[vector_id] = entry;
}

__interrupt static void divide_by_zero_handler(struct interrupt_frame *frame)  {
    printk("Really? Divide by zero bruh?");
    frame->ip+=4;
}

__force_inline static void load_idt(idt_pointer_t *idt_ptr) {
    asm volatile (
            "lidt %0 \n"
            :
            : "m" (*idt_ptr)
            : "memory"
    );
}
