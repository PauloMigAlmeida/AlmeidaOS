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

;======================================================================================================================
; Physical Memory layout:
;
;   00000000 - 000003ff        1,024 bytes     Real mode IVT
;   00000400 - 000004ff          256 bytes     BIOS data area
;   00000500 - 00007bff       30,464 bytes     Free
;   00007c00 - 00007dff          512 bytes     First-stage boot loader (MBR)
;   00007e00 - 0009fbff      622,080 bytes     Free
;   0009fc00 - 0009ffff        1,024 bytes     Extended BIOS data area (EBDA)
;   000a0000 - 000bffff      131,072 bytes     BIOS video memory
;   000c0000 - 000fffff      262,144 bytes     ROM
;======================================================================================================================

;======================================================================================================================
; Physical Memory utilisation layout:
;
;   Second Loader   = 0x07e00 -> 0x8800        (assuming 5 IO blocks)
;   E820 memory map = 0x08800 -> 0x9000        (assuming 2048 bytes which is enough space for 102 entries)
;   Kernel          = 0x09000 -> 0x1bc00       (assuming 150 IO blocks)
;   (guard hole)    = 0x1bc00 -> 0xfffff       (room to increase any of the above [+- 912 Kb] - hard limit given A20)
;   Early Paging    = 0x20000 -> 0x28000       (early 10-MiB identity paging)
;												 - Kernel is moved to another location before early paging is setup
;======================================================================================================================

;======================================================================================================================
; Physical Memory Constants
;======================================================================================================================
MBR.Mem.Stack.Top           equ   0x00007c00
Loader.Mem.Stack.Top        equ   0x00007e00

; BIOS e820 memory map
e820.Mem.Start.Address     equ (Loader.Mem.Stack.Top + Loader.File.NumberOfBlocks * 512) ; this should be 0x8800
e820.Mem.End.Address       equ (e820.Mem.Start.Address + 4 * 512) ; enough for 102 entries

; Kernel code:
Loader.Kernel.Start.Address       equ e820.Mem.End.Address ; This should be 0x9000
Loader.Kernel.End.Address         equ (Loader.Kernel.Start.Address + Kernel.File.NumberOfBlocks * 512)
Kernel.New.Start.PhysicalAddress  equ 0x00200000
Kernel.New.ELFTextHeader.Offset   equ 0x00001000 ; .text starts <p> + 0x1000

; User code:
; 		-> this should be 0x1c000 assuming kernel occupies 152 IO blocks
Loader.UserProg.Start.Address		equ Loader.Kernel.End.Address
Loader.UserProg.End.Address			equ Loader.Kernel.End.Address + UserProg.File.NumberOfBlocks * 512

; Early paging
Paging.Start.Address  equ   0x20000
Paging.Table.Size     equ   0x1000									  		; 0x1000 = 4kb = 512 entries of 64 bits
Mem.PML4.Address      equ   Paging.Start.Address                      		; PML4
Mem.PDPE.Address      equ   Mem.PML4.Address + Paging.Table.Size      		; 0x20000 + PML4 (512 entries of 64 bits)
Mem.PDE.Address       equ   Mem.PDPE.Address + Paging.Table.Size      		; 0x21000 + PDPE (512 entries of 64 bits)
Paging.End.Address    equ   Mem.PDE.Address  + (64 * Paging.Table.Size)     ; 0x22000 + 64x PDE (512 entries of 64 bits)


;======================================================================================================================
; Virtual Memory utilisation layout:
;
;=======================================================================================================================
;    Start addr    |   Offset   |     End addr     |  Size   | VM area description
;=======================================================================================================================
;                  |            |                  |         |
; 0000000000000000 |    0       | 00007fffffffffff |  128 TB | user-space virtual memory, different per mm
;__________________|____________|__________________|_________|__________________________________________________________
;                  |            |                  |         |
; 0000800000000000 | +128    TB | ffff7fffffffffff | ~16M TB | ... huge, almost 64 bits wide hole of non-canonical
;                  |            |                  |         |     virtual memory addresses up to the -128 TB
;                  |            |                  |         |     starting offset of kernel mappings.
;__________________|____________|__________________|_________|__________________________________________________________
;                                                            |
;                                                            | Kernel-space virtual memory, shared among all processes
;____________________________________________________________|__________________________________________________________
;                  |            |                  |         |
; ffff800000000000 | -128    TB | ffff80001fffffff |  512 MB | kernel text mapping, mapped to physical address 0
;__________________|____________|____________________________|__________________________________________________________
;                  |            |                  |         |
; ffff800020000000 | ~ -128  TB | ffff80003fffffff |  512 MB | unused hole / guard hole
;__________________|____________|____________________________|__________________________________________________________
;                  |            |                  |         |
; ffff800040000000 | ~ -128  TB | ffff80005fffffff |  512 MB | memory allocator header
;__________________|____________|____________________________|__________________________________________________________
;                  |            |                  |         |
; ffff800060000000 | ~ -128  TB | ffff80205fffffff |  128 GB | memory allocator content -> content (max), which includes
;                  |            |                  |         | space mapped for kmalloc, vmalloc
;__________________|____________|__________________|_________|__________________________________________________________
;
;======================================================================================================================

;======================================================================================================================
; Virtual Memory Constants
;======================================================================================================================

; Kernel code
Kernel.New.Start.VirtualAddress   equ 0xffff800000200000  ; placing it in the higher-half of the memory

%endif ; __ALMEIDAOS_GLOBALMEM_INC__
