/*
 * keyboard.h
 *
 *  Created on: Jul 13, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_DEVICE_KEYBOARD_H_
#define INCLUDE_KERNEL_DEVICE_KEYBOARD_H_

void keyboard_init(void);
void keyboard_handle_irq(void);

#endif /* INCLUDE_KERNEL_DEVICE_KEYBOARD_H_ */
