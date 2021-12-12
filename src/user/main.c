/*
 * main.c
 *
 *  Created on: 4/12/2021
 *      Author: Paulo Almeida
 */

#include "libc/compiler/freestanding.h"
#include "libc/unistd.h"

void umain(void) {
    write("Hello from user space", 22);
    write("Fuck yeah!", 11);
}

