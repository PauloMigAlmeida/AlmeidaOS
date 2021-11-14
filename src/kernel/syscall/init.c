/*
 * init.c
 *
 *  Created on: 13/11/2021
 *      Author: Paulo Almeida
 */

#include "kernel/syscall/init.h"
#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/bug.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/bit.h"
#include "kernel/lib/printk.h"
#include "kernel/arch/msr.h"
#include "kernel/arch/gdt_segments.h"

/*

 From Intel 64 Manual -> Section 5.8.8 -> Fast System calls in 64-bit more

 SYSCALL is intended for use by user code running at privilege level 3 to access operating system or executive
 procedures running at privilege level 0. SYSRET is intended for use by privilege level 0 operating system or execu-
 tive procedures for fast returns to privilege level 3 user code.

 Stack pointers for SYSCALL/SYSRET are not specified through model specific registers. The clearing of bits in
 RFLAGS is programmable rather than fixed. SYSCALL/SYSRET save and restore the RFLAGS register.

 For SYSCALL, the processor saves RFLAGS into R11 and the RIP of the next instruction into RCX; it then gets the
 privilege-level 0 target code segment, instruction pointer, stack segment, and flags as follows:

 - Target code segment — Reads a non-NULL selector from IA32_STAR[47:32].
 - Stack segment — Computed by adding 8 to the value in IA32_STAR[47:32].
 - Target instruction pointer — Reads a 64-bit address from IA32_LSTAR. (The WRMSR instruction ensures
 that the value of the IA32_LSTAR MSR is canonical.)

 - Flags — The processor sets RFLAGS to the logical-AND of its current value with the complement of the value in
 the IA32_FMASK MSR.


 When SYSRET transfers control to 64-bit mode user code using REX.W, the processor gets the privilege level 3
 target code segment, instruction pointer, stack segment, and flags as follows:

 - Target code segment — Reads a non-NULL selector from IA32_STAR[63:48] + 16.
 - Target instruction pointer — Copies the value in RCX into RIP.
 - Stack segment — IA32_STAR[63:48] + 8.
 - EFLAGS — Loaded from R11.

 */

void syscall_handler(void) {
    printk_info("syscall_handler called");
}

static bool is_syscall_inst_supported(void) {
    /* If CPUID.80000001H:EDX[11] = 1 -> SYSCALL and SYSRET are available */
    uint32_t eax = 0x80000001, ebx = 0, ecx = 0, edx = 0;
    cpuid(&eax, &ebx, &ecx, &edx);
    return test_bit(11, edx);
}

void syscall_init(void) {
    /* sanity checks */
    BUG_ON(!is_syscall_inst_supported());

    uint64_t star_reg = rdmsr(MSR_IA32_STAR);
    star_reg &= 0x00000000ffffffff;

    /* SYSRET with RPL set to ring 3 */
    star_reg |= ((uint64_t) GDT64_SEGMENT_SELECTOR_USER_CODE - 16) << 48;
    /* SYSCALL with RPL set to ring 0 */
    star_reg |= ((uint64_t) GDT64_SEGMENT_SELECTOR_KERNEL_CODE) << 32;
    wrmsr(MSR_IA32_STAR, star_reg);

    /* SYSCALL target instruction pointer */
    wrmsr(MSR_IA32_LSTAR, (uint64_t) syscall_handler);

    /* enable syscall */
    uint64_t efer_reg = rdmsr(MSR_IA32_EFER);
    set_bit(0, efer_reg);
    wrmsr(MSR_IA32_EFER, efer_reg);

    /* Don't clear RFLAGS during SYSCALL */
    wrmsr(MSR_IA32_FMASK, 0);

    printk_info("SYSCALL/SYSRET initialised");

}
