%include "../../../include/boot/global/macro.asm"

; Export references to C
global syscall_jump_usermode
global syscall_entry

extern syscall_handler

; create elf section that is always placed first when linking asm and c files
section .text

;align 16
syscall_jump_usermode:
	mov rcx, rdi ; to be loaded into RIP
	mov r11, 0x202 ; to be loaded into EFLAGS
	o64 sysret 	;use "o64 sysret" if you assemble with NASM

syscall_entry:
	; preserve all values so we can access them from C
	;  and know where to go back to (RCX, r11)
	pushaq

	; serve the system call
	call syscall_handler

	; restore / clean up the mess
	pop r15
 	pop r14
 	pop r13
 	pop r12
 	pop r11
 	pop r10
 	pop r9
 	pop r8
 	pop rsi
 	pop rdi
 	pop rbp
 	pop rdx
 	pop rcx
 	pop rbx
 	add rsp, 8       ; ditch rax value it's used for the return value

	; go back to where we came from
	o64 sysret

