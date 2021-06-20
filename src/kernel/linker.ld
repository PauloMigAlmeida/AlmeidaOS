OUTPUT_FORMAT("elf64-x86-64")
ENTRY(kernel_start)

SECTIONS
{
  /* 
  	Kernel ELF file loads at 0x200000 but the text section starts with an 
  	offset 0x1000. Hence, 0x201000
  */
  . = 0x201000;

  /*
    The order of the files specified to the gnu linker changes the content of
    the final executable.

    For instance, imagine that you have 2 files start.o and main.o, but start.o
    during its execution call a method kmain for main.o then you must call the
    gnu linker with start.o as the first file. Otherwise, it will call kmain
    from main.o and when it returns, the value from the stack (ss) will be
    garbage resulting in your OS failing to boot.

    Linux gets around this problem by creating a section name called "head.text"
    and ensuring that this comes first in the linker file. There is nothing on
    the ELF specification that says that we can't use it that way as long as
    we don't mess with the Special sections

    https://refspecs.linuxfoundation.org/elf/elf.pdf - Page 46
  */

  .head.text : {
    *(.head.text)
  }

  .text : {
    *(.text)
  }

  /*
    TODO: Check which sections that should be aligned...
  */
  . = ALIGN(16);
  .data : {
    *(.data)
  }


  .bss : ALIGN(4K)
  {
		_BSS_START = ABSOLUTE(.);
		*(.bss)
		*(COMMON)
  }
  _BSS_SIZE = ABSOLUTE(.) - _BSS_START;


  .rodata : {
    *(.rodata)
  }

/*
  /DISCARD/ : {
  	*(.debug_info)
    *(.debug_abbrev)
    *(.debug_aranges)
    *(.debug_line)
    *(.debug_str)
    *(.debug_frame)
	}
*/
}
