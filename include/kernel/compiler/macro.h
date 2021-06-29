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
#define __force_inline       inline __attribute__((always_inline))

#endif /* INCLUDE_KERNEL_COMPILER_MACRO_H_ */
