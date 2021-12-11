/*
 * write.c
 *
 *  Created on: 12/12/2021
 *      Author: Paulo Almeida
 */

#include "kernel/syscall/write.h"
#include "kernel/lib/printk.h"
#include "kernel/lib/string.h"
#include "kernel/mm/kmem.h"
#include "kernel/compiler/bug.h"

size_t sys_write(const char *string, size_t length){
    /* sanity checks */
    BUG_ON(length == 0 || string == NULL);

    /* copy from user space (I should probably turn this into a routine) */
    char* content = kmalloc(length, KMEM_DEFAULT);
    memcpy(content, string, length);

    printk_info(content);
    return length;
}

