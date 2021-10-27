# Lessons learned

The objective of this file is to document things I came across which took me a
while to make a decision about and most likely I would forget it if I hadn't
written them here.

It might be good subjects for a blog post some time in the future:

### size_t - (20/06/2021)
typedefs that end with `*_t` are POSIX reserved so we should not try to create
one with this nomenclature.

`size_t` is a special case because it belong to both ISO C and POSIX.

Some old C code still defines `size_t` like the Linux Kernel (`include/linux/types.h`)
for historical reasons. The history is a bit unclear as `size_t` typedef was
introduced to linux even before they moved to git (2.6.\*), so I couldn't really
put my finger on *why* someone would do that and still keep it after so many
years.

The takeaway here is that `size_t` can be obtained in most compilers by
including the `<stddef.h>` header. Yet to be confirmed, but everything leads me
to believe that this won't after GCC `-ffreestanding` goals.

### GCC Extend Assembly - (20/06/2021)

Output Syntax: `[ [``asmSymbolicName``] ] ``constraint`` (``cvariablename``)`

Prefix:

- Output constraints must begin with either `=` (a variable overwriting an
  existing value) or `+` (when reading and writing).
- When using =, do not assume the location contains the existing value on entry
  to the asm, except when the operand is tied to an input;

Constraints:

- After the prefix, there must be one or more additional constraints that
  describe where the value resides. Common constraints include `r` for register
  and `m` for memory. When you list more than one possible location
  (for example, `"=rm"`), the compiler chooses the most efficient one based on
  the current context. If you list as many alternates as the asm statement
  allows, you permit the optimisers to produce the best possible code.

Cvariablename:

- Output operand expressions must be `lvalues`
- Operands using the `+` constraint modifier count as two operands (that is,
  both as input and output) towards the total maximum of 30 operands per asm
  statement.
- Use the `&` constraint modifier on all output operands that must not overlap an
  input

Input Syntax: `[ [``asmSymbolicName``] ] ``constraint`` (``cexpression``)`

- Input constraint strings may not begin with either `=` or `+`.
- Warning: Do not modify the contents of input-only operands (except for inputs
  tied to outputs). The compiler assumes that on exit from the asm statement
  these operands contain the same values as they had before executing the
  statement. It is not possible to use clobbers to inform the compiler that the
  values in these inputs are changing. One common work-around is to tie the
  changing input variable to an output variable that never gets used.

In this example using the fictitious combine instruction, the constraint "0" for
 input operand 1 says that it must occupy the same location as output operand 0.
  Only input operands may use numbers in constraints, and they must each refer
  to an output operand. Only a number (or the symbolic assembler name) in the
  constraint can guarantee that one operand is in the same place as another.
  The mere fact that foo is the value of both operands is not enough to guarantee
   that they are in the same place in the generated assembler code.

```C
asm ("combine %2, %0"
   : "=r" (foo)
   : "0" (foo), "g" (bar));
```

### QEMU+GDB Debugging for C files - (20/06/2021)

Given the fact that the kernel file is loaded dynamically, for some reason it gets 
lost when it comes to debug symbols defined in ELF sections. As a result, `layout src` 
doesn't work.

To workaround the problem, I tweaked the kernel_dir Makefile to generate 2 files:

- `kernel` (ELF file with no debug info in it) which is slighty slimmer
- `kernel.debug` (ELF file with only the debug info in it).

To get it working, here goes the steps to follow:

(1) Set the breakpoint to memory address in which a C code is being mapped to.

```Shell
$ readelf --syms build/kernel/kernel

Symbol table '.symtab' contains 24 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS start.asm
     2: 0000000000000005     0 NOTYPE  LOCAL  DEFAULT  ABS Loader.File.NumberOfBlock
     3: 0000000000000064     0 NOTYPE  LOCAL  DEFAULT  ABS Kernel.File.NumberOfBlock
     4: 000000000000000d     0 NOTYPE  LOCAL  DEFAULT  ABS CR
     5: 000000000000000a     0 NOTYPE  LOCAL  DEFAULT  ABS LF
     6: 00000000002010a0     0 NOTYPE  LOCAL  DEFAULT    3 LOG_PREFIX
     7: 0000000000007c00     0 NOTYPE  LOCAL  DEFAULT  ABS MBR.Mem.Stack.Top
     8: 0000000000007e00     0 NOTYPE  LOCAL  DEFAULT  ABS Loader.Mem.Stack.Top
     9: 0000000000010000     0 NOTYPE  LOCAL  DEFAULT  ABS Paging.Start.Address
    10: 0000000000010000     0 NOTYPE  LOCAL  DEFAULT  ABS Mem.PML4.Address
    11: 0000000000011000     0 NOTYPE  LOCAL  DEFAULT  ABS Mem.PDPE.Address
    12: 0000000000012000     0 NOTYPE  LOCAL  DEFAULT  ABS Mem.PDE.Address
    13: 0000000000013000     0 NOTYPE  LOCAL  DEFAULT  ABS Mem.PTE.Address
    14: 0000000000018000     0 NOTYPE  LOCAL  DEFAULT  ABS Paging.End.Address
    15: 0000000000008800     0 NOTYPE  LOCAL  DEFAULT  ABS Loader.Kernel.Start.Addre
    16: 0000000000200000     0 NOTYPE  LOCAL  DEFAULT  ABS Kernel.New.Start.Address
    17: 0000000000001000     0 NOTYPE  LOCAL  DEFAULT  ABS Kernel.New.ELFTextHeader.
    18: 0000000000201010     0 NOTYPE  LOCAL  DEFAULT    1 kernel_start.endless_loop
    19: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS main.c
    20: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS string.c
    21: 000000000020103d    87 FUNC    GLOBAL DEFAULT    2 memcpy
    22: 0000000000201014    41 FUNC    GLOBAL DEFAULT    2 kmain
    23: 0000000000201000     0 NOTYPE  GLOBAL DEFAULT    1 kernel_start
```

