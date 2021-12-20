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

    size_t counter = 0;
    while (1) {
        counter++;

        if (counter % 100000000 == 0) {
            /* get current time */
            time_t now = time();

            /* get current process id */
            pid_t curr_pid = getpid();

            /* print messge */
            char msg[50];
            memset(msg, '\0', sizeof(msg));
            memcpy(msg, "process ", 8);
            msg[8] = curr_pid + '0';
            msg[9] = ' ';

            ltoa(now, msg+10, 10);

            write(msg, strlen(msg) + 1);
        }
    }

}

