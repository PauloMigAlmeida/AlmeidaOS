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

; Scratchpad:
;   Second Loader   = 0x7e00 -> 0x8800 (assuming 5 IO blocks)
;   E820 memory map = 0x8800 -> 0x8c00 (assuming 512 bytes which is enough space for 128 entries)
;   Kernel          = 0x8c00 -> 0x15400 (assuming 100 IO blocks)

; BIOS e820 memory map
e820.Mem.Start.Address     equ (Loader.Mem.Stack.Top + Loader.File.NumberOfBlocks * 512) ; this should be 0x8800
e820.Mem.End.Address       equ (e820.Mem.Start.Address + 2 * 512) ; enough for 128 entries

; Kernel code:
Loader.Kernel.Start.Address       equ e820.Mem.End.Address
Kernel.New.Start.Address          equ 0x00200000 ; TODO: to be defined yet...
Kernel.New.ELFTextHeader.Offset   equ 0x00001000 ; .text starts <p> + 0x1000

; Rationale:
; Second stage loader, e820 mem map and kernel can grow up from 0x00007e00 to 0x00019fff (+- 72.5 Kb)
Paging.Start.Address  equ   0x00020000
Mem.PML4.Address      equ   0x00020000  ; PML4
Mem.PDPE.Address      equ   0x00021000 ; 0x20000 + PML4 (512 entries of 64 bits)
Mem.PDE.Address       equ   0x00022000 ; 0x21000 + PDPE (512 entries of 64 bits)
Mem.PTE.Address       equ   0x00023000 ; 0x22000 + PDE (512 entries of 64 bits)
Paging.End.Address    equ   0x00028000 ; 0x23000 + 5 PT tables (512 entries of 64 bits)


%endif ; __ALMEIDAOS_GLOBALMEM_INC__