(2) Initiate GDB and QEMU in debug mode

```Shell
make qemu-debug
make gdb-debug
```

(3) Set a breakpoint to the desired function/line. (I will use kmain in this example)

```Shell
(gdb) br *0x201014
Breakpoint 2 at 0x201014

(gdb) c
Continuing.
```

(4) Add symbol file and correct source path to the GDB session

```Shell
(gdb) layout src
(gdb) set directories src/kernel/

(gdb) symbol-file build/kernel/kernel.debug 
Reading symbols from build/kernel/kernel.debug...

```

### QEMU Dump Memory snapshot to a file - (20/06/2021)

I came across a situation in which I needed to check the entire memory state. We can achieve this in a few different ways:

**GDB native way:**

```Shell
# x -> Examine, 50 -> number of results, 0x201014 -> start address
(gdb) x/50 0x201014
0x201014:	0xe5894855	0x10ec8348	0xf845c748	0x000b8000
0x201024:	0xf8458b48	0x485000c6	0x48f8458b	0xc601c083
0x201034:	0x00b80a00	0xc9000000	0x894855c3	0xec8348e5
0x201044:	0x7d894838	0x758948d8	0x558948d0	0x458b48c8
0x201054:	0xe8c148c8	0xc1894803	0xc8458b48	0x4907e083
0x201064:	0x8b48c089	0x8b48d855	0x8948d045	0xc68948d7
0x201074:	0x49a548f3	0xa4f3c889	0x48f08948	0x8948fa89
0x201084:	0x8948f84d	0x8948f055	0x8b48e845	0xc3c9d845
0x201094:	0x00000000	0x00000000	0x00000000	0x6d6c415b
0x2010a4:	0x61646965	0x205d534f	0x00203a3a	0x00000000
0x2010b4:	0x00000000	0x00000000	0x00000000	0x00000000
0x2010c4:	0x00000000	0x00000001	0xfff10000	0x00000005
0x2010d4:	0x00000000	0x00000000

```

More at: https://sourceware.org/gdb/current/onlinedocs/gdb/Memory.html

**QEMU:**

