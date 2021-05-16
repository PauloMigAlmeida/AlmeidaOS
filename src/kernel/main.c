#include <stddef.h>

void kmain(void)
{
  char* video = (char*)0xb8000;
  video[0] = 'P';
  video[1] = 0x0a;

}
