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


  .bss : ALIGN(16)
  {
		*(.bss)
		*(COMMON)
  }

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
