[BITS 64]

; create elf section that is always placed first when linking asm and c files
section .head.text

  ; C-defined functions that this code relies on
  ;extern memset
  extern umain

  ; Export references to C
  global user_entry

user_entry:

  ; The System V ABI requires the direction flag to be clear on function entry.
  cld

  ; Set %ebp to NULL. This sets a stopping point for coredump functionality when
  ; we try to unwind the call trace to printk the info in the processor at the
  ; time everything went to custard.
  xor rbp, rbp
  ; enter the kernel
  call umain

  ; we should never ever come back from umain function, but if it does for any
  ; reason, hang the cpu
  ;jmp endless_loop

  .endless_loop:
    jmp .endless_loop
