#include "kernel/compiler/freestanding.h"
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
//  int b = 2;
//  memcpy(&b, &a, sizeof(int));
//
//  printk("a: %d\n", a);
//  printk("a: %d\nbut a is also: %d\n", a,a);
//  printk("Value of b is: %d\n", b);
//
//  // test row reset;
//  for(int i=0; i < 25; i++){
//	  printk("i: %d\n", i);
//  }
//
//  // test soft wrap
//  printk("abcdefghijklmnopqrstuvxzabcdefghijklmnopqrstuvxzabcdefghijklmnopqrstuvxzabcdefghijklmnopqrstuvxz\n");
//
//
//  for(int i=0; i < 5; i++){
//  	  printk("i: %d\n", i);
//    }
//  printk("Test negative numbers: %d", -2147483647);

  a = 0xff;
  printk("hex a: 0x%x\n", 4294967295);
  printk("uint a: %u\n", 4294967295);
  printk("octal a: %o\n", 511);
  printk("char a: %c\n", 'p');
  printk("My name is: %s\n", "AlmeidaOS");
  printk("My name is: %s\n", "");
  printk("My name is: %s\n", "AlmeidaOS");

  return 0;
}

/*
  System V AMD64 Calling Convention

  RDI, RSI, RDX, RCX, R8, R9 -> Others are on the stack
  RAX -> Return values
  RAX, RCX, RDX, RSI, RDI,R8, R9, R10, R11 ->  Caller Saved Registers
  RBX, RBP, R12, R13, R14, R15 ->  Calle Saved Registers
*/
