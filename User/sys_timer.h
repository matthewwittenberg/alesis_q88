/*
 * timer.h
 *
 *  Created on: Sep 18, 2023
 *      Author: matt
 */

#ifndef SYS_TIMER_H_
#define SYS_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

void sys_timer_init();
uint32_t get_ms();
void wait_ms(uint32_t ms);

#endif /* SYS_TIMER_H_ */
