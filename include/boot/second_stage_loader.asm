;=============================================================================
; @file second_stage_loader.inc
;
; Memory/Message constants and macros used by second stage loader code.
;=============================================================================

%ifndef __ALMEIDAOS_SSL_INC__
%define __ALMEIDAOS_SSL_INC__

;=============================================================================
; Memory layout
;=============================================================================
Loader.Mem.Stack.Top  equ   0x00007e00

;===============================================================================
; Message Constants
;===============================================================================
Realmode.SecondStage.Booting.Msg            db '[AlmeidaOS] :: Booting Second Stage Loader',0x0d,0x0a,0
Realmode.SecondStage.A20Enabled.Msg         db '[AlmeidaOS] :: A20 enabled successfully',0x0d,0x0a,0
Realmode.SecondStage.A20EnablingError.Msg   db '[AlmeidaOS] :: A20 could not be enabled. Aborting',0x0d,0x0a,0
Realmode.SecondStage.CPUIDNotSupported.Msg  db '[AlmeidaOS] :: CPUID instruction is not available. Aborting',0x0d,0x0a,0
Realmode.SecondStage.64BitNotSupported.Msg  db '[AlmeidaOS] :: 64-bit mode is not available. Aborting',0x0d,0x0a,0
Realmode.SecondStage.64BitSupported.Msg     db '[AlmeidaOS] :: 64-bit mode is available',0x0d,0x0a,0


;===============================================================================
; Functions
;===============================================================================

;=============================================================================
; enable_A20
;
; Enable the A20 address line, so memory above 1MiB can be accessed.
;
; Returned flags:
;   CF      Set if enabled
;
; Killed registers:
;   None
;=============================================================================
enable_A20:

    ; Preserve ax register.
    push    ax

    ; Check if the A20 line is already enabled.
    call    test_A20
    jc      .done

    .attempt1:

      ; Attempt enabling with the BIOS.
      mov     ax,     0x2401
      int     0x15

      ; Check if A20 line is now enabled.
      call    test_A20
      jc      .done

    .attempt2:

      ; Attempt enabling with the keyboard controller.
      call    .attempt2.wait1

      ; Disable keyboard
      mov     al,     0xad
      out     0x64,   al
      call    .attempt2.wait1

      ; Read from input
      mov     al,     0xd0
      out     0x64,   al
      call    .attempt2.wait2

      ; Get keyboard data
      in      al,     0x60
      push    eax
      call    .attempt2.wait1

      ; Write to output
      mov     al,     0xd1
      out     0x64,   al
      call    .attempt2.wait1

      ; Send data
      pop     eax
      or      al,     2
      out     0x60,   al
      call    .attempt2.wait1

      ; Enable keyboard
      mov     al,     0xae
      out     0x64,   al
      call    .attempt2.wait1

      ; Check if the A20 line is now enabled.
      call    test_A20
      jc      .done

      jmp     .attempt3

      .attempt2.wait1:

        in      al,     0x64
        test    al,     2
        jnz     .attempt2.wait1
        ret

      .attempt2.wait2:

        in      al,     0x64
        test    al,     1
        jz      .attempt2.wait2
        ret

    .attempt3:

      ; Attempt enabling with the FAST A20 feature.
      in      al,     0x92
      or      al,     2
      out     0x92,   al
      xor     ax,     ax

      ; Check if A20 line is now enabled.
      call test_A20
      jc  .done

    .failed:
      ; Display a status message.
      mov si, Realmode.SecondStage.A20EnablingError.Msg
      call display_string

      ; hangs the system
      jmp endless_loop

    .done:
      ; Display a status message.
      mov si, Realmode.SecondStage.A20Enabled.Msg
      call display_string

      ; Restore register.
      pop     ax

      ret


;=============================================================================
; test_A20
;
; Check to see if the A20 address line is enabled.
;
; Return flags:
;   CF      Set if enabled
;
; Killed registers:
;   None
;=============================================================================
test_A20:

    ; Preserve registers.
    push    ds
    push    es
    pusha

    ; Initialize return result to "not enabled".
    clc

    ; Set es segment register to 0x0000.
    xor     ax,     ax
    mov     es,     ax

    ; Set ds segment register to 0xffff.
    not     ax
    mov     ds,     ax

    ; If the A20 line is disabled, then es:di and ds:si will point to the same
    ; physical memory location due to wrap-around at 1 MiB.
    ;
    ; es:di = 0000:0500 = 0x0000 * 16 + 0x0500 = 0x00500 = 0x0500
    ; ds:si = ffff:0510 = 0xffff * 16 + 0x0510 = 0x10500 = 0x0500
    mov     di,     0x0500
    mov     si,     0x0510

    ; Preserve the original values stored at es:di and ds:si.
    mov     ax,     [es:di]
    push    ax
    mov     ax,     [ds:si]
    push    ax

    ; Write different values to each logical address.
    mov     byte [es:di],   0x00
    mov     byte [ds:si],   0xff

    ; If a store to ds:si changes the value at es:di, then memory wrapped and
    ; A20 is not enabled.
    cmp     byte [es:di],   0xff

    ; Restore the original values stored at es:di and ds:si.
    pop     ax
    mov     [ds:si],    ax
    pop     ax
    mov     [es:di],    ax

    je      .done

    .enabled:

        ; Set the carry flag to indicate the A20 line is enabled.
        stc

    .done:

        ; Restore registers.
        popa
        pop     es
        pop     ds

        ret


