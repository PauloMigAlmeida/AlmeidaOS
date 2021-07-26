;=============================================================================
; @file second_stage_loader.asm
;
; Memory/Message constants and macros used by second stage loader code.
;=============================================================================

%ifndef __ALMEIDAOS_SSL_INC__
%define __ALMEIDAOS_SSL_INC__


;===============================================================================
; Message Constants
;===============================================================================
Realmode.SecondStage.Booting.Msg            db 'Booting Second Stage Loader',CR,LF,0
Realmode.SecondStage.A20Enabled.Msg         db 'A20 enabled successfully',CR,LF,0
Realmode.SecondStage.A20EnablingError.Msg   db 'A20 could not be enabled. Aborting',CR,LF,0
Realmode.SecondStage.CPUIDNotSupported.Msg  db 'CPUID instruction is not available. Aborting',CR,LF,0
Realmode.SecondStage.64BitNotSupported.Msg  db '64-bit mode is not available. Aborting',CR,LF,0
Realmode.SecondStage.64BitSupported.Msg     db '64-bit mode is available',CR,LF,0
Realmode.SecondStage.LoadingGDT.Msg         db 'Loading 32-bit Global Table Descriptor',CR,LF,0
Realmode.SecondStage.EnteringPMode.Msg      db 'Enabling Protected Mode in the CPU',CR,LF,0
Realmode.SecondStage.e820NotSupported.Msg   db 'BIOS E820 is not supported ',CR,LF,0
Realmode.SecondStage.e820LoadingError.Msg   db 'BIOS E820 failed to read next memory entry',CR,LF,0
ProtectedMode.SecondStage.Booting.Msg       db 'Protected Mode (32-bit) was enabled',CR,LF,0


;===============================================================================
; Functions
;===============================================================================

;=============================================================================
; enable_A20
;
; Enable the A20 address line, so memory above 1MiB can be accessed.
;
; Returned flags:
;   CF      Set if enabled
;
; Killed registers:
;   None
;=============================================================================
enable_A20:

    ; Preserve ax register.
    push    ax

    ; Check if the A20 line is already enabled.
    call    test_A20
    jc      .done

    .attempt1:

      ; Attempt enabling with the BIOS.
      mov     ax,     0x2401
      int     0x15

      ; Check if A20 line is now enabled.
      call    test_A20
      jc      .done

    .attempt2:

      ; Attempt enabling with the keyboard controller.
      call    .attempt2.wait1

      ; Disable keyboard
      mov     al,     0xad
      out     0x64,   al
      call    .attempt2.wait1

      ; Read from input
      mov     al,     0xd0
      out     0x64,   al
      call    .attempt2.wait2

      ; Get keyboard data
      in      al,     0x60
      push    eax
      call    .attempt2.wait1

      ; Write to output
      mov     al,     0xd1
      out     0x64,   al
      call    .attempt2.wait1

      ; Send data
      pop     eax
      or      al,     2
      out     0x60,   al
      call    .attempt2.wait1

      ; Enable keyboard
      mov     al,     0xae
      out     0x64,   al
      call    .attempt2.wait1

      ; Check if the A20 line is now enabled.
      call    test_A20
      jc      .done

      jmp     .attempt3

      .attempt2.wait1:

        in      al,     0x64
        test    al,     2
        jnz     .attempt2.wait1
        ret

      .attempt2.wait2:

        in      al,     0x64
        test    al,     1
        jz      .attempt2.wait2
        ret

    .attempt3:

      ; Attempt enabling with the FAST A20 feature.
      in      al,     0x92
      or      al,     2
      out     0x92,   al
      xor     ax,     ax

      ; Check if A20 line is now enabled.
      call test_A20
      jc  .done

    .failed:
      ; Display a status message.
      mov si, Realmode.SecondStage.A20EnablingError.Msg
      call display_string

      ; hangs the system
      jmp endless_loop

    .done:
      ; Display a status message.
      mov si, Realmode.SecondStage.A20Enabled.Msg
      call display_string

      ; Restore register.
      pop     ax

      ret


;=============================================================================
; test_A20
;
; Check to see if the A20 address line is enabled.
;
; Return flags:
;   CF      Set if enabled
;
; Killed registers:
;   None
;=============================================================================
test_A20:

    ; Preserve registers.
    push    ds
    push    es
    pusha

    ; Initialize return result to "not enabled".
    clc

    ; Set es segment register to 0x0000.
    xor     ax,     ax
    mov     es,     ax

    ; Set ds segment register to 0xffff.
    not     ax
    mov     ds,     ax

    ; If the A20 line is disabled, then es:di and ds:si will point to the same
    ; physical memory location due to wrap-around at 1 MiB.
    ;
    ; es:di = 0000:0500 = 0x0000 * 16 + 0x0500 = 0x00500 = 0x0500
    ; ds:si = ffff:0510 = 0xffff * 16 + 0x0510 = 0x10500 = 0x0500
    mov     di,     0x0500
    mov     si,     0x0510

    ; Preserve the original values stored at es:di and ds:si.
    mov     ax,     [es:di]
    push    ax
    mov     ax,     [ds:si]
    push    ax

    ; Write different values to each logical address.
    mov     byte [es:di],   0x00
    mov     byte [ds:si],   0xff

    ; If a store to ds:si changes the value at es:di, then memory wrapped and
    ; A20 is not enabled.
    cmp     byte [es:di],   0xff

    ; Restore the original values stored at es:di and ds:si.
    pop     ax
    mov     [ds:si],    ax
    pop     ax
    mov     [es:di],    ax

    je      .done

    .enabled:

        ; Set the carry flag to indicate the A20 line is enabled.
        stc

    .done:

        ; Restore registers.
        popa
        pop     es
        pop     ds

        ret


