[BITS 16]
[ORG 0x7c00]

; Produce a map file containing all symbols and sections.
[map all ../../build/boot/mbr.map]
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
%include "../../include/boot/global/const.asm"
%include "../../include/boot/global/mem.asm"
%include "../../include/boot/mode/realmode.asm"

start:
    ; Proper initialisation of stack during BIOS bootloader
    ;   https://stackoverflow.com/a/33975465/832748
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov bx, MBR.Mem.Stack.Top

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
    ; Print booting message
    mov si, Realmode.FirstStage.Booting.Msg
    call display_string

    ; Check if disk extensions is available in the BIOS
    ; If it doesn't it hangs the system, otherwise it returns from the function
    call bios_check_extensions_present

    ; Clean up registers that will be passed as parameters
    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx

    ; Read the second-stage loader from the disk.
    mov eax, Loader.Mem.Stack.Top
    mov bx, Loader.File.NumberOfBlocks
    mov ecx, 1
    call bios_extended_read_sectors_from_drive

    ; Save DriveId to dl to be retrieved on second stage loader -> TBC
    mov dl,[BIOS.Drive.Id]

    ; Perform a long jump to the second-stage loader
    jmp 0:Loader.Mem.Stack.Top

    ; enter a endless loop. This instruction should never be reached
    jmp endless_loop


; Zero-pad the file all the way up to the byte number 510
times 510-($-$$) db 0

; Add the boot signature AA55 at the very end (Little endian)
signature       dw      0xaa55
