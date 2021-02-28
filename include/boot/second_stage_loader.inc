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
Mem.Stack.Top   equ   0x00007e00

;===============================================================================
; Message Constants
;===============================================================================
Realmode.SecondStage.Booting.Msg db '[AlmeidaOS] :: Booting Second Stage Loader',0x0d,0x0a,0

%endif ; __ALMEIDAOS_SSL_INC__
