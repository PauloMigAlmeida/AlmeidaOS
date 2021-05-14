;=============================================================================
; @file mem.asm
;
; Memory related constants used across mutiple bootloader file.
; It's been hard to keep track of which parts of the memory are actually unused
; as I don't work on this every day. Having said that, I hope that centralising
; this type of information here will make decisions easier in the future.
;=============================================================================

%ifndef __ALMEIDAOS_GLOBALMEM_INC__
%define __ALMEIDAOS_GLOBALMEM_INC__

;===============================================================================
; Memory layout:
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

;=============================================================================
; Memory layout
;=============================================================================
MBR.Mem.Stack.Top           equ   0x00007c00
Loader.Mem.Stack.Top        equ   0x00007e00

; Rationale:
; Second stage loader can grow up from 0x00007e00 to 0x00009fff (+- 8.5 Kb)
Paging.Start.Address  equ   0x00010000
Mem.PML4.Address      equ   0x00010000  ; PML4
Mem.PDPE.Address      equ   0x00011000 ; 0x10000 + PML4 (512 entries of 64 bits)
Mem.PDE.Address       equ   0x00012000 ; 0x11000 + PDPE (512 entries of 64 bits)
Mem.PTE.Address       equ   0x00013000 ; 0x12000 + PDE (512 entries of 64 bits)
Paging.End.Address    equ   0x00018000 ; 0x13000 + 5 PT tables (512 entries of 64 bits)


%endif ; __ALMEIDAOS_GLOBALMEM_INC__
