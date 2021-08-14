;=============================================================================
; @file gdt64.asm
;
; Global Descriptor Table
;=============================================================================

%ifndef __ALMEIDAOS_GDT64_INC__
%define __ALMEIDAOS_GDT64_INC__

;-----------------------------------------------------------------------------
; Global Descriptor Table used in 64-bit long mode
;-----------------------------------------------------------------------------
GDT64.Table:

    ; Null descriptor
    dw      0x0000  ; LimitLow
    dw      0x0000  ; BaseLow
    db      0x00    ; BaseMiddle
    db      0x00    ; Access
    db      0x00    ; LimitHighFlags
    db      0x00    ; BaseHigh

    ; kernel: code segment descriptor (selector = 0x08)
    ; most fields are ignored (hence set to 0) according to AMD 64 manual
    ;   Section 4.8.1 - Long-Mode Code Segment Descriptiors
    dw      0x0000      ; LimitLow
    dw      0x0000      ; BaseLow
    db      0x00        ; BaseMiddle
    db      10011000b   ; P=1, DPL=00, 1, 1, C=0, R=0 , A=0
    db      00100000b   ; G=0, D=0, L=1, AVL=0, Segment limit [19:16] = 0
    db      0x00        ; BaseHigh

    ; kernel: data segment descriptor (selector = 0x10)
    ; most fields are ignored (hence set to 0) according to AMD 64 manual
    ;   Section 4.8.3 - Long-Mode Data Segment Descriptiors
    dw      0x0000      ; LimitLow
    dw      0x0000      ; BaseLow
    db      0x00        ; BaseMiddle
    db      10010010b   ; Access -> P=1, DPL=00, 1, 0, E=0, W=1 , A=0
    db      0x00        ; LimitHighFlags -> G=0, D=0, L=0, AVL=0, Segment limit [19:16] = 0
    db      0x00        ; BaseHigh


GDT64.Table.Size    equ     ($ - GDT64.Table)

GDT64.Table.Pointer:
    dw  GDT64.Table.Size - 1    ; Limit = offset of last byte in table
    dq  GDT64.Table


%endif ; __ALMEIDAOS_GDT64_INC__
