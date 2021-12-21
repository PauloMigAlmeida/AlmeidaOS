
; Include useful functions, constants and macros
%include "../../../include/boot/global/macro.asm"
%include "../../../include/boot/global/const.asm"
%include "../../../include/boot/global/mem.asm"


; Export references to C
global syscall_entry

extern syscall_handler

; create elf section that is always placed first when linking asm and c files
section .text

syscall_entry:
	; TODO: read from TSS
	; Ideally we should point this to the kernel stack in the TSS.rsp0. For some reason I'm having trouble accessing it correctly
	; Right now we are not dealing with preemtable kernel so pointing it to the original
	; kernel stack will work.
	push rsp
	mov rsp, Kernel.New.Start.VirtualAddress

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

 	pop rsp

	; go back to where we came from
	o64 sysret

