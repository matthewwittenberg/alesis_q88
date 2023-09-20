/*
 * led.h
 *
 *  Created on: Sep 20, 2023
 *      Author: matt
 */

#ifndef LED_H_
#define LED_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	LED_TYPE_ADVANCED,
	LED_TYPE_OCTAVE_UP_A,
	LED_TYPE_OCTAVE_UP_B,
	LED_TYPE_OCTAVE_DOWN_A,
	LED_TYPE_OCTAVE_DOWN_B,
} LED_TYPE_T;

void led_init();
void led_set(LED_TYPE_T led, bool enable);

#endif /* LED_H_ */
