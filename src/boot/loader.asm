[BITS 16]
[ORG 0x7E00]

; Produce a map file containing all symbols and sections.
[map all ../../build/boot/loader.map]

;===============================================================================
; loader
;
; Second-stage boot loader entry point
;
; Due to the limited size of the first-stage boot loader, its only mission in
; life is to load the Second-stage boot leader. The second-stage boot loader is
; responsible for essentially preparing the system to enter into the protected
; mode.
;
; Input registers:
;
;   DL      Boot drive number
;
; Memory layout before this code starts running:
;
;   00000000 - 000003ff        1,024 bytes     Real mode IVT
;   00000400 - 000004ff          256 bytes     BIOS data area
;   00000500 - 00007bff       30,464 bytes     Free
;   00007c00 - 00007dff          512 bytes     First-stage boot loader (MBR)
;   00007e00 - 0009fbff      622,080 bytes     Free -> We are here now
;   0009fc00 - 0009ffff        1,024 bytes     Extended BIOS data area (EBDA)
;   000a0000 - 000bffff      131,072 bytes     BIOS video memory
;   000c0000 - 000fffff      262,144 bytes     ROM
;===============================================================================

; jump to start procedure
jmp start

; Include functions/constants that are useful in real mode
%include "../../include/boot/realmode.asm"
%include "../../include/boot/second_stage_loader.asm"

start:

  ; Proper initialisation of stack during BIOS bootloader
  ;   https://stackoverflow.com/a/33975465/832748
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov bx, Loader.Mem.Stack.Top

  ; Turn off interrupts for SS:SP update to avoid a problem with buggy 8088 CPUs
  cli
  ; SS = 0x0000
  mov ss, ax
  ; SP = 0x7c00
  ; Set the stack starting just below where the bootloader is at 0x0:0x7c00.
  mov sp, bx
  ; Turn interrupts back on
  sti

boot:
  ; Save DriveId for later
  mov [BIOS.Drive.Id], dl

  ; Print booting message
  mov si, Realmode.SecondStage.Booting.Msg
  call display_string

  ; Attempt to enable the A20 line if necessary.
  call enable_A20

  ; Check whether we are running on a 64-bit processor
  call cpu_supports_64_bit_mode

  ; Prepare to enter protected mode
  call enter_protected_mode

  ; enter a endless loop. This instruction should never be reached
  jmp endless_loop


[BITS 32]

; Include Constants/Variables/routines useful in protected mode
%include "../../include/boot/protectedmode.asm"


protected_mode_boot:

