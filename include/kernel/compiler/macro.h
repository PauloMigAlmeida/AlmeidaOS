/*
 * macro.h
 *
 *  Created on: Jun 29, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_COMPILER_MACRO_H_
#define INCLUDE_KERNEL_COMPILER_MACRO_H_

/*
 * tells gcc to inline functions even when not optimizing
 * more: https://gcc.gnu.org/onlinedocs/gcc/Inline.html
 */
#define __force_inline		inline __attribute__((always_inline))

/*
 * This attribute, attached to an enum, struct, or union type definition,
 * specified that the minimum required memory be used to represent the type.
 * more: https://gcc.gnu.org/onlinedocs/gcc-3.3/gcc/Type-Attributes.html
 */
#define __packed            __attribute__((__packed__))


/*
 * This attribute specifies a minimum alignment (in bytes) for variables of
 * the specified type.
 * More: https://gcc.gnu.org/onlinedocs/gcc-3.3/gcc/Type-Attributes.html
 */
#define __aligned(x)        __attribute__((aligned(x)))

/* calculate the length of the array - and avoid tendinitis ;) */
#define ARR_SIZE(arr)		sizeof(arr)/sizeof(arr[0])

/* Min value routine - widely used to truncate memcpy op for security reasons */
#define MIN(a, b)           ((a) < (b) ? (a) : (b))

/* Byte-wise swap two items of size SIZE. Credits: GNU libC*/
#define EXCH(a, b, size)                                                      \
  do {                                                                        \
      register size_t __size = (size);                                        \
      register char *__a = (a), *__b = (b);                                   \
      do {                                                                    \
          char __tmp = *__a;                                                  \
          *__a++ = *__b;                                                      \
          *__b++ = __tmp;                                                     \
        } while (--__size > 0);                                               \
    } while (0)


#endif /* INCLUDE_KERNEL_COMPILER_MACRO_H_ */
