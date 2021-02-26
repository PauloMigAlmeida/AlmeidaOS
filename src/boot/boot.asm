[BITS 16]
[ORG 0x7c00]

;===============================================================================
; boot
;
; First-stage boot loader entry point
;
; The BIOS initialiates the boot process by running the code here in 16-bit
; real mode. The segment registers are all set to zero, and the instruction
; pointer (IP) is 0x7c00.
;
; Input registers:
;
;   AX      Boot signature (should be 0xaa55)
;   DL      Boot drive number
;
; Memory layout before this code starts running:
;
;   00000000 - 000003ff        1,024 bytes     Real mode IVT
;   00000400 - 000004ff          256 bytes     BIOS data area
;   00000500 - 00007bff       30,464 bytes     Free
;   00007c00 - 00007dff          512 bytes     First-stage boot loader (MBR)
;   00007e00 - 0009fbff      622,080 bytes     Free
;   0009fc00 - 0009ffff        1,024 bytes     Extended BIOS data area (EBDA)
;   000a0000 - 000bffff      131,072 bytes     BIOS video memory
;   000c0000 - 000fffff      262,144 bytes     ROM
;===============================================================================

; jump to start procedure
jmp start

; Include functions/constants that are useful in real mode
%include "../../include/boot/realmode.inc"

start:
    ; Proper initialisation of stack during BIOS bootloader
    ;   https://stackoverflow.com/a/33975465/832748
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov bx, Mem.Stack.Top

    ; Turn off interrupts for SS:SP update to avoid a problem with buggy 8088 CPUs
    cli
    ; SS = 0x0000
    mov ss, ax
    ; SP = 0x7c00
    ; Set the stack starting just below where the bootloader is at 0x0:0x7c00.
    mov sp, bx
    ; Turn interrupts back on
    sti

    ; Save DriveId for later
    mov [BIOS.Drive.Id], dl

boot:
    mov si, Realmode.Booting.Msg
    call display_string

    call bios_check_extensions_present


    ; enter a endless loop. This instruction should never be reached
    jmp endless_loop



times 510-($-$$) db 0

; Add the boot signature AA55 at the very end (Little endian)
signature       dw      0xaa55
