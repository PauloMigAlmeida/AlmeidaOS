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
global vector10
global vector11
global vector12
global vector13
global vector14
global vector16
global vector17
global vector18
global vector19
global vector20
global vector21
global vector32
global vector33


%macro  vector_interrupt_save_state 2
  ; save general purpose registers
  pushaq
  ; trap number
  push %1
  ; errono
  push %2
%endmacro

%macro  vector_interrupt_restore_state 0
  ; restore general purpose registers
  popaq
  ; pop trap number and errono off the stack
  add rsp, 16
%endmacro

%macro  vector_interrupt_body_generator 2
  ; save general purpose registers
  vector_interrupt_save_state %1,%2

  ; SystemV ABI requires DF to be clear on function entry
  cld
  ; RDI is the first parameter according to the System V AMD64 Calling Convention
  mov rdi, rsp
  ; call C interrupt_handler function
  call interrupt_handler

  ; restore general purpose registers
  vector_interrupt_restore_state
  ; special return instruction for interrupts
  iretq
%endmacro

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
  vector_interrupt_body_generator 0,0

vector1:
  vector_interrupt_body_generator 1,0

vector2:
  vector_interrupt_body_generator 2,0

vector3:
  vector_interrupt_body_generator 3,0

vector4:
  vector_interrupt_body_generator 4,0

vector5:
  vector_interrupt_body_generator 5,0

vector6:
  vector_interrupt_body_generator 6,0

vector7:
  vector_interrupt_body_generator 7,0

vector8:
  vector_interrupt_body_generator 8,0

vector10:
  vector_interrupt_body_generator 10,0

vector11:
  vector_interrupt_body_generator 11,0

vector12:
  vector_interrupt_body_generator 12,0

vector13:
  vector_interrupt_body_generator 13,0

vector14:
  vector_interrupt_body_generator 14,0

vector16:
  vector_interrupt_body_generator 16,0

vector17:
  vector_interrupt_body_generator 17,0

vector18:
  vector_interrupt_body_generator 18,0

vector19:
  vector_interrupt_body_generator 19,0

vector20:
  vector_interrupt_body_generator 20,0

vector21:
  vector_interrupt_body_generator 21,0

vector32:
  vector_interrupt_body_generator 32,0

vector33:
  vector_interrupt_body_generator 33,0
