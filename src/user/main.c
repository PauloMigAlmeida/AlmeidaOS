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

    pid_t curr_pid = getpid();
    char pid_c[2];
    pid_c[0] = curr_pid + '0';
    pid_c[1] = '\0';
    write(pid_c, 2);
}

