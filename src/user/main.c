/*
 * main.c
 *
 *  Created on: 4/12/2021
 *      Author: Paulo Almeida
 */

#include "libc/unistd.h"
#include "libc/string.h"
#include "libc/stdlib.h"

void umain(void) {
    write("Hello from user space", 22);

    /* get current time */
    char time_str[50];
    memset(time_str, '\0', sizeof(time_str));
    time_t now = time();
    ltoa(now, time_str, 10);
    write(time_str, strlen(time_str) + 1);

    /* get current process id */
    pid_t curr_pid = getpid();
    char pid_c[2];
    pid_c[0] = curr_pid + '0';
    pid_c[1] = '\0';
    write(pid_c, 2);
}

