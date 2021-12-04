/*
 * main.c
 *
 *  Created on: 4/12/2021
 *      Author: Paulo Almeida
 */

/* Freestanding headers
 * https://wiki.osdev.org/C_Library#Freestanding_and_Hosted
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>

static void write_string(const char *string)
{
    volatile char *video = (volatile char*) 0xb8000;
    while (*string != 0)
    {
        *video++ = *string++;
        *video++ = 2; //green
    }
}

void umain(void) {
    write_string("Hello from user space");
}

