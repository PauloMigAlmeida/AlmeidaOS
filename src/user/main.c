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
    /* greeting */
    char msg[100];
    memset(msg, '\0', sizeof(msg));

    size_t counter = 0;
    while (1) {
        counter++;

        if (counter % 1000000 == 0) {
            memset(msg, '\0', sizeof(msg));
            memcpy(msg, "process ", 8);

            /* get current process id */
            pid_t curr_pid = getpid();
            ltoa(curr_pid, msg + strlen(msg), 10);

            memcpy(msg + strlen(msg), " unix time: ", 12);

            /* get current time */
            time_t now = time();
            ltoa(now, msg + strlen(msg), 10);

            /* print messge */
            write(msg, strlen(msg) + 1);
        }
    }

}

