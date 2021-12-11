/*
 * unistd.h
 *
 *  Created on: 11/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_LIBC_UNISTD_H_
#define INCLUDE_LIBC_UNISTD_H_

#include "libc/compiler/freestanding.h"

long write(const char* string, size_t length);


#endif /* INCLUDE_LIBC_UNISTD_H_ */
