OUTPUT_FORMAT("elf64-x86-64")
ENTRY(user_entry)

SECTIONS
{  
  . = 0x41000;  

  .head.text : {
    *(.head.text)
  }

  .text : {
    *(.text)
  }

  . = ALIGN(16);
  .data : {
    *(.data)
  }


  .bss : ALIGN(4K) {
    _BSS_START = ABSOLUTE(.);
    *(.bss)
    *(COMMON)
  }
  
  _BSS_SIZE = ABSOLUTE(.) - _BSS_START;

  .rodata : {
    *(.rodata)
  }   

}
