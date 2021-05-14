;=============================================================================
; @file const.asm
;
; Constants that are used during compilation to make the assembly code readable
;=============================================================================

%ifndef __ALMEIDAOS_GLOBALCONST_INC__
%define __ALMEIDAOS_GLOBALCONST_INC__

;===============================================================================
; Message Constants
;===============================================================================
CR                  equ 0x0d
LF                  equ 0x0a
LOG_PREFIX          db '[AlmeidaOS] :: ',0

%endif ; __ALMEIDAOS_GLOBALCONST_INC__