; Accoring to Intel 64 manual
; Section: 9.9.1 Switching to Protected Mode
;
;   9. After entering protected mode, the segment registers continue to hold the
;      contents they had in real-address mode. The JMP or CALL instruction in
;     step 4 resets the CS register. Perform one of the following operations to
;     update the contents of the remaining segment registers.
;
;     — Reload segment registers DS, SS, ES, FS, and GS.
;       If the ES, FS, and/or GS registers are not going to be used,
;       load them with a null selector.

  ; 0x10 = 16 which points to the GDT Data Segment (and the right DPL too)
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  mov esp, Loader.Mem.Stack.Top


  ; 10. Execute the LIDT instruction to load the IDTR register with the address
  ;   and limit of the protected-mode IDT.
  lidt [IDT32.Table.Pointer]

  ; 11. Execute the STI instruction to enable maskable hardware interrupts and
  ;   perform the necessary hardware operation to enable NMI interrupts.
  sti

  ; fix video text printing
  call pm_retrive_video_cursor_settings

  ; display status message
  mov eax, ProtectedMode.SecondStage.Booting.Msg
  call pm_display_string

  ;===============================================================================
  ; Paging study area:
  ;
  ; I've been struggling to fully grasp paging given the complexity of the subject
  ; and the fact that it's hard to keep track of all the small details after a few
  ; days. I'm gonna just dump relevant bits here to help me with the implementation
  ;
  ; Paging
  ;   to enable paging in long mode with 4kb size: (More info on page 189 AMD 64 manual)
  ;   * CR4.PAE=Enabled
  ;   * PDPE.PS=0
  ;   * PDE.PS=0
  ;   * Set CR3 to PML4 base address
  ;   Maximum virtual address -> 64-bit
  ;   Maximum physical address -> 52-bit
  ;
  ;
  ; Page translation is controlled by the PG bit in CR0 (bit 31).
  ;   -> Set to 1 to enable
  ; Physical-address extensions are controlled by the PAE bit in CR4 (bit 5)
  ;   -> Setting CR4.PAE = 1 enables virtual addresses to be translated into
  ;       physical addresses up to 52 bits long.
  ; Page-Size Extensions (PSE) Bit ->  CR4 (bit 4)
  ;   -> If both CR4.PAE=0 and CR4.PSE=0, the only available page size is 4 Kbytes. (which is the one I want)
  ;   -> The value of CR4.PSE is ignored when long mode is active. This is because
  ;        physical-address extensions must be enabled in long mode, and
  ;         the only available page sizes are 4 Kbytes and 2 Mbytes.
  ;
  ;
  ; Long mode translation:
  ;   -> Before activating long mode, PAE must be enabled by setting CR4.PAE to 1.
  ;   -> Activating long mode before enabling PAE causes a general-protection
  ;       exception (#GP) to occur.
  ;
  ; Canonical Address Form:
  ;   -> The AMD64 architecture requires implementations supporting fewer than
  ;       the full 64-bit virtual address to ensure that those addresses are in
  ;       canonical form.
  ;
  ; CR3
  ;   -> Points to the PML4 base address
  ;   -> CR3 is expanded to 64 bits in long mode, allowing the PML4 table to be
  ;       located anywhere in the 52-bit physical-address space.
  ;   -> Details about the contents of each bit can be found on page 200 of the AMD 64 manual)
  ;
  ; 4-Kbyte Page Translation:
  ;   -> In long mode, 4-Kbyte physical-page translation is performed by dividing
  ;       the virtual address into six fields
  ;
  ;     Bits 63:48 are a sign extension of bit 47, as required for canonical-address forms.
  ;     Bits 47:39 index into the 512-entry page-map level-4 table.
  ;     Bits 38:30 index into the 512-entry page-directory pointer table.
  ;     Bits 29:21 index into the 512-entry page-directory table.
  ;     Bits 20:12 index into the 512-entry page table.
  ;     Bits 11:0 provide the byte offset into the physical page.
  ;
  ; Misc:
  ; -> In long mode, however, you only have 512 entries per table as each entry is eight bytes long.
  ;     This means that,,,,
  ;         -> one entry in a PT can address 4kB,
  ;         -> one entry in a PDT can address 2MB,
  ;         -> one entry in a PDPT can address 1GB and
  ;         -> one entry in a PML4T can address 512GB.
  ;      ... This means that only 256TB can be addressed.
  ;
  ;
  ;===============================================================================

  ; TODO -> Find an area of the memory to set up paging.. I only need 8192 bytes
  ;   -> Done: Defined in mem.asm (Mem.PML4.Start.Address - 0x10000)
  ; TODO -> Set up paging
  ;   -> Sub tasks:
  ;       -> Clean memmory that's gonna be used by the pages ->
  ;         -> I thought that cleaning was done...but not really...I feel like I'm either overcleaning or undercleaning it
  ;             https://stackoverflow.com/questions/45665451/how-does-this-osdev-identity-map-work-for-pae-paging
  ;             https://gist.github.com/hibariya/9dc9b836e39a04300a410e92368dec7d
  ;       -> Set up the pages there with the right bits set
  ;       ->
  ; TODO -> Load GDT 64 after paging is configured.
  ; TODO -> Enter Long mode

  ; Setup paging
  call pm_setup_page_tables

  ; enter a endless loop. This instruction should never be reached
  jmp pm_endless_loop


; On physical devices this isn't required because the BIOS will
; pull the x number of blocks regardless of their content, however,
; if you are using QEMU and a raw image, it will strugle to Read
; the BIOS Disk Access Packet (DAP ) because the file finishes
; way earlier than the number of blocks requested.
times (Loader.File.NumberOfBlocks * 512) - ($ - $$) db 0
