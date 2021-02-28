;=============================================================================
; @file realmode.inc
;
; Memory/Message constants and macros used by boot loader code.
;=============================================================================

%ifndef __ALMEIDAOS_BOOT_MEM_INC__
%define __ALMEIDAOS_BOOT_MEM_INC__

;=============================================================================
; Global variables
;=============================================================================
BIOS.Drive.Id   equ   0
; While I don't choose the filesystem to be used, physical device blocks
; will be the unit used for now. This defines that the second stage Loader
; can't be bigger than 5*512 bytes (which ought to be enough for now)
Loader.File.NumberOfBlocks   equ   5

;===============================================================================
; Functions
;===============================================================================
display_string:
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


endless_loop:
; Disable interruptions
cli
  .end:
    hlt
    jmp .end

%endif ; __ALMEIDAOS_BOOT_MEM_INC__
