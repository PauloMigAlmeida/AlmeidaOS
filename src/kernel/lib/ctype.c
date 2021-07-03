/*
 * ctype.c
 *
 *  Created on: Jul 4, 2021
 *      Author: parallels
 */

#include "kernel/lib/ctype.h"

bool isdigit(char c) {
	return (unsigned)c - '0' < 10;
}
