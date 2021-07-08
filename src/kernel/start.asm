[BITS 64]

section .data

; Include functions/constants that are useful in real mode
%include "../../include/boot/global/const.asm"
%include "../../include/boot/global/mem.asm"

; create elf section that is always placed first when linking asm and c files
section .head.text

	extern kmain
	global kernel_start
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

  xor rbp, rbp       ; Set %ebp to NULL
	; enter the kernel 
  call kmain

.endless_loop:
  cli
  hlt
  jmp .endless_loop


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
