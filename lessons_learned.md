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