;===============================================================================
; memzero
;
; Fill memory region with zero
;
; Inputs:
;   EDI ->  Buffer address
;   ECX ->  Amount of bytes to fill
;
; Killed registers:
;   None
;===============================================================================

memzero:
  ; preserve all registers
  pusha

  ; clear direction flag
  cld

  ; Define vale to be stored
  xor   eax,   eax

  ; repeat until ecx is 0
  rep stosb

  ; restore registers
  popa
  ret

;===============================================================================
; bios_e820_memory_map
;
; Read (free) memory available on this machine
;
; Killed registers:
;   None
;===============================================================================

bios_e820_memory_map:
  ; preserve all registers
  pusha

  ; never assume destination memory is zero'ed
  mov   edi,    e820.Mem.Start.Address
  mov   ecx,    (e820.Mem.End.Address - e820.Mem.Start.Address)
  call  memzero

  .first_memory_entry:
    ; BIOS Query System Address Map identifier
    mov   eax,  0xe820
    ; During first call, there is no continuation value to be passed
    xor   ebx,  ebx
    ; system map magic number 'SMAP'
    mov   edx,  0x534d4150
    ; Memory location where this should storead
    mov   edi,  e820.Mem.Start.Address
    ; The length in bytes of the structure passed to the BIOS.
    mov   ecx,  20
    ; call BIOS fuction
    int   0x15

    ; Carry flag set during first call to BIOS means this isn't supported
    jc    .not_supported

  .loop:

    ; ebx is set to 0 if we hit the end of the list
    test  ebx,  ebx
    jz  .done

    ; BIOS Query System Address Map identifier
    mov   eax,  0xe820
    ; system map magic number 'SMAP'
    mov   edx,  0x534d4150
    ; The length in bytes of the structure passed to the BIOS.
    mov   ecx,  20
    ; buffer pointer isn't moved automatically, so we do it manually
    add   di,   cx
    ; call BIOS fuction
    int   0x15

    ; Carry Flag set means that we had problems
    jc  .error

    ; test if BIOS signature is the one we expect
    cmp   eax,  0x534d4150
    jne   .error

    ; get next entry of the list
    jmp   .loop

  .not_supported:

    ; print error message if there is space in the mbr file
    mov   si,   Realmode.SecondStage.e820NotSupported.Msg
    call  display_string

    ; halt the machine as there is no other way to continue booting the os
    jmp   endless_loop

  .error:

    ; print error message if there is space in the mbr file
    mov   si,   Realmode.SecondStage.e820LoadingError.Msg
    call  display_string

    ; halt the machine as there is no other way to continue booting the os
    jmp   endless_loop

  .done:
    ; restore registers
    clc
    popa
    ret

;=============================================================================
; HasCPUID
;
; Detect if the cpu supports the CPUID instruction.
;
; Bit 21 of the EFLAGS register can be modified only if the CPUID instruction
; is supported.
;
; Return flags:
;   CF      Set if CPUID is supported
;
; Killed registers:
;   None
;=============================================================================
HasCPUID:

  ; Preserve registers.
  push    eax
  push    ecx

  ; Copy flags to eax and ecx.
  pushfd
  pop     eax
  mov     ecx,    eax

  ; Set flag 21 (the ID bit)
  xor     eax,    (1 << 21)
  push    eax
  popfd

  ; Copy flags back to eax. If CPUID is supported, bit 21 will still be set.
  pushfd
  pop     eax

  ; Restore the original flags from ecx.
  push    ecx
  popfd

  ; Initialize the return flag (carry) to unsupported.
  clc

  ; If eax and ecx are equal, then flag 21 didn't remain set, and CPUID is
  ; not supported.
  xor     eax,    ecx
  jz      .done       ; CPUID is not supported

  .supported:

    ; CPUID is supported.
    stc

  .done:

    ; Restore registers.
    pop     ecx
    pop     eax

    ret

