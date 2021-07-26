;=============================================================================
; @file realmode.inc
;
; Memory/Message constants and macros used by boot loader code.
;=============================================================================

%ifndef __ALMEIDAOS_BOOT_MEM_INC__
%define __ALMEIDAOS_BOOT_MEM_INC__

;===============================================================================
; Message Constants
;===============================================================================
Realmode.FirstStage.Booting.Msg             db 'Booting BIOS First Stage Loader',CR,LF,0
Realmode.BIOSDiskExtensionPresent.Msg       db 'BIOS Disk Extension is present',CR,LF,0
Realmode.BIOSDiskExtensionNotPresent.Msg    db 'BIOS Disk Extension is not present... aborting',CR,LF,0
Realmode.BIOSDiskExtensionLoadingError.Msg  db 'BIOS Disk Extension failed to read loader aborting',CR,LF,0
;=============================================================================
; Global variables
;=============================================================================
BIOS.Drive.Id   equ   0

; Specifically used during BIOS read sectors operations
BIOSDAPReadPacket times 16 db 0


;===============================================================================
; Functions
;===============================================================================

bios_check_extensions_present:
  ; push values into the stack to preserve them once we are done with this fnc
  pusha

  ;41h = function number for extensions check
  mov ah, 0x41
  ; Drive index (e.g. 1st HDD = 80h)
  mov dl, [BIOS.Drive.Id] ; (realmode.inc)
  ; signature required by this function
  mov bx, 0x55aa
  ; call int 13h ah=41h: Check Extensions present
  ; Ref: https://en.wikipedia.org/wiki/INT_13H#INT_13h_AH=41h:_Check_Extensions_Present
  int 0x13

  ; Results
  ; CF -> Set On Not Present, Clear If Present
  jc .not_found
  ; BX -> must be equal to AA55h
  cmp bx, 0xaa55
  je .found

  .not_found:
    ; print message
    mov si, Realmode.BIOSDiskExtensionNotPresent.Msg
    call display_string
    ; halt the machine as there is no other way to continue booting the os
    jmp endless_loop
  .found:
    ; print message
    mov si, Realmode.BIOSDiskExtensionPresent.Msg
    call display_string
    ; restore values from the stack
    popa
    ret

;===============================================================================
; bios_extended_read_sectors_from_drive
;
; Read IO blocks from disk and place them into the desired memory destination
;
; Input:
;   EAX -> Destination in which blocks will be loaded in memory
;   EBX -> Number of blocks to be read from disk
;   ECX -> Absolute block number from which the reading procedure will start
;
; Killed registers:
;   None
;===============================================================================

bios_extended_read_sectors_from_drive:
  ; push values into the stack to preserve them once we are done with this fnc
  pusha

  ; Setting the DAP: Disk Address Packet
  ;
  ; mov the address of the variable BIOSDAPReadPacket to SI register
  mov si, BIOSDAPReadPacket
  ; offset: 00h  | range size: 1 byte | size of DAP (set this to 10h)
  mov byte[si], 0x10
  ; offset: 01h  | range size: 1 byte | unused, should be zero
  mov byte[si+1], 0
  ; offset: 02h..03h  | range size: 2 byte | number of sectors to be read
  ;  (some Phoenix BIOSes are limited to a maximum of 127 sectors)
  mov word[si+2], bx
  ; offset: 04h..07h  | range size: 4 byte | segment:offset pointer to the memory
  ;   buffer to which sectors will be transferred (note that x86 is
  ;   little-endian: if declaring the segment and offset separately,
  ;   the offset must be declared before the segment)
  mov dword[si+4], eax
  ; offset: 08h..0Fh  | range size: 8 byte | absolute number of the start of the
  ;    sectors to be read (1st sector of drive has number 0) using logical block
  ;    addressing (note that the lower half comes before the upper half)
  ; PS: Since we are using dd and the block 0 is the MBR, then we set it to 1
  mov dword[si+8], ecx
  mov dword[si+12], 0

  ; 42h = function number for extended read
  mov ah, 0x42
  ; drive index (e.g. 1st HDD = 80h)
  mov dl, [BIOS.Drive.Id] ; (realmode.inc)

  ; call INT 13h AH=42h: Extended Read Sectors From Drive
  ; Ref: https://en.wikipedia.org/wiki/INT_13H#INT_13h_AH=42h:_Extended_Read_Sectors_From_Drive
  int 0x13
  ; Results
  ; CF -> Set On Error, Clear If No Error
  jc .error
  ; restore values from the stack
  popa
  ret

  .error:
    ; print message
    mov si, Realmode.BIOSDiskExtensionLoadingError.Msg
    call display_string
    ; halt the machine as there is no other way to continue booting the os
    jmp endless_loop

display_string:
    ; preserve all registers
    pusha

    ; preserve original message so we can print the log prefix
    push si
    mov si, LOG_PREFIX
    call display_string_internal

    ; get the original message reference back
    pop si
    call display_string_internal

    ; restore registers
    popa
    ret

display_string_internal:
    pusha

    mov     ah,     0x0e    ; int 10 AH=0x0e
    xor     bx,     bx

    cld

    .loop:

        ; Read next string character into al register.
        lodsb

        ; Break when a null terminator is reached.
        cmp     al,     0
        je      .done

        ; Call int 10 function 0eh (print character to teletype)
        int     0x10
        jmp     .loop

    .done:

        popa
        ret


endless_loop:
; Disable interruptions
cli
  .end:
    hlt
    jmp .end

%endif ; __ALMEIDAOS_BOOT_MEM_INC__
