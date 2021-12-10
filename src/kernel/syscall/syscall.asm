%include "../../../include/boot/global/macro.asm"

; Notes to myself:
;	- Initially, we will have a routine that jumps to ring 3 via IRET to test whether
;		everything is working as expected. Once this is done, we shall move this to
;		syscall jump

; Export references to C
global syscall_jump_usermode
global interrupt_jump_usermode

; create elf section that is always placed first when linking asm and c files
section .text

interrupt_jump_usermode:
	mov ax, 0x18 | 3 	; ring 3 data with bottom 2 bits set for ring 3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax 			; SS is handled by iret


	; SS selector + RPL = 3 (Ring 3)
	push 0x18 | 3
	; RSP address that IRET will return to
	mov rax, 0x40000
	push rax
	; RFLAGS (inclusing interrupt flag = enabled)
    pushf
	; CS selector + RPL = 3 (Ring 3)
	push 0x20 | 3
	; RIP (place in which the CPU should start processing)
	push rdi
	iretq


align 16
syscall_jump_usermode:
	pushaq
	mov rcx, rdi ; to be loaded into RIP
	mov r11, 0x202 ; to be loaded into EFLAGS
	o64 sysret 	;use "o64 sysret" if you assemble with NASM
	popaq
	ret