cpu_supports_64_bit_mode:
    ; Detect if the cpuid instruction is available.
    call    HasCPUID
    jnc     .error.noCPUID

    ; Is the processor info function supported?
    mov     eax,    0x80000000  ; Get Highest Extended Function Supported
    cpuid
    cmp     eax,    0x80000001
    jb      .error.no64BitMode

    ; Use processor info function to see if 64-bit mode is supported.
    mov     eax,    0x80000001  ; Extended Processor Info and Feature Bits
    cpuid
    test    edx,    (1 << 29)   ; 64-bit mode bit
    jz      .error.no64BitMode

    ; Clear 32-bit register values.
    xor     eax,    eax
    xor     edx,    edx

    ; Display a status message.
    mov     si,     Realmode.SecondStage.64BitSupported.Msg
    call    display_string
    ; returns from function
    ret

    .error.noCPUID:
      ; Display a status message.
      mov     si,     Realmode.SecondStage.CPUIDNotSupported.Msg
      call    display_string
      ; hangs the system
      jmp endless_loop

    .error.no64BitMode:
      ; Display a status message.
      mov     si,     Realmode.SecondStage.64BitNotSupported.Msg
      call    display_string
      ; hangs the system
      jmp endless_loop


;=============================================================================
; Enter_protected_mode
;
; Prepare the system to enter the protected mode (32-bits).
;
; Return flags:
;   None
;
; Killed registers:
;   All, nothing will be saved.
;=============================================================================
enter_protected_mode:

; Instructions from the Intel 64 manual
; 9.9.1 Switching to Protected Mode
;
; Before switching to protected mode from real mode, a minimum set of system data structures and code modules must be loaded into memory, as described in Section 9.8, “Software Initialization for Protected-Mode Operation.” Once these tables are created, software initialization code can switch into protected mode.
; Protected mode is entered by executing a MOV CR0 instruction that sets the PE flag in the CR0 register. (In the same instruction, the PG flag in register CR0 can be set to enable paging.) Execution in protected mode begins with a CPL of 0.
; Intel 64 and IA-32 processors have slightly different requirements for switching to protected mode. To insure upwards and downwards code compatibility with Intel 64 and IA-32 processors, we recommend that you follow these steps:
; 1. Disable interrupts. A CLI instruction disables maskable hardware interrupts. NMI interrupts can be disabled with external circuitry. (Software must guarantee that no exceptions or interrupts are generated during the mode switching operation.)
; 2. Execute the LGDT instruction to load the GDTR register with the base address of the GDT.
; 3. Execute a MOV CR0 instruction that sets the PE flag (and optionally the PG flag) in control register CR0.
; 4. Immediately following the MOV CR0 instruction, execute a far JMP or far CALL instruction. (This operation is typically a far jump or call to the next instruction in the instruction stream.)
; 5. The JMP or CALL instruction immediately after the MOV CR0 instruction changes the flow of execution and serializes the processor.
; 6. If paging is enabled, the code for the MOV CR0 instruction and the JMP or CALL instruction must come from a page that is identity mapped (that is, the linear address before the jump is the same as the physical address after paging and protected mode is enabled). The target instruction for the JMP or CALL instruction does not need to be identity mapped.
; 7. If a local descriptor table is going to be used, execute the LLDT instruction to load the segment selector for the LDT in the LDTR register.
; 8. Execute the LTR instruction to load the task register with a segment selector to the initial protected-mode task or to a writable area of memory that can be used to store TSS information on a task switch.
; 9. After entering protected mode, the segment registers continue to hold the contents they had in real-address mode. The JMP or CALL instruction in step 4 resets the CS register. Perform one of the following operations to update the contents of the remaining segment registers.
; — Reload segment registers DS, SS, ES, FS, and GS. If the ES, FS, and/or GS registers are not going to be used, load them with a null selector.
; — Perform a JMP or CALL instruction to a new task, which automatically resets the values of the segment registers and branches to a new code segment.
; 10. Execute the LIDT instruction to load the IDTR register with the address and limit of the protected-mode IDT.
; 11. Execute the STI instruction to enable maskable hardware interrupts and perform the necessary hardware operation to enable NMI interrupts.

  ; Disable interruptions
  cli

  ; print status message (yes, int x10 is a software interrupt hence not affected by cli)
  mov si, Realmode.SecondStage.LoadingGDT.Msg
  call display_string

  ; Load 32-bit GDT
  lgdt [GDT32.Table.Pointer]

  ; print status message
  mov si, Realmode.SecondStage.EnteringPMode.Msg
  call display_string

  ; Enable protected mode.
  mov     eax,    cr0
  or      eax,    (1 << 0)    ; CR.PE
  mov     cr0,    eax

  ; Accoring to Intel 64 manual
  ; Section: 9.9.1 Switching to Protected Mode
  ;
  ; 4. Immediately following the MOV CR0 instruction,
  ;    execute a far JMP or far CALL instruction.
  ;    (This operation is typically a far jump or call
  ;     to the next instruction in the instruction stream.)
  jmp 0x08:protected_mode_boot


%endif ; __ALMEIDAOS_SSL_INC__
