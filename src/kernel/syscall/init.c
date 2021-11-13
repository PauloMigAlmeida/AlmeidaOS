/*
 * init.c
 *
 *  Created on: 13/11/2021
 *      Author: Paulo Almeida
 */

#include "kernel/syscall/init.h"
#include "kernel/compiler/freestanding.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/bit.h"
#include "kernel/compiler/bug.h"

static bool is_syscall_inst_supported(void) {
    /* If CPUID.80000001H:EDX[11] = 1 -> SYSCALL and SYSRET are available */
    uint32_t eax = 0x80000001, ebx = 0, ecx = 0, edx = 0;
    cpuid(&eax, &ebx, &ecx, &edx);
    return test_bit(11, edx);
}

void syscall_init(void) {
    /* sanity checks */
    BUG_ON(!is_syscall_inst_supported());

}
