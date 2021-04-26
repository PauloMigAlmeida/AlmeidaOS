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

;=============================================================================
; Memory layout
;=============================================================================
MBR.Mem.Stack.Top           equ   0x00007c00
Loader.Mem.Stack.Top        equ   0x00007e00

; Rationale:
; Second stage loader can grow up from 0x00007e00 to 0x00009fff (+- 8.5 Kb)
Mem.PML4.Start.Address  equ   0x00010000
Mem.PML4.End.Address    equ   0x00018000 ; 0x10000 + (512 entries of 64 bits each)
Mem.PDPE.Start.Address  equ   0x00018000
Mem.PDPE.End.Address    equ   0x00020000 ; 0x18000 + (512 entries of 64 bits each)
Mem.PDE.Start.Address   equ   0x00020000
Mem.PDE.End.Address     equ   0x00028000 ; 0x20000 + (512 entries of 64 bits each)
Mem.PTE.Start.Address   equ   0x00028000
Mem.PTE.End.Address     equ   0x00030000 ; 0x28000 + (512 entries of 64 bits each)


%endif ; __ALMEIDAOS_GLOBALMEM_INC__
