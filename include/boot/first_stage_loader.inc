;=============================================================================
; @file first_stage_loader.inc
;
; Memory/Message constants and macros used by first stage loader code.
;=============================================================================

%ifndef __ALMEIDAOS_FSL_INC__
%define __ALMEIDAOS_FSL_INC__

;=============================================================================
; Memory layout
;=============================================================================
Mem.Stack.Top   equ   0x00007c00

;===============================================================================
; Message Constants
;===============================================================================
Realmode.FirstStage.Booting.Msg db '[AlmeidaOS] :: Booting BIOS First Stage Loader',0x0d,0x0a,0
Realmode.BIOSDiskExtensionPresent.Msg db '[AlmeidaOS] :: BIOS Disk Extension is present',0x0d,0x0a,0
Realmode.BIOSDiskExtensionNotPresent.Msg db '[AlmeidaOS] :: BIOS Disk Extension is not present... aborting',0x0d,0x0a,0

;===============================================================================
; Functions
;===============================================================================
bios_check_extensions_present:
  ; push values into the stack to preserve them once we are done with this fnc
  pusha

  ;41h = function number for extensions check
  mov ah, 0x41
  ; Drive index (e.g. 1st HDD = 80h)
  mov dl, [BIOS.Drive.Id] ; (realmode.inc)
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
  je .found

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


%endif ; __ALMEIDAOS_FSL_INC__
