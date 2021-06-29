#include <stddef.h>

#include "kernel/video/vga_console.h"
#include "kernel/lib/string.h"
#include "kernel/lib/printk.h"

static int force_bss_content;
static char force_bss_content2;

int kmain(void) {
  vga_console_init();

  //force_bss_content
  force_bss_content = 11;
  force_bss_content2 = 'o';

  // dummy test of the memcpy function
  int a = 1;
  int b = 2;
  memcpy(&b, &a, sizeof(int));

  printk("a: %d\n", a);
  printk("Value of b is: %d\n", b);

  // test row reset;
  for(int i=0; i < 25; i++){
	  printk("i: %d\n", i);
  }

  // test soft wrap
  //TODO: change printk's implementation so it can deal with soft-wraps edge cases
//  printk("abcdefghijklmnopqrstuvxzabcdefghijklmnopqrstuvxzabcdefghijklmnopqrstuvxzabcdefghijklmnopqrstuvxz\n");

//  printk("Test negative numbers: %d", -15);

  return 0;
}

/*
  System V AMD64 Calling Convention

  RDI, RSI, RDX, RCX, R8, R9 -> Others are on the stack
  RAX -> Return values
  RAX, RCX, RDX, RSI, RDI,R8, R9, R10, R11 ->  Caller Saved Registers
  RBX, RBP, R12, R13, R14, R15 ->  Calle Saved Registers
*/
