OUTPUT_FORMAT("elf64-x86-64")
ENTRY(kmain)

SECTIONS
{
  . = 0x200000;

  .text : {
    *(.text)
  }

  .rodata : {
    *(.rodata)
  }

  /*
    Check which sections that should be aligned...
  */
  . = ALIGN(16);
  .data : {
    *(.data)
  }

  .bss : {
    *(.bss)
  }
}
