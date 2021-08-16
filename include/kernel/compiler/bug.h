/*
 * bug.h
 *
 *  Created on: Aug 16, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_COMPILER_BUG_H_
#define INCLUDE_KERNEL_COMPILER_BUG_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/asm/generic.h"
#include "kernel/lib/printk.h"

/* halts the kernel when things that shuldn't happen do so  */
#define BUG_ON(cond)                                                          \
  do {                                                                        \
      if((cond)) {                                                            \
          printk_error("BUG_ON: file: %s, line: %u", __FILE__, __LINE__);     \
          fatal();                                                            \
      }                                                                       \
  } while (0)

#endif /* INCLUDE_KERNEL_COMPILER_BUG_H_ */
