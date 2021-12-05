;=============================================================================
; @file const.asm
;
; Constants that are used during compilation to make the assembly code readable
;=============================================================================

%ifndef __ALMEIDAOS_GLOBALCONST_INC__
%define __ALMEIDAOS_GLOBALCONST_INC__

;===============================================================================
; BIOS Disk Extendsion Constants
;===============================================================================
; While I don't choose the filesystem to be used, physical device blocks
; will be the unit used for now. This defines that the second stage Loader
; can't be bigger than 5*512 bytes (which ought to be enough for now)
Loader.File.NumberOfBlocks   equ   5
Kernel.File.NumberOfBlocks   equ   152 ; Make it 4KB aligned
UserProg.File.NumberOfBlocks   	 equ   20
BIOS.DiskExt.MaxBlocksPerOp  equ   127 ; (some BIOSes are limited to 127 sectors)

;===============================================================================
; Message Constants
;===============================================================================
CR                  equ 0x0d
LF                  equ 0x0a
LOG_PREFIX          db '[AlmeidaOS] :: ',0

%endif ; __ALMEIDAOS_GLOBALCONST_INC__
