[BITS 64]

section .data

; Include functions/constants that are useful in real mode
%include "../../include/boot/global/const.asm"
%include "../../include/boot/global/mem.asm"

; create elf section that is always placed first when linking asm and c files
section .head.text

extern kmain
global kernel_start

kernel_start:
  ; The System V ABI requires the direction flag to be clear on function entry.
  cld

  xor ax,ax
  mov ss, ax

  ; TODO: ELF specification dictates that we must clean BSS area before init
  ;
  ; .bss: This section holds uninitialized data that contribute to the program's
  ; memory image. By definition, the system initializes the data with zeros
  ; when the program begins to run. The section occupies no file space, as
  ; indicated by the section type, SHT_NOBITS.
  ;
  ; https://refspecs.linuxfoundation.org/elf/elf.pdf - Page 29

  mov rsp, Kernel.New.Start.Address
  call kmain

.endless_loop:
  cli
  hlt
  jmp .endless_loop