This can be achieved using the QEMU machine protocol described [here](https://wiki.ubuntu.com/DebuggingKernelWithQEMU)

This consists of initiating qemu with `-qmp tcp:localhost:4444,server,nowait` and sending some
commands via telnet. I've already added that to the `qemu-debug` target of the Makefile.

On one terminal execute:

```Shell
make qemu-debug
```

When ready to do the snapshot, run on another terminal:

```Shell
telnet localhost 4444

{"execute": "qmp_capabilities"}
{"execute":"dump-guest-memory","arguments":{"paging":false,"protocol":"file:/tmp/vmcore.img"}}
```

### QEMU finding offending instruction - (10/07/2021)

After I wrote the crash dump / coredump (whatever way it's supposed to be called). I needed to
find out which instructions brought the system down. QEMU can log the $pc value (which is basically 
the RIP content) like shown below

```Shell
check_exception old: 0xffffffff new 0xd
     1: v=0d e=0082 i=0 cpl=0 IP=0008:0000000000201074 pc=0000000000201074 SP=0000:0000000000200ff0 env->regs[R_EAX]=0000000000000001
RAX=0000000000000001 RBX=0000000000000000 RCX=0000000000000019 RDX=00000000000003d5
RSI=0000000000000000 RDI=0000000000000006 RBP=0000000000200ff0 RSP=0000000000200ff0
R8 =0000000000000006 R9 =0000000000000000 R10=0000000000202e6c R11=0000000000000000
R12=0000000000000000 R13=0000000000000000 R14=0000000000000000 R15=0000000000000000
RIP=0000000000201074 RFL=00000203 [------C] CPL=0 II=0 A20=1 SMM=0 HLT=0
ES =0010 0000000000000000 00000000 00009300 DPL=0 DS   [-WA]
CS =0008 0000000000000000 00000000 00209800 DPL=0 CS64 [---]
SS =0000 0000000000000000 00000000 00000000
DS =0010 0000000000000000 00000000 00009300 DPL=0 DS   [-WA]
FS =0010 0000000000000000 00000000 00009300 DPL=0 DS   [-WA]
GS =0010 0000000000000000 00000000 00009300 DPL=0 DS   [-WA]
LDT=0000 0000000000000000 0000ffff 00008200 DPL=0 LDT
TR =0000 0000000000000000 0000ffff 00008b00 DPL=0 TSS64-busy
GDT=     0000000000007e31 00000017
IDT=     0000000000205010 000000ff
CR0=80000011 CR2=0000000000000000 CR3=0000000000010000 CR4=00000020
DR0=0000000000000000 DR1=0000000000000000 DR2=0000000000000000 DR3=0000000000000000 
DR6=00000000ffff0ff0 DR7=0000000000000400
CCS=0000000000000001 CCD=0000000000000001 CCO=EFLAGS  
EFER=0000000000000500

```
Now I know that whatever is at line `pc=0000000000201074` is what ultimately caused that to panic.

With that info I can execute:

```Shell
objdump -S --start-address=0x201074 build/kernel/kernel | awk '{print $0} $3~/retq?/{exit}'
```

Return looks like this:

```Shell
build/kernel/kernel:     file format elf64-x86-64

Disassembly of section .text:

0000000000201074 <kmain+0x34>:
  201074:	eb fe                	jmp    201074 <kmain+0x34>

0000000000201076 <vga_console_init>:
  201076:	55                   	push   %rbp
  201077:	48 89 e5             	mov    %rsp,%rbp
  20107a:	48 bf a0 37 20 00 00 	movabs $0x2037a0,%rdi
  201081:	00 00 00 
  201084:	48 b8 7c 2f 20 00 00 	movabs $0x202f7c,%rax
  20108b:	00 00 00 
  20108e:	ff d0                	callq  *%rax
  201090:	b8 00 00 00 00       	mov    $0x0,%eax
  201095:	48 ba 45 11 20 00 00 	movabs $0x201145,%rdx
  20109c:	00 00 00 
  20109f:	ff d2                	callq  *%rdx
  2010a1:	90                   	nop
  2010a2:	5d                   	pop    %rbp
  2010a3:	c3                   	retq   

```

### Choosing sorting algorithm for e820 memory entries - (29/07/2021)

BIOS 0xe820 returns an unordered list of entries containing the memory map information.
Each entry is 20 bytes long (I'm ignoring ACPI values TBH).
I reserved a fraction of the low memory address to hold a few hundreded entries.

Because this is unordered, I neeed to sort them in order to find entries that can be
squashed together. Here is where the problem begins:

Since at this point we have no dinamic paging / dynamic memory allocation available I had
to do everything using in-place sorting algorithms - which greatly reduces the breath of
options significantly. 

I initially thought of using MergeSort (modified to work on Linked-List to keep O(1) auxiliary
space).

**Pros**: 

- in the worst case, it doesn't degrade to O(n^2) like quicksort (even though this can be 
	mostly statistically avoided) 

**Cons**:

- the fact that linked list nodes would require a pointer would end up increasing the size of
each entry significantly as each entry is just 20-bytes long.

Because of that, I implemented a simple version of the qsort algorithm based on the Algorithm-24-series
found on Safari books.


### GDB print struct array - (22/08/2021)

@(number) determines how many times you the operation to repeat

```
 p (buddy_slot_t)*0x100000@511
```

### Testing TSS - (27/10/2021)

In OS development, the easiest thing is to get to 'analysis paralysis' state. That's what prevents progress 
given that can always start with something more archaic and evolve it later.

To test interrupts in Ring 3, one needs to choose among the many different ways to jump to ring 3. Most likely,
one wants to use syscall and sysret (because that's what it wants to implement that for his OS) but that implies
that a lot more must be implemented before this could be tested.

To test it quickly, we can afford the luxury of 'not doing it right' and just test that already. Here is what I did:

On `src/kernel/start.asm`:

```assembly
; Export to C 
global go_to_ring3

go_to_ring3:
    ; SS selector + RPL = 3 (Ring 3)
    push 0x20 | 3
    ; RSP address (in this test I'm using the same stack for kernel)
    mov rax, Kernel.New.Start.VirtualAddress
    push rax
    ; RFLAGS (inclusing interrupt flag = enabled)
    push 0x202
    ; CS selector + RPL = 3 (Ring 3)
    push 0x18 | 3
    ; RIP (place in which the CPU should start processing)
    mov rax, user_entry
    push rax
    iretq

user_entry:
    ; Check if we are really in Ring 3) 
    mov ax, cs
    and ax, 3
    cmp al, 11b
    jne user_end

    ; Print a character in the vga to confirm
    mov rax, 0xffff8000000b8000
    mov byte[rax], 'U'
    mov rax, 0xffff8000000b8001
    mov byte[rax], 0xE

user_end:
    jmp user_end

``` 

On `src/kernel/main.c`:

```C
extern void go_to_ring3(void);

void kmain(void) {
	.....
    /* Unleash all possible problems in the world */
    enable_interrupts();

    /* enabled IRQs */
    spurious_irq_enable();
    keyboard_enable();
    pit_enable();

    /* Test jumping to Ring 3 */
    go_to_ring3();
}
	
```
