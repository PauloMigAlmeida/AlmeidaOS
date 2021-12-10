[BITS 64]

section .data

; Include useful functions, constants and macros
%include "../../include/boot/global/macro.asm"


; create elf section that is always placed first when linking asm and c files
section .head.text

  ; C-defined functions that this code relies on
  extern umain

  ; Export references to C
  global user_entry

  ; Linker-generated symbols
  extern _BSS_START
  extern _BSS_SIZE

user_entry:

  ; The System V ABI requires the direction flag to be clear on function entry.
  cld

  ; Set up RSP otherwise syscall/sysret fail since the context switch doesn't carry \
  ;	RSP value for ring 3 like it does when using IRETQ
  mov rsp, 0x40000

  ; ELF specification dictates that we must clean BSS area before init
  ;
  ; .bss: This section holds uninitialized data that contribute to the program's
  ; memory image. By definition, the system initializes the data with zeros
  ; when the program begins to run. The section occupies no file space, as
  ; indicated by the section type, SHT_NOBITS.
  ;
  ; https://refspecs.linuxfoundation.org/elf/elf.pdf - Page 29
  mov rdi, _BSS_START
  mov rcx, _BSS_SIZE
  call memzero


  ; Set %ebp to NULL. This sets a stopping point for coredump functionality when
  ; we try to unwind the call trace to printk the info in the processor at the
  ; time everything went to custard.
  xor rbp, rbp
  ; enter the kernel
  call umain

  ; we should never ever come back from umain function, but if it does for any
  ; reason, hang the cpu
  .endless_loop:
    jmp .endless_loop

;===============================================================================
; memzero
;
; Fill memory region with zero
;
; Inputs:
;   RDI ->  Buffer address
;   RCX ->  Amount of bytes to fill
;
; Killed registers:
;   None
;===============================================================================

memzero:
  ; preserve all registers
  pushaq

  ; clear direction flag
  cld

  ; Define vale to be stored
  xor   rax,   rax

  ; repeat until ecx is 0
  rep stosb

  ; restore registers
  popaq
  ret
