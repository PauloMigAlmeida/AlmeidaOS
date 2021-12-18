/*
 * pid.c
 *
 * Notes to myself:
 *  - right now I'm focusing on the scheduling part so I won't bother trying to come
 *    up with a better solution than O(n). When I'm done I may come back to this
 *    just for fun.
 *
 *  Created on: 18/12/2021
 *      Author: Paulo Almeida
 */

#include "kernel/task/pid.h"
#include "kernel/compiler/bug.h"

#define PID_MAX     1024

/* BSS section reset is meant to initialise this array to false */
static bool pid_map[PID_MAX];

pid_t find_free_pid(void) {
    pid_t ret = -1;

    for(size_t i = 1; i < PID_MAX; i++){
        if(!pid_map[i]){
            pid_map[i] = true;
            ret = i;
            break;
        }
    }

    BUG_ON(ret == -1);
    return ret;
}

