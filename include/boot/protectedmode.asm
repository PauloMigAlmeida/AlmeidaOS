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

;===============================================================================
; Message Constants
;===============================================================================
ProtectedMode.SecondStage.32IDTVec0.Msg  db '[AlmeidaOS] :: Div by 0 32-bit trap gate trigged',0x0d,0x0a,0
ProtectedMode.SecondStage.CleanPages.Msg db '[AlmeidaOS] :: Cleaning 4-level paging structure',0x0d,0x0a,0

;=============================================================================
; Global variables
;=============================================================================
cur_row:      dd 0x00
cur_col:      dd 0x00
screen_width: dd 0x00

;-----------------------------------------------------------------------------
; Interrupt Descriptor Table used (temporarily) in 32-bit protected mode
;-----------------------------------------------------------------------------
IDT32.Table:
.vec0:
    dw pm_div0_int_handler  ; offset bits 0..15
    dw 0x0008               ; a code segment selector in GDT or LDT
    db 0x00                 ; unused, set to 0
    db 10001111b            ; ( P=1, DPL=00b, S=0, type=1111b ) -> 32-bit trap gate
    dw 0x0000               ; offset bits 16..31

IDT32.Table.Size  equ  ($ - IDT32.Table)

IDT32.Table.Pointer:
  dw IDT32.Table.Size - 1
  dq IDT32.Table

;===============================================================================
; Functions
;===============================================================================

;===============================================================================
; pm_div0_int_handler:
;
; Dummy 32-bit trap gate handler that displays a message when triggered.
; The rationale for creating this function here is to get me used to the IDT
; structure (took me a while to understand it) as I know I will have to use it
; in long mode for real work.
;
; Refs: https://wiki.osdev.org/Interrupts_Descriptor_Table
;       https://stackoverflow.com/a/3425381/832748
;       https://www.amd.com/system/files/TechDocs/24593.pdf (Chapter 8)
;
; Killed registers:
;   None
;===============================================================================
pm_div0_int_handler:
  ; preserve the registers
  pusha;

  ; display the status message
  mov eax, ProtectedMode.SecondStage.32IDTVec0.Msg
  call pm_display_string

  ; restore the registers
  popa

  ; at this point of the booting process/developement, any int is 'fatal'.
  ; usually, we would use iret to return to the EIP that triggered this
  ; interruption but this would only trigger it again.. so I'm halting the os
  jmp pm_endless_loop
  ; iret

;===============================================================================
; pm_retrive_video_cursor_settings
;
; Obtains BIOS cursor position that was used in real mode so new messages can
; be printed where the BIOS stopped at
;
; Killed registers:
;   None
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


;===============================================================================
; pm_display_string
;
; Print funtion used in protected mode
;
; Killed registers:
;   None
;===============================================================================
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
    ; Each cell on the screen is two bytes. The current byte offset in video
    ; memory can be computed as 0xb8000+(cur_row * screen_width + cur_col) * 2

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


;===============================================================================
; set_cursor
;
; Set the hardware cursor position based on the current column (cur_col) and
; current row (cur_row) coordinates
; See:      https://wiki.osdev.org/Text_Mode_Cursor#Moving_the_Cursor_2
; See:      https://stackoverflow.com/a/53866689/832748
;
; Killed registers:
;   ecx, edx
;===============================================================================
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

;=============================================================================
; setup_page_tables
;
; Setup Paging mechanism used in long mode (64-bits). This routine cleans the
; memory used, create pages and enable paging
;
; Return flags:
;   None
;
; Killed registers:
;   None
;=============================================================================
pm_setup_page_tables:
  ; Preserve registers
  pusha

  ; TODO: set the PDE.PS bit (bit 7) cleared to 0, indicating a 4-Kbyte physical-page translation.

  ; Clean memory used to hold the page tables
  .clean_memory:
    ; Display status message
    mov eax, ProtectedMode.SecondStage.CleanPages.Msg
    call pm_display_string

    cld
    xor eax, eax
    xor ecx, ecx
    mov edi, Mem.PML4.Address
    mov ecx, (Paging.End.Address - Paging.Start.Address) >> 2
    rep stosd

  ; Setup pages structure and flag bits
  .setup_tables:
    ; Real address is left shifted 12 bits to allow page entry bits to be set
    .LeftShift equ 12
    ; Present (1) and ReadWrite(2) bits set
    .StdBits   equ 0x03

    ; Create a single entry [0] in PML4 Table.
    ;   -> one entry in a PML4T can address 512GB
    mov DWORD [Mem.PML4.Address], (Mem.PDPE.Address << .LeftShift) | .StdBits

    ; Create a single entry [0] in PDPT Table.
    ;   -> one entry in a PDPT can address 1GB
    mov DWORD [Mem.PDPE.Address], (Mem.PDE.Address << .LeftShift) | .StdBits

    ; Create entries [0...4] in PDE Table.
    ;   -> one entry in a PDE can address 2MB
    mov DWORD [Mem.PDE.Address], (Mem.PTE.Address << .LeftShift) | .StdBits
    mov DWORD [Mem.PDE.Address + 0x08], ((Mem.PTE.Address + 0x1000) << .LeftShift) | .StdBits
    mov DWORD [Mem.PDE.Address + 0x10], ((Mem.PTE.Address + 0x1000 * 2) << .LeftShift) | .StdBits
    mov DWORD [Mem.PDE.Address + 0x18], ((Mem.PTE.Address + 0x1000 * 3) << .LeftShift) | .StdBits
    mov DWORD [Mem.PDE.Address + 0x20], ((Mem.PTE.Address + 0x1000 * 4) << .LeftShift) | .StdBits

    ; Create all 512 entries in the PT table.
    ;   -> one entry in a PT can address 4Kb
    ; TODO: implement loop

  ; Restore registers.
  popa

  ret


pm_endless_loop:
; Disable interruptions
cli
  .end:
    hlt
    jmp .end

%endif ; __ALMEIDAOS_BOOT_PM_INC__
