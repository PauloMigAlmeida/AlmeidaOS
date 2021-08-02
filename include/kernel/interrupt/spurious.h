/*
 * spurious.h
 *
 *  Created on: Aug 2, 2021
 *      Author: Paulo Almeida
 */

#ifndef INCLUDE_KERNEL_INTERRUPT_SPURIOUS_H_
#define INCLUDE_KERNEL_INTERRUPT_SPURIOUS_H_

void spurious_irq_enable(void);
void spurious_handle_irq(void);

#endif /* INCLUDE_KERNEL_INTERRUPT_SPURIOUS_H_ */
