/*
 * cpu.c
 *
 *  Created on: Jul 5, 2021
 *      Author: Paulo Almeida
 */

#include "kernel/arch/cpu.h"
#include "kernel/compiler/freestanding.h"
#include "kernel/compiler/macro.h"
#include "kernel/asm/generic.h"
#include "kernel/arch/msr.h"
#include "kernel/lib/bit.h"
#include "kernel/lib/printk.h"

struct cpu_version_info {
    /* obtained via cpuid */
    uint8_t stepping_id :4;
    uint8_t model_id :4;
    uint8_t family_id :4;
    uint8_t extended_model_id :4;
    uint8_t extended_family_id :8;

    /* calculated dynamically */
    uint16_t display_family :16;
    uint8_t display_model :8;

} __packed;
typedef struct cpu_version_info cpu_version_info;

static cpu_version_info cpu_ver_info = { 0 };

void cpu_init() {
    /* CPUID.01H:EAX -> Returns Model, Family, Stepping Information */
    uint32_t eax = 0x1, ebx = 0, ecx = 0, edx = 0;
    cpuid(&eax, &ebx, &ecx, &edx);

    cpu_ver_info.stepping_id = extract_bit_chunk(0, 3, eax);
    cpu_ver_info.model_id = extract_bit_chunk(4, 7, eax);
    cpu_ver_info.family_id = extract_bit_chunk(8, 11, eax);

    if (cpu_ver_info.family_id != 0xf) {
        cpu_ver_info.display_family = cpu_ver_info.family_id;
    } else {
        cpu_ver_info.extended_family_id = extract_bit_chunk(20, 27, eax);
        cpu_ver_info.display_family = cpu_ver_info.extended_family_id + cpu_ver_info.family_id;
    }

    if (cpu_ver_info.family_id == 0x6 || cpu_ver_info.family_id == 0xf) {
        cpu_ver_info.extended_model_id = extract_bit_chunk(16, 19, eax);
        cpu_ver_info.display_model = ((uint16_t) cpu_ver_info.extended_model_id << 4) + cpu_ver_info.model_id;
    } else {
        cpu_ver_info.display_model = cpu_ver_info.model_id;
    }

    printk("cpu_version_info :: stepping_id: 0x%.4x, model_id: 0x%.4x, family_id: 0x%.4x, extended_model_id: 0x%.4x, "
            "extended_family_id: 0x%.8x, display_family: 0x%.8x, display_model: 0x%.8x", cpu_ver_info.stepping_id,
            cpu_ver_info.model_id, cpu_ver_info.family_id, cpu_ver_info.extended_model_id,
            cpu_ver_info.extended_family_id, cpu_ver_info.display_family, cpu_ver_info.display_model);

    // early intel cpu
    enable_intel_faststring();
}

void enable_intel_faststring() {
    /*
     * From Intel 64 manual -> Volume 4 -> Chapter 2:
     *
     * Model specific registers and its bit-fields may be supported for a finite range of processor families/models.
     * To distinguish between different processor family and/or models, software must use CPUID.01H leaf function to
     * query the combination of DisplayFamily and DisplayModel to determine model-specific availability of MSRs
     *
     * IA32_MISC_ENABLE -> 0F_0H -> Intel Xeon Processor, Intel Xeon processor MP, Intel Pentium 4 processors
     */

    if (is_msr_supported()) {
        if (cpu_ver_info.display_family == 0xf && cpu_ver_info.display_model == 0x0) {
            unsigned long long ia32_misc_enable = rdmsr(MSR_IA32_MISC_ENABLE);
            printk("IA32_MISC_ENABLE is: 0x%.16llx", ia32_misc_enable);
            if(!test_bit(0, ia32_misc_enable)){
                // enable fast-string ourselves
                ia32_misc_enable |= MSR_IA32_MISC_ENABLE_FAST_STRING_BIT;
                wrmsr(MSR_IA32_MISC_ENABLE, ia32_misc_enable);

                // check if it stuck (QEMU is the king of ignoring CPU bits sometimes)
                ia32_misc_enable = rdmsr(MSR_IA32_MISC_ENABLE);
                if(!test_bit(0, ia32_misc_enable)){
                    printk("IA32_MISC_ENABLE (fast-string) couldn't be enabled");
                }else{
                    printk("IA32_MISC_ENABLE (fast-string) enabled manually");
                }
            }else {
                printk("IA32_MISC_ENABLE (fast-string) was enabled by the BIOS");
            }
        } else {
            printk("CPU doesn't support fast-string");
        }

    }
}
