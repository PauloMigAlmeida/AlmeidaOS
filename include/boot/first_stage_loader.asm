;=============================================================================
; @file first_stage_loader.inc
;
; Memory/Message constants and macros used by first stage loader code.
;=============================================================================

%ifndef __ALMEIDAOS_FSL_INC__
%define __ALMEIDAOS_FSL_INC__

;=============================================================================
; Memory layout
;=============================================================================
MBR.Mem.Stack.Top     equ   0x00007c00
Loader.Mem.Stack.Top  equ   0x00007e00

;===============================================================================
; Message Constants
;===============================================================================
Realmode.FirstStage.Booting.Msg             db '[AlmeidaOS] :: Booting BIOS First Stage Loader',0x0d,0x0a,0
Realmode.BIOSDiskExtensionPresent.Msg       db '[AlmeidaOS] :: BIOS Disk Extension is present',0x0d,0x0a,0
Realmode.BIOSDiskExtensionNotPresent.Msg    db '[AlmeidaOS] :: BIOS Disk Extension is not present... aborting',0x0d,0x0a,0
Realmode.BIOSDiskExtensionLoadingError.Msg  db '[AlmeidaOS] :: BIOS Disk Extension failed to read loader aborting',0x0d,0x0a,0

;===============================================================================
; Variables
;===============================================================================
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

bios_extended_read_sectors_from_drive:
  ; push values into the stack to preserve them once we are done with this fnc
  pusha

  ; 42h = function number for extended read
  mov ah, 0x42
  ; drive index (e.g. 1st HDD = 80h)
  mov dl, [BIOS.Drive.Id] ; (realmode.inc)

  ; Setting the DAP: Disk Address Packet
  ;
  ; mov the address of the variable BIOSDAPReadPacket to SI register
  mov si, BIOSDAPReadPacket
  ; offset: 00h  | range size: 1 byte | size of DAP (set this to 10h)
  mov byte[si], 0x10
  ; offset: 01h  | range size: 1 byte | unused, should be zero
  mov byte[si+1], 0
  ; offset: 02h..03h  | range size: 2 byte | number of sectors to be read
  mov word[si+2], Loader.File.NumberOfBlocks
  ; offset: 04h..07h  | range size: 4 byte | segment:offset pointer to the memory
  ;   buffer to which sectors will be transferred (note that x86 is
  ;   little-endian: if declaring the segment and offset separately,
  ;   the offset must be declared before the segment)
  mov word[si+4], Loader.Mem.Stack.Top
  mov word[si+6], 0
  ; offset: 08h..0Fh  | range size: 8 byte | absolute number of the start of the
  ;    sectors to be read (1st sector of drive has number 0) using logical block
  ;    addressing (note that the lower half comes before the upper half)
  ; PS: Since we are using dd and the block 0 is the MBR, then we set it to 1
  mov dword[si+8], 1
  mov dword[si+12], 0

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

%endif ; __ALMEIDAOS_FSL_INC__
