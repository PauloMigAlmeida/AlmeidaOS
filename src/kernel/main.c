#include <stddef.h>
#include "kernel/lib/string.h"

static int force_bss_content;
static char force_bss_content2;

int kmain(void) {
  //force_bss_content
  force_bss_content = 11;
  force_bss_content2 = 'o';

  // dummy test of the memcpy function
  int a = 10;
  int b = 8;
  memcpy(&b, &a, sizeof(int));

  char* video = (char*)0xb8000;
  video[0] = 'P';
  video[1] = 0x0a;

  return 0;
}

/*
  System V AMD64 Calling Convention

  RDI, RSI, RDX, RCX, R8, R9 -> Others are on the stack
  RAX -> Return values
  RAX, RCX, RDX, RSI, RDI,R8, R9, R10, R11 ->  Caller Saved Registers
  RBX, RBP, R12, R13, R14, R15 ->  Calle Saved Registers
*/
