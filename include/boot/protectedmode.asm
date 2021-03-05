;=============================================================================
; @file protectedmode.asm
;
; Memory/Message constants and macros used by boot loader code.
;=============================================================================

%ifndef __ALMEIDAOS_BOOT_PM_INC__
%define __ALMEIDAOS_BOOT_PM_INC__

;=============================================================================
; Constants
;=============================================================================
PM.Video_Text.Addr        equ 0xb8000
PM.Video_Text.Colour      equ 0x07    ; White on black attribute

;=============================================================================
; Global variables
;=============================================================================
cur_row:      dd 0x00
cur_col:      dd 0x00
screen_width: dd 0x00

;===============================================================================
; Functions
;===============================================================================

pm_retrive_video_cursor_settings:
  ; To be able to print content using 0xb8000 below the last message, we have to
  ; pull the position where the BIOS cursor stopped at. For that we are going to
  ; use the Bias Data Area which is located between 00000400 - 000004ff.
  ;
  ; The parts I'm interested in are:
  ;     40:50  8 words	Cursor position of pages 1-8, high order byte=row
  ;		                  low order byte=column; changing this data isn't
  ;		                  reflected immediately on the display
  ;
  ;                    Addr: 0x40 << 4 + 0x50 = 0x450
  ;                    ********************
  ;                    *  Byte 1 : Byte 2 *
  ;                    ********************
  ;
  ;                    In theory, Byte 1 should be the high order byte and
  ;                    Byte 2 should be the low order byte, however due to the
  ;                    x86_64 endianess, it's the other way around
  ;
  ;     40:4A	  word	Number of screen columns

  ; preserve eax register
  push eax

  ; Clear EAX for the instructions below
  xor eax, eax

  ; last BIOS column position
  mov al, [0x450]

  ; Copy to current column
  mov [cur_col], eax

  ; last BIOS row position
  mov al, [0x450 + 1]

  ; Copy to current row
  mov [cur_row], eax

  ; Word at address 0x44a = # of columns (screen width)
  mov ax, word[0x44a]

  ; Copy to screen width
  mov [screen_width], eax

  ; restore eax
  pop eax

  ; return
  ret



; eax -> 1 parameter - address of the string to be printed
;
; Each cell on the screen is two bytes. The current byte offset in video
; memory can be computed as 0xb8000+(cur_row * screen_width + cur_col) * 2
pm_display_string:
  pusha

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
    ; mul instruction will override the content of edx, save that to the stack
    push edx
    ; (cur_row * screen_width + cur_col)
    mov eax, [cur_row]
    mul dword [screen_width]
    add eax, [cur_col]
    ; restore that
    pop edx

    ; send char to to the video memory address
    mov byte[PM.Video_Text.Addr + eax * 2], dl
    ; set char colour
    mov byte[PM.Video_Text.Addr + eax * 2 + 1], PM.Video_Text.Colour

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

    call set_cursor             ; Update hardware cursor position

    ; restore registers
    popa
    ;return
    ret

; Function: set_cursor
;           set the hardware cursor position based on the
;           current column (cur_col) and current row (cur_row) coordinates
; See:      https://wiki.osdev.org/Text_Mode_Cursor#Moving_the_Cursor_2
;
; Inputs:   None

set_cursor:
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





; pm_display_string:
;     pusha
;     mov esi, eax                ; Set ESI to beginning of string
;
;     ; Assume base of text video memory is ALWAYS 0xb8000
;     mov ebx, PM.Video_Text.Addr    ; EBX = beginning of video memory
;
;     mov eax, [cur_row]          ; EAX = cur_row
;     mul dword [screen_width]    ; EAX = cur_row * screen_width
;     mov edx, eax                ; EDX = copy of offset to beginning of line
;     add eax, [cur_col]          ; EAX = cur_row * screen_width + cur_col
;     lea edi, [ebx + eax * 2]    ; EDI = memory location of current screen cell
;
;     mov ah, PM.Video_Text.Colour ; Set attribute
;     jmp .getch
; .repeat:
;     cmp al, 0xd                  ; Is the character a carriage return?
;     jne .chk_lf                 ;     If not skip and check for line feed
;     lea edi, [ebx + edx * 2]    ; Set current video memory pointer to beginning of line
;     mov dword [cur_col], 0      ; Set current column to 0
;     jmp .getch                  ; Process next character
; .chk_lf:
;     cmp al, 0xa                  ; Is the character a line feed?
;     jne .write_chr              ;     If not then write character
;     mov eax, [screen_width]
;     lea edi, [edi + eax * 2]    ; Set current video memory ptr to same pos on next line
;     inc dword [cur_row]         ; Set current row to next line
;     mov ah, PM.Video_Text.Colour ; Reset attribute
;     jmp .getch                  ; Process next character
;
; .write_chr:
;     inc dword [cur_col]         ; Update current column
;     stosw
;
; .getch:
;     lodsb                       ; Get character from string
;     test al, al                 ; Have we reached end of string?
;     jnz .repeat                 ;     if not process next character
;
; .end:
;     ; call set_cursor             ; Update hardware cursor position
;
;     popa
;     ret


pm_endless_loop:
; Disable interruptions
cli
  .end:
    hlt
    jmp .end

%endif ; __ALMEIDAOS_BOOT_PM_INC__
