/*
 * pid.h
 *
 *  Created on: 18/12/2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_TASK_PID_H_
#define INCLUDE_KERNEL_TASK_PID_H_

#include "kernel/compiler/freestanding.h"
#include "kernel/sys/types.h"

pid_t find_free_pid(void);

#endif /* INCLUDE_KERNEL_TASK_PID_H_ */
