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
    ;   Section 4.8.1 - Long-Mode Code Segment Descriptors
    dw      0x0000      ; LimitLow
    dw      0x0000      ; BaseLow
    db      0x00        ; BaseMiddle
    db      10011000b   ; P=1, DPL=00, 1, 1, C=0, R=0 , A=0
    db      00100000b   ; G=0, D=0, L=1, AVL=0, Segment limit [19:16] = 0
    db      0x00        ; BaseHigh

    ; kernel: data segment descriptor (selector = 0x10)
    ; most fields are ignored (hence set to 0) according to AMD 64 manual
    ;   Section 4.8.3 - Long-Mode Data Segment Descriptors
    dw      0x0000      ; LimitLow
    dw      0x0000      ; BaseLow
    db      0x00        ; BaseMiddle
    db      10010010b   ; Access -> P=1, DPL=00, 1, 0, E=0, W=1 , A=0
    db      0x00        ; LimitHighFlags -> G=0, D=0, L=0, AVL=0, Segment limit [19:16] = 0
    db      0x00        ; BaseHigh

    ; user: data segment descriptor (selector = 0x18)
    ; most fields are ignored (hence set to 0) according to AMD 64 manual
    ;   Section 4.8.3 - Long-Mode Data Segment Descriptors
    dw      0x0000      ; LimitLow
    dw      0x0000      ; BaseLow
    db      0x00        ; BaseMiddle
    db      11110010b   ; Access -> P=1, DPL=11, 1, 0, E=0, W=1 , A=0
    db      0x00        ; LimitHighFlags -> G=0, D=0, L=0, AVL=0, Segment limit [19:16] = 0
    db      0x00        ; BaseHigh

    ; user: code segment descriptor (selector = 0x20)
    ; most fields are ignored (hence set to 0) according to AMD 64 manual
    ;   Section 4.8.1 - Long-Mode Code Segment Descriptors
    dw      0x0000      ; LimitLow
    dw      0x0000      ; BaseLow
    db      0x00        ; BaseMiddle
    db      11111000b   ; P=1, DPL=11, 1, 1, C=0, R=0 , A=0
    db      00100000b   ; G=0, D=0, L=1, AVL=0, Segment limit [19:16] = 0
    db      0x00        ; BaseHigh

	; TSS: Task State Segment descriptor (selector = 0x28)
	;	Section 7.2.4 - TSS Descriptor in 64-bit mode - Intel 64 manual Volume 3
TSS64.Descriptor:
    dw      TSS64.Segment.Size - 1      ; SegmentLimit 	[15:00]
    dw      0x0000      				; Base Low 		[15:00]
    db      0x00        				; Base Middle 	[23:16]
    db      10001001b   				; P=1, DPL=00, 0, Type=1001 (Available 64-bit TSS - AMD manual 4.8.3)
    db      00000000b   				; G=0, 0, 0, AVL=0, Segment limit [19:16] = 0
    db      0x00        				; Base High		[31:24]
    dd		0x00						; Base Highest	[63:32]
    dd		0x00						; Reserved

GDT64.Table.Size    equ     ($ - GDT64.Table)

GDT64.Table.Pointer:
    dw  GDT64.Table.Size - 1    		; Limit = offset of last byte in table
    dq  GDT64.Table

TSS64.Segment:
	dd 	0x00							; Reserved
	dq	0x00							; RSP0 (Ring 0 64-bit stack canonical address)
	dq	0x00							; RSP1 (Ring 1 64-bit stack canonical address)
	dq	0x00							; RSP2 (Ring 2 64-bit stack canonical address)
	dq	0x00							; Reserved
	dq	0x00							; IST 1
	dq	0x00							; IST 2
	dq	0x00							; IST 3
	dq	0x00							; IST 4
	dq	0x00							; IST 5
	dq	0x00							; IST 6
	dq	0x00							; IST 7
	dq	0x00							; Reserved
	dw	0x00							; Reserved
	dw	0x00							; I/O Map Base Address

TSS64.Segment.Size	equ		($ - TSS64.Segment)

%endif ; __ALMEIDAOS_GDT64_INC__
