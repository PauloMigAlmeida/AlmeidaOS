%include "../../../include/boot/global/macro.asm"

; Export references to C
global syscall_entry

extern syscall_handler

; create elf section that is always placed first when linking asm and c files
section .text

syscall_entry:
	;TODO: Test (while I figure out how to disable interrupts via SFMASK)
	; the idea is to not have to deal with preemptable stuff that can cause
	; a syscall to be stopped half-way to server an interrupt and then come back.
	; While this seems pretty cool to figure out, it will for certain take a
	; little longer as debugging becomes really hard.
	cli

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

