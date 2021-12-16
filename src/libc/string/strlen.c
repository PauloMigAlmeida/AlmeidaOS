/*
 * strlen.c
 *
 *  Created on: 16/12/2021
 *      Author: Paulo Almeida
 */

#include "libc/string.h"

size_t strlen(const char *buf) {
    /* we don't count NUL-terminator */
    size_t len = 0;
    for (; *(buf + len) != '\0'; len++)
        /* nothing */ ;
    return len;
}