;=============================================================================
; HasCPUID
;
; Detect if the cpu supports the CPUID instruction.
;
; Bit 21 of the EFLAGS register can be modified only if the CPUID instruction
; is supported.
;
; Return flags:
;   CF      Set if CPUID is supported
;
; Killed registers:
;   None
;=============================================================================
HasCPUID:

  ; Preserve registers.
  push    eax
  push    ecx

  ; Copy flags to eax and ecx.
  pushfd
  pop     eax
  mov     ecx,    eax

  ; Set flag 21 (the ID bit)
  xor     eax,    (1 << 21)
  push    eax
  popfd

  ; Copy flags back to eax. If CPUID is supported, bit 21 will still be set.
  pushfd
  pop     eax

  ; Restore the original flags from ecx.
  push    ecx
  popfd

  ; Initialize the return flag (carry) to unsupported.
  clc

  ; If eax and ecx are equal, then flag 21 didn't remain set, and CPUID is
  ; not supported.
  xor     eax,    ecx
  jz      .done       ; CPUID is not supported

  .supported:

    ; CPUID is supported.
    stc

  .done:

    ; Restore registers.
    pop     ecx
    pop     eax

    ret

cpu_supports_64_bit_mode:
    ; Detect if the cpuid instruction is available.
    call    HasCPUID
    jnc     .error.noCPUID

    ; Is the processor info function supported?
    mov     eax,    0x80000000  ; Get Highest Extended Function Supported
    cpuid
    cmp     eax,    0x80000001
    jb      .error.no64BitMode

    ; Use processor info function to see if 64-bit mode is supported.
    mov     eax,    0x80000001  ; Extended Processor Info and Feature Bits
    cpuid
    test    edx,    (1 << 29)   ; 64-bit mode bit
    jz      .error.no64BitMode

    ; Clear 32-bit register values.
    xor     eax,    eax
    xor     edx,    edx

    ; Display a status message.
    mov     si,     Realmode.SecondStage.64BitSupported.Msg
    call    display_string
    ; returns from function
    ret

    .error.noCPUID:
      ; Display a status message.
      mov     si,     Realmode.SecondStage.CPUIDNotSupported.Msg
      call    display_string
      ; hangs the system
      jmp endless_loop

    .error.no64BitMode:
      ; Display a status message.
      mov     si,     Realmode.SecondStage.64BitNotSupported.Msg
      call    display_string
      ; hangs the system
      jmp endless_loop

;-----------------------------------------------------------------------------
; Global Descriptor Table used (temporarily) in 32-bit protected mode
;-----------------------------------------------------------------------------
GDT32.Table:

    ; Null descriptor
    dw      0x0000  ; LimitLow
    dw      0x0000  ; BaseLow
    db      0x00    ; BaseMiddle
    db      0x00    ; Access
    db      0x00    ; LimitHighFlags
    db      0x00    ; BaseHigh

    ; 32-bit protected mode - code segment descriptor (selector = 0x08)
    ; (Base=0, Limit=4GiB-1, RW=1, DC=0, EX=1, PR=1, Priv=0, SZ=1, GR=1)
    dw      0xffff      ; LimitLow
    dw      0x0000      ; BaseLow
    db      0x00        ; BaseMiddle
    db      10011010b   ; Access
    db      11001111b   ; LimitHighFlags
    db      0x00        ; BaseHigh

    ; 32-bit protected mode - data segment descriptor (selector = 0x10)
    ; (Base=0, Limit=4GiB-1, RW=1, DC=0, EX=0, PR=1, Priv=0, SZ=1, GR=1)
    dw      0xffff      ; LimitLow
    dw      0x0000      ; BaseLow
    db      0x00        ; BaseMiddle
    db      10010010b   ; Access
    db      11001111b   ; LimitHighFlags
    db      0x00        ; BaseHigh

    ; 16-bit protected mode - code segment descriptor (selector = 0x18)
    ; (Base=0, Limit=1MiB-1, RW=1, DC=0, EX=1, PR=1, Priv=0, SZ=0, GR=0)
    dw      0xffff      ; LimitLow
    dw      0x0000      ; BaseLow
    db      0x00        ; BaseMiddle
    db      10011010b   ; Access
    db      00000001b   ; LimitHighFlags
    db      0x00        ; BaseHigh

    ; 16-bit protected mode - data segment descriptor (selector = 0x20)
    ; (Base=0, Limit=1MiB-1, RW=1, DC=0, EX=0, PR=1, Priv=0, SZ=0, GR=0)
    dw      0xffff      ; LimitLow
    dw      0x0000      ; BaseLow
    db      0x00        ; BaseMiddle
    db      10010010b   ; Access
    db      00000001b   ; LimitHighFlags
    db      0x00        ; BaseHigh

GDT32.Table.Size    equ     ($ - GDT32.Table)

GDT32.Table.Pointer:
    dw  GDT32.Table.Size - 1    ; Limit = offset of last byte in table
    dd  GDT32.Table


enter_protected_mode:
  ; Disable interruptions
  cli

  ; Load 32-bit GDT
  lgdt [GDT32.Table.Pointer]

  ; Enable protected mode.
   mov     eax,    cr0
   or      eax,    (1 << 0)    ; CR.PE
   mov     cr0,    eax

   jmp 8:protected_mode_boot


%endif ; __ALMEIDAOS_SSL_INC__
