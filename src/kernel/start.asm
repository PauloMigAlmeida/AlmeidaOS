[BITS 64]

; Include functions/constants that are useful in real mode
%include "../../include/boot/global/const.asm"
%include "../../include/boot/global/mem.asm"

section .text

extern kmain
global kernel_start

kernel_start:
  ; The System V ABI requires the direction flag to be clear on function entry.
  cld

  mov rsp, Kernel.New.Start.Address
  call kmain

  ; TODO: For some reason, when kmain returns (which should be the case in the real world)
  ; instead of halting the system, it restarts... I wonder if that has something to do
  ; with any inturrption ... then again, I haven't set up IDT for 64 mode yet.

.endless_loop:
  cli
  hlt
  jmp .endless_loop
