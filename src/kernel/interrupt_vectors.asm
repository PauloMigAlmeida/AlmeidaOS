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
global vector39

; Error code is pushed onto the stacka already
%macro  vector_interrupt_errorcode_present_save_state 1
  ; trap number
  push %1
  ; save general purpose registers
  pushaq
  ; save system control registers
  pushacr
%endmacro

; No error code is returned from this vector, so we fake one to ensure we can use a single C struct for simplicity
%macro  vector_interrupt_plain_save_state 2
  ; errono
  push %2
  ; trap number
  push %1
  ; save general purpose registers
  pushaq
  ; save system control registers
  pushacr
%endmacro

%macro  vector_interrupt_restore_state 0
  ; pop system control registers off the stack
  add rsp, 40
  ; restore general purpose registers
  popaq
  ; pop trap number and errono off the stack
  add rsp, 16
%endmacro

%macro  vector_interrupt_body_generator 0
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
;	-> push all system control registers
;   -> push all general purpose registers
;   -> push trap number
;   -> push error number - not all interrupts have one but this ensure I can use
;         a single C struct for that :)
; Killed registers:
;   None
;===============================================================================
vector0:
  vector_interrupt_plain_save_state 0,0
  vector_interrupt_body_generator

vector1:
  vector_interrupt_plain_save_state 1,0
  vector_interrupt_body_generator

vector2:
  vector_interrupt_plain_save_state 2,0
  vector_interrupt_body_generator

vector3:
  vector_interrupt_plain_save_state 3,0
  vector_interrupt_body_generator

vector4:
  vector_interrupt_plain_save_state 4,0
  vector_interrupt_body_generator

vector5:
  vector_interrupt_plain_save_state 5,0
  vector_interrupt_body_generator

vector6:
  vector_interrupt_plain_save_state 6,0
  vector_interrupt_body_generator

vector7:
  vector_interrupt_plain_save_state 7,0
  vector_interrupt_body_generator

vector8:
  vector_interrupt_errorcode_present_save_state 8
  vector_interrupt_body_generator

vector10:
  vector_interrupt_errorcode_present_save_state 10
  vector_interrupt_body_generator

vector11:
  vector_interrupt_errorcode_present_save_state 11
  vector_interrupt_body_generator

vector12:
  vector_interrupt_errorcode_present_save_state 12
  vector_interrupt_body_generator

vector13:
  vector_interrupt_errorcode_present_save_state 13
  vector_interrupt_body_generator

vector14:
  vector_interrupt_errorcode_present_save_state 14
  vector_interrupt_body_generator

vector16:
  vector_interrupt_plain_save_state 16,0
  vector_interrupt_body_generator

vector17:
  vector_interrupt_errorcode_present_save_state 17
  vector_interrupt_body_generator

vector18:
  vector_interrupt_plain_save_state 18,0
  vector_interrupt_body_generator

vector19:
  vector_interrupt_plain_save_state 19,0
  vector_interrupt_body_generator

vector20:
  vector_interrupt_plain_save_state 20,0
  vector_interrupt_body_generator

vector21:
  vector_interrupt_errorcode_present_save_state 21
  vector_interrupt_body_generator

vector32:
  vector_interrupt_plain_save_state 32,0
  vector_interrupt_body_generator

vector33:
  vector_interrupt_plain_save_state 33,0
  vector_interrupt_body_generator

vector39:
  vector_interrupt_plain_save_state 39,0
  vector_interrupt_body_generator
