/*
 * qsort.h
 *
 *  Created on: Jul 30, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_LIB_QSORT_H_
#define INCLUDE_KERNEL_LIB_QSORT_H_

#include "kernel/compiler/freestanding.h"

typedef int (*qsort_cmp_fun)(const void *, const void *);

void qsort(void *base, size_t num_elems, size_t item_width, qsort_cmp_fun cmp);

#endif /* INCLUDE_KERNEL_LIB_QSORT_H_ */
