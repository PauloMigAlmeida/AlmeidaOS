[BITS 16]
[ORG 0x7E00]

; Produce a map file containing all symbols and sections.
[map all ../../build/boot/loader.map]

;===============================================================================
; loader
;
; Second-stage boot loader entry point
;
; Due to the limited size of the first-stage boot loader, its only mission in
; life is to load the Second-stage boot leader. The second-stage boot loader is
; responsible for essentially preparing the system to enter into the protected
; mode.
;
; Input registers:
;
;   DL      Boot drive number
;
; Memory layout before this code starts running:
;
;   00000000 - 000003ff        1,024 bytes     Real mode IVT
;   00000400 - 000004ff          256 bytes     BIOS data area
;   00000500 - 00007bff       30,464 bytes     Free
;   00007c00 - 00007dff          512 bytes     First-stage boot loader (MBR)
;   00007e00 - 0009fbff      622,080 bytes     Free -> We are here now
;   0009fc00 - 0009ffff        1,024 bytes     Extended BIOS data area (EBDA)
;   000a0000 - 000bffff      131,072 bytes     BIOS video memory
;   000c0000 - 000fffff      262,144 bytes     ROM
;===============================================================================

; jump to start procedure
jmp start

; Include functions/constants that are useful in real mode
%include "../../include/boot/realmode.asm"
%include "../../include/boot/second_stage_loader.asm"

start:

  ; Proper initialisation of stack during BIOS bootloader
  ;   https://stackoverflow.com/a/33975465/832748
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov bx, Loader.Mem.Stack.Top

  ; Turn off interrupts for SS:SP update to avoid a problem with buggy 8088 CPUs
  cli
  ; SS = 0x0000
  mov ss, ax
  ; SP = 0x7c00
  ; Set the stack starting just below where the bootloader is at 0x0:0x7c00.
  mov sp, bx
  ; Turn interrupts back on
  sti

boot:
  ; Save DriveId for later
  mov [BIOS.Drive.Id], dl

  ; Print booting message
  mov si, Realmode.SecondStage.Booting.Msg
  call display_string

  ; Attempt to enable the A20 line if necessary.
  call enable_A20

  ; Check whether we are running on a 64-bit processor
  call cpu_supports_64_bit_mode

  ; Prepare to enter protected mode
  call enter_protected_mode

  ; enter a endless loop. This instruction should never be reached
  jmp endless_loop


[BITS 32]

; TODO:
;  1 -> Revisit the GDT descriptions created...I don't think we need one for 16-bit... in fact, I think we may not need more than 3 (Null, code, data)
;  2 -> Initialise the segments on protected mode correctly...the ! is printing at the top...this doesn't look good at all.
;  3 -> Add docs/comments for the things you decide (bits/cnfs) -> GDT segments
;  4 -> Try to refactor that second_stage_loader...the GDT stuff made it look a bit weird
;  5 -> Create a display string function for protected mode.
;  6 -> Push stuff to stack on enter_protected_mode function


TestMessage db 'Protected Mode',0xc,0xa,0

protected_mode_boot:

  sti;

  mov ebx,0xb8000    ; The video address
  mov ax,'P'         ; The character to be print
  mov ah,0x0F        ; The color: white(F) on black(0)
  mov [ebx],ax        ; Put the character into the video memory

  ; Disable interruptions
  cli

  .end:
    hlt
    jmp .end

display_string_32:
    pusha

    mov     ah,     0x0e    ; int 10 AH=0x0e
    xor     bx,     bx

    cld

    .loop:

        ; Read next string character into al register.
        lodsb

        ; Break when a null terminator is reached.
        cmp     al,     0
        je      .done

        ; Call int 10 function 0eh (print character to teletype)
        int     0x10
        jmp     .loop

    .done:

        popa
        ret

; On physical devices this isn't required because the BIOS will
; pull the x number of blocks regardless of their content, however,
; if you are using QEMU and a raw image, it will strugle to Read
; the BIOS Disk Access Packet (DAP ) because the file finishes
; way earlier than the number of blocks requested.
times (Loader.File.NumberOfBlocks * 512) - ($ - $$) db 0
