;=============================================================================
; @file longmode.asm
;
; Memory/Message constants and macros used by boot loader code.
;=============================================================================

%ifndef __ALMEIDAOS_BOOT_LM_INC__
%define __ALMEIDAOS_BOOT_LM_INC__

;=============================================================================
; Constants
;=============================================================================
LM.Video_Text.Addr        equ 0xb8000
LM.Video_Text.Colour      equ 0x0a    ; Green on black attribute
;===============================================================================
; Message Constants
;===============================================================================
LongMode.SecondStage.Booting.Msg  db '[AlmeidaOS] :: Long mode (64-bit) was enabled in the CPU \o/',0x0d,0x0a,0

;===============================================================================
; NASM Macros
;===============================================================================
%macro pushaq 0
    push rax      ;save current rax
    push rbx      ;save current rbx
    push rcx      ;save current rcx
    push rdx      ;save current rdx
    push rbp      ;save current rbp
    push rdi       ;save current rdi
    push rsi       ;save current rsi
    push r8        ;save current r8
    push r9        ;save current r9
    push r10      ;save current r10
    push r11      ;save current r11
    push r12      ;save current r12
    push r13      ;save current r13
    push r14      ;save current r14
    push r15      ;save current r15
%endmacro

%macro popaq 0
  pop r15       ;restore r15
  pop r14       ;restore r14
  pop r13       ;restore r13
  pop r12       ;restore r12
  pop r11       ;restore r11
  pop r10       ;restore r10
  pop r9        ;restore r9
  pop r8        ;restore r8
  pop rsi       ;restore rsi
  pop rdi       ;restore rdi
  pop rbp       ;restore rbp
  pop rdx       ;restore rdx
  pop rcx       ;restore rcx
  pop rbx       ;restore rbx
  pop rax       ;restore rax
%endmacro

;===============================================================================
; Functions
;===============================================================================
;===============================================================================
; lm_display_string
;
; Print funtion used in protected mode
;
; Killed registers:
;   None
;===============================================================================
lm_display_string:
  pushaq

  ; move pointer to ebx so I keep eax free for later use
  mov ebx, eax

  ; clean up registers that will be used
  xor eax, eax
  xor ecx, ecx
  xor edx, edx

  .loop:
    ; Grab one character
    mov dl, byte[ebx + ecx]

    ; check if that's the end of the string
    cmp dl, 0
    je .done

    ; check if that's the carriage return char
    cmp dl, CR
    je .cr_lf

    ; check if that's the LF char
    cmp dl, LF
    je .cr_lf

  .print_char:
    ; Each cell on the screen is two bytes. The current byte offset in video
    ; memory can be computed as 0xb8000+(cur_row * screen_width + cur_col) * 2

    ; mul instruction will override the content of edx, save that to the stack
    push rdx
    ; (cur_row * screen_width + cur_col)
    mov eax, [cur_row]
    mul dword [screen_width]
    add eax, [cur_col]
    ; restore that
    pop rdx

    ; send char to to the video memory address
    mov byte[LM.Video_Text.Addr + eax * 2], dl
    ; set char colour
    mov byte[LM.Video_Text.Addr + eax * 2 + 1], LM.Video_Text.Colour

    ; incr ecx so next time the next char will be grabbed from memory
    inc ecx
    ; update cursor location
    inc dword [cur_col]
    ; loop
    jmp .loop

  .cr_lf:
    ; incr ecx so next time the next char will be grabbed from memory
    inc ecx

    ; if LF than we may want to update cur_row, otherwise ignore CR
    cmp dl, LF
    je .break_line

    ; if CR then return to the loop
    jmp .loop

  .break_line:
    ; reset the cursor pointers
    inc dword [cur_row]
    mov dword[cur_col], 0
    je .loop

  .done:

    call lm_set_cursor             ; Update hardware cursor position

    ; restore registers
    popaq
    ;return
    ret


;===============================================================================
; lm_set_cursor
;
; Set the hardware cursor position based on the current column (cur_col) and
; current row (cur_row) coordinates
; See:      https://wiki.osdev.org/Text_Mode_Cursor#Moving_the_Cursor_2
; See:      https://stackoverflow.com/a/53866689/832748
;
; Killed registers:
;   ecx, edx
;===============================================================================
lm_set_cursor:
    ; EAX = cur_row
    mov ecx, [cur_row]
    ; ECX = cur_row * screen_width
    imul ecx, [screen_width]
    ; ECX = cur_row * screen_width + cur_col
    add ecx, [cur_col]

    ; Send low byte of cursor position to video card
    mov edx, 0x3d4
    mov al, 0x0f
    ; Output 0x0f to 0x3d4
    out dx, al
    inc edx
    mov al, cl
    ; Output lower byte of cursor pos to 0x3d5
    out dx, al

    ; Send high byte of cursor position to video card
    dec edx
    mov al, 0x0e
    ; Output 0x0e to 0x3d4
    out dx, al
    inc edx
    mov al, ch
    ; Output higher byte of cursor pos to 0x3d5
    out dx, al

    ret

lm_endless_loop:
; Disable interruptions
cli
  .end:
    hlt
    jmp .end

%endif ; __ALMEIDAOS_BOOT_LM_INC__
