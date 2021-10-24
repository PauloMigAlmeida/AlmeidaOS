[BITS 64]

section .data

; Include useful functions, constants and macros
%include "../../include/boot/global/const.asm"
%include "../../include/boot/global/mem.asm"
%include "./gdt64.asm"

  ; Export references to C
  global e820_mem_start
  global e820_mem_end

  ; variables
  e820_mem_start:  dq  e820.Mem.Start.Address
  e820_mem_end:    dq  e820.Mem.End.Address


; create elf section that is always placed first when linking asm and c files
section .head.text

  ; C-defined functions that this code relies on
  extern memset
  extern kmain

	; Linker-generated symbols
  extern _BSS_START
  extern _BSS_SIZE

	; Export references to C
  global kernel_entry

kernel_entry:

  ; Reload the 64-bit GDT.
  mov rdi, GDT64.Table.Pointer
  lgdt    [rdi]

  ; Load TSS Selector (0x28 on GDT64.Table)
  mov 	ax, 	0x28
  ltr 	ax

  ; Far jumps are invalid in long mode so this trick is required
  ; to obtain the same result. This is required to properly load
  ; the new GDT structure
  push 	0x08
  mov 	rdi, 	kernel_start
  push 	rdi
  db 	0x48
  retf


kernel_start:
  ; The System V ABI requires the direction flag to be clear on function entry.
  cld

  ; ELF specification dictates that we must clean BSS area before init
  ;
  ; .bss: This section holds uninitialized data that contribute to the program's
  ; memory image. By definition, the system initializes the data with zeros
  ; when the program begins to run. The section occupies no file space, as
  ; indicated by the section type, SHT_NOBITS.
  ;
  ; https://refspecs.linuxfoundation.org/elf/elf.pdf - Page 29
  mov rdi, _BSS_START
  mov rsi, 0x0
  mov rdx, _BSS_SIZE
  call memset

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
