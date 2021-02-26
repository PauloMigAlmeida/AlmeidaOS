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

;=============================================================================
; Memory layout
;=============================================================================
Mem.Stack.Top   equ   0x00007c00

;===============================================================================
; Message Constants
;===============================================================================
Realmode.Booting.Msg db '[AlmeidaOS] :: Booting BIOS First Stage Loader',0x0d,0x0a,0
Realmode.BIOSDiskExtensionPresent.Msg db '[AlmeidaOS] :: BIOS Disk Extension is present',0x0d,0x0a,0
Realmode.BIOSDiskExtensionNotPresent.Msg db '[AlmeidaOS] :: BIOS Disk Extension is not present... aborting',0x0d,0x0a,0

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


bios_check_extensions_present:
  ; push values into the stack to preserve them once we are done with this fnc
  pusha

  ;41h = function number for extensions check
  mov ah, 0x41
  ; Drive index (e.g. 1st HDD = 80h)
  mov dl, [BIOS.Drive.Id]
  ; signature required by this function
  mov bx, 0x55aa
  ; call int 13h ah=41h: Check Extensions present
  ; Ref: https://en.wikipedia.org/wiki/INT_13H#INT_13h_AH=41h:_Check_Extensions_Present
  int 0x13

  ; Results
  ; CF -> Set On Not Present, Clear If Present
  jc .not_found
  ; BX -> must be equal to AA55h
  cmp bx, 0xaa55
  jne .not_found

  jmp .found

  .not_found:
    ; print message
    mov si, Realmode.BIOSDiskExtensionNotPresent.Msg
    call display_string
    ; halt the machine as there is no other way to continue booting the os
    jmp endless_loop
  .found:
    ; print message
    mov si, Realmode.BIOSDiskExtensionPresent.Msg
    call display_string
    ; restore values from the stack
    popa
    ret


endless_loop:
; Disable interruptions
cli
  .end:
    hlt
    jmp .end

%endif ; __ALMEIDAOS_BOOT_MEM_INC__
