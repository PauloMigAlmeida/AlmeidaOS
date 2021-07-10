%include "../../include/boot/global/macro.asm"

; C-defined functions that this code relies on
extern interrupt_handler

; Export references to C
global vector0
global vector1
global vector2
global vector3
global vector4
global vector5
global vector6
global vector7
global vector8

section .text
;===============================================================================
; Pre-defined vectors used in the x86-64 IDT (Long mode)
;
; Stack state:
;   -> push all general purpose registers
;   -> push trap number
;   -> push error number - not interrupts have one but this ensure I can use a
;         single C struct for that :)
; Killed registers:
;   None
;===============================================================================
vector0:
  ; save general purpose registers
  pushaq
  ; trap number
  push 0
  ; errono
  push 0

  ; RDI is the first parameter according to the System V AMD64 Calling Convention
  mov rdi, rsp
  ; call C interrupt_handler function
  call interrupt_handler

  ; restore general purpose registers
  popaq
  ; pop trap number and errono off the stack
  add rsp, 16
  ; special return instruction for interrupts
  iretq

vector1:
  ; save general purpose registers
  pushaq
  ; trap number
  push 1
  ; errono
  push 0

  ; RDI is the first parameter according to the System V AMD64 Calling Convention
  mov rdi, rsp
  ; call C interrupt_handler function
  call interrupt_handler

  ; restore general purpose registers
  popaq
  ; pop trap number and errono off the stack
  add rsp, 16
  ; special return instruction for interrupts
  iretq

vector2:
  ; save general purpose registers
  pushaq
  ; trap number
  push 2
  ; errono
  push 0

  ; RDI is the first parameter according to the System V AMD64 Calling Convention
  mov rdi, rsp
  ; call C interrupt_handler function
  call interrupt_handler

  ; restore general purpose registers
  popaq
  ; pop trap number and errono off the stack
  add rsp, 16
  ; special return instruction for interrupts
  iretq

vector3:
  ; save general purpose registers
  pushaq
  ; trap number
  push 3
  ; errono
  push 0

  ; RDI is the first parameter according to the System V AMD64 Calling Convention
  mov rdi, rsp
  ; call C interrupt_handler function
  call interrupt_handler

  ; restore general purpose registers
  popaq
  ; pop trap number and errono off the stack
  add rsp, 16
  ; special return instruction for interrupts
  iretq

vector4:
  ; save general purpose registers
  pushaq
  ; trap number
  push 4
  ; errono
  push 0

  ; RDI is the first parameter according to the System V AMD64 Calling Convention
  mov rdi, rsp
  ; call C interrupt_handler function
  call interrupt_handler

  ; restore general purpose registers
  popaq
  ; pop trap number and errono off the stack
  add rsp, 16
  ; special return instruction for interrupts
  iretq

vector5:
  ; save general purpose registers
  pushaq
  ; trap number
  push 5
  ; errono
  push 0

  ; RDI is the first parameter according to the System V AMD64 Calling Convention
  mov rdi, rsp
  ; call C interrupt_handler function
  call interrupt_handler

  ; restore general purpose registers
  popaq
  ; pop trap number and errono off the stack
  add rsp, 16
  ; special return instruction for interrupts
  iretq

vector6:
  ; save general purpose registers
  pushaq
  ; trap number
  push 6
  ; errono
  push 0

  ; RDI is the first parameter according to the System V AMD64 Calling Convention
  mov rdi, rsp
  ; call C interrupt_handler function
  call interrupt_handler

  ; restore general purpose registers
  popaq
  ; pop trap number and errono off the stack
  add rsp, 16
  ; special return instruction for interrupts
  iretq

vector7:
  ; save general purpose registers
  pushaq
  ; trap number
  push 7
  ; errono
  push 0

  ; RDI is the first parameter according to the System V AMD64 Calling Convention
  mov rdi, rsp
  ; call C interrupt_handler function
  call interrupt_handler

  ; restore general purpose registers
  popaq
  ; pop trap number and errono off the stack
  add rsp, 16
  ; special return instruction for interrupts
  iretq

vector8:
  ; save general purpose registers
  pushaq
  ; trap number
  push 8
  ; errono
  push 0

  ; RDI is the first parameter according to the System V AMD64 Calling Convention
  mov rdi, rsp
  ; call C interrupt_handler function
  call interrupt_handler

  ; restore general purpose registers
  popaq
  ; pop trap number and errono off the stack
  add rsp, 16
  ; special return instruction for interrupts
  iretq
