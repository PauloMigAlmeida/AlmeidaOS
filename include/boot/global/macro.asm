;=============================================================================
; @file macro.asm
;
; Contains NASM macros used across all across the assembly files
;=============================================================================

;===============================================================================
; NASM Macros
;===============================================================================
%macro pushaq 0
    push rax      ;save current rax
    push rbx      ;save current rbx
    push rcx      ;save current rcx
    push rdx      ;save current rdx
    push rbp      ;save current rbp
    push rdi      ;save current rdi
    push rsi      ;save current rsi
    push r8       ;save current r8
    push r9       ;save current r9
    push r10      ;save current r10
    push r11      ;save current r11
    push r12      ;save current r12
    push r13      ;save current r13
    push r14      ;save current r14
    push r15      ;save current r15
%endmacro

%macro popaq 0
  pop r15       ;restore r15
  pop r14       ;restore r14
  pop r13       ;restore r13
  pop r12       ;restore r12
  pop r11       ;restore r11
  pop r10       ;restore r10
  pop r9        ;restore r9
  pop r8        ;restore r8
  pop rsi       ;restore rsi
  pop rdi       ;restore rdi
  pop rbp       ;restore rbp
  pop rdx       ;restore rdx
  pop rcx       ;restore rcx
  pop rbx       ;restore rbx
  pop rax       ;restore rax
%endmacro

