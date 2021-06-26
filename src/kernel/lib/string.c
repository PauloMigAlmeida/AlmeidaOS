#include "kernel/lib/string.h"

/*
asm ( assembler template
    : output operands                   (optional)
    : input operands                    (optional)
    : clobbered registers list          (optional)
    );

  Notes for myself:

  movsq in linux uses the fast-string:
    - https://gist.github.com/sheepdestroyer/5f439475ad8c660cb455982cba1222c4
    - Intel 64 Manual 1: Section 7.3.9.3 Fast-String Operation

    TODO: enable that via C...I will do it via assembly for now

    The MOVS, MOVSB, MOVSW, and MOVSD instructions can be preceded by the REP prefix (see “REP/REPE/REPZ
/REPNE/REPNZ—Repeat String Operation Prefix” for a description of the REP prefix) for block moves of ECX bytes,
words, or doublewords.

*/


void* memcpy(void* dst, void* src, size_t size) {

	/*

objdump -D -M intel  build/kernel/lib/string.o | awk -v RS= '/^[[:xdigit:]]+ <memcpy>/'
0000000000000000 <memcpy>:
   0:	55                   	push   rbp
   1:	48 89 e5             	mov    rbp,rsp
   4:	48 83 ec 18          	sub    rsp,0x18

   8:	48 89 7d f8          	mov    QWORD PTR [rbp-0x8],rdi -> dst
   c:	48 89 75 f0          	mov    QWORD PTR [rbp-0x10],rsi -> src
  10:	48 89 55 e8          	mov    QWORD PTR [rbp-0x18],rdx -> size
  14:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  18:	48 c1 e8 03          	shr    rax,0x3					-> Size >> 3
  1c:	48 8b 55 e8          	mov    rdx,QWORD PTR [rbp-0x18]	-> size
  20:	48 89 d6             	mov    rsi,rdx					-> size to rsi
  23:	83 e6 07             	and    esi,0x7					-> remainder
  26:	48 89 f2             	mov    rdx,rsi					-> remainder to rdx
  29:	48 89 c1             	mov    rcx,rax					-> Size >> 3 to rcx
  2c:	f3 48 a5             	rep movs QWORD PTR es:[rdi],QWORD PTR ds:[rsi]
  2f:	48 89 d1             	mov    rcx,rdx
  32:	f3 a4                	rep movs BYTE PTR es:[rdi],BYTE PTR ds:[rsi]
  34:	48 89 f0             	mov    rax,rsi
  37:	48 89 fa             	mov    rdx,rdi
  3a:	48 89 55 f8          	mov    QWORD PTR [rbp-0x8],rdx
  3e:	48 89 45 f0          	mov    QWORD PTR [rbp-0x10],rax
  42:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]
  46:	c9                   	leave
  47:	c3                   	ret
	 */
	asm volatile(
			"rep movsq \n\t"
			"movq rcx, %[remainder] \n\t"
			"rep movsb \n\t"
			: "=&D" (dst), "=&S" (src) //output operands
			: "c"(size >> 3), [remainder] "g" (size & 7) //input operands
			: "memory" // clobbered registers list
	);

	/*
	 *
 0000000000000000 <memcpy>:
   0:	55                   	push   rbp
   1:	48 89 e5             	mov    rbp,rsp
   4:	48 83 ec 38          	sub    rsp,0x38
   8:	48 89 7d d8          	mov    QWORD PTR [rbp-0x28],rdi
   c:	48 89 75 d0          	mov    QWORD PTR [rbp-0x30],rsi
  10:	48 89 55 c8          	mov    QWORD PTR [rbp-0x38],rdx
  14:	48 8b 45 c8          	mov    rax,QWORD PTR [rbp-0x38]
  18:	48 c1 e8 03          	shr    rax,0x3
  1c:	48 89 c1             	mov    rcx,rax
  1f:	48 8b 45 c8          	mov    rax,QWORD PTR [rbp-0x38]
  23:	83 e0 07             	and    eax,0x7
  26:	49 89 c0             	mov    r8,rax
  29:	48 8b 55 d8          	mov    rdx,QWORD PTR [rbp-0x28]
  2d:	48 8b 45 d0          	mov    rax,QWORD PTR [rbp-0x30]
  31:	48 89 d7             	mov    rdi,rdx
  34:	48 89 c6             	mov    rsi,rax
  37:	f3 48 a5             	rep movs QWORD PTR es:[rdi],QWORD PTR ds:[rsi]
  3a:	49 89 c8             	mov    r8,rcx
  3d:	f3 a4                	rep movs BYTE PTR es:[rdi],BYTE PTR ds:[rsi]
  3f:	48 89 f0             	mov    rax,rsi
  42:	48 89 fa             	mov    rdx,rdi
  45:	48 89 4d f8          	mov    QWORD PTR [rbp-0x8],rcx
  49:	48 89 55 f0          	mov    QWORD PTR [rbp-0x10],rdx
  4d:	48 89 45 e8          	mov    QWORD PTR [rbp-0x18],rax
  51:	48 8b 45 d8          	mov    rax,QWORD PTR [rbp-0x28]
  55:	c9                   	leave
  56:	c3                   	ret

	 */
//  long d0, d1, d2;
//  asm volatile(
//    "rep ; movsq\n\t" // Move qword from address (R|E)SI to (R|E)DI.
//    "movq %4,%%rcx\n\t"
//    "rep ; movsb\n\t"
//    : "=&c" (d0), "=&D" (d1), "=&S" (d2)
//    : "0" (size >> 3), "g" (size & 7), "1" (dst), "2" (src)
//    : "memory");

  return dst;
}

void* memset(void* buf, char value, size_t size) {
	/*
	 objdump -D -M intel  build/kernel/lib/string.o | awk -v RS= '/^[[:xdigit:]]+ <memset>/'
0000000000000048 <memset>:
  48:	55                   	push   rbp
  49:	48 89 e5             	mov    rbp,rsp
  4c:	48 83 ec 18          	sub    rsp,0x18
  50:	48 89 7d f8          	mov    QWORD PTR [rbp-0x8],rdi
  54:	89 f0                	mov    eax,esi
  56:	48 89 55 e8          	mov    QWORD PTR [rbp-0x18],rdx
  5a:	88 45 f4             	mov    BYTE PTR [rbp-0xc],al
  5d:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  61:	48 c1 e8 03          	shr    rax,0x3
  65:	48 89 c2             	mov    rdx,rax
  68:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  6c:	83 e0 07             	and    eax,0x7
  6f:	48 89 c6             	mov    rsi,rax
  72:	0f b6 45 f4          	movzx  eax,BYTE PTR [rbp-0xc]
  76:	48 89 d1             	mov    rcx,rdx
  79:	f3 48 ab             	rep stos QWORD PTR es:[rdi],rax
  7c:	48 89 f1             	mov    rcx,rsi
  7f:	f3 aa                	rep stos BYTE PTR es:[rdi],al
  81:	48 89 f8             	mov    rax,rdi
  84:	48 89 45 f8          	mov    QWORD PTR [rbp-0x8],rax
  88:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]
  8c:	c9                   	leave
  8d:	c3                   	ret
	 */
	asm volatile(
		"rep stosq \n\t"
		"movq rcx, %[remainder] \n\t"
		"rep stosb \n\t"
		: "=&D" (buf) //output operands
		: "a" (value), "c"(size >> 3), [remainder] "g" (size & 7) //input operands
		: "memory" // clobbered registers list
	);
	return buf;
}
