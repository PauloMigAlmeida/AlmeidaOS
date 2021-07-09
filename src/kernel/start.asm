[BITS 64]

section .data

; Include useful functions, constants and macros
%include "../../include/boot/global/const.asm"
%include "../../include/boot/global/mem.asm"
%include "../../include/boot/global/macro.asm"

; create elf section that is always placed first when linking asm and c files
section .head.text

	extern kmain
	extern interrupt_handler;
	global kernel_start
	global divide_by_zero_isr;
	extern _BSS_START   ; Linker-generated symbol
	extern _BSS_SIZE    ; Linker-generated symbol

kernel_start:
  ; The System V ABI requires the direction flag to be clear on function entry.
  cld

  xor ax,ax
  mov ss, ax

	; prepare stack for the env
	mov rsp, Kernel.New.Start.Address + Kernel.New.ELFTextHeader.Offset

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
  call kmain

  ; we should never ever come back from kmain function, but if it does for any
  ; reason, hang the cpu
  jmp .endless_loop

  .endless_loop:
    cli
    hlt
    jmp .endless_loop


divide_by_zero_isr:
  pushaq
  ; push trap number
  push 0
  ;push errono (not all interrupts have an error code but this ensures I can use a
  ; single struct to wrap all the values)
  push 0

  mov rdi, rsp
  call interrupt_handler

  popaq
  add rsp, 16
  iretq




;===============================================================================
; memzero
;
; Zero out memory that starts at address RDI with size RCX
;
; Killed registers:
;   rdi, rcx
;===============================================================================
memzero:
  cld

	.iter:
		mov byte[rdi], 0x0
		inc rdi
		loop .iter;

  ret

; Paulo, if you want to change that to stob, you can get the speed boost when you
; enable fast-string in the processor
