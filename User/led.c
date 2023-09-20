/*
 * led.c
 *
 *  Created on: Sep 20, 2023
 *      Author: matt
 */

#include "led.h"
#include "NUC100Series.h"

#define LED_ADVANCED PA4
#define LED_ADVANCED_PORT PA
#define LED_ADVANCED_PIN 4

#define LED_OCTAVE_UP_A PE5
#define LED_OCTAVE_UP_A_PORT PE
#define LED_OCTAVE_UP_A_PIN 5

#define LED_OCTAVE_UP_B PA5
#define LED_OCTAVE_UP_B_PORT PA
#define LED_OCTAVE_UP_B_PIN 5

#define LED_OCTAVE_DOWN_A PA6
#define LED_OCTAVE_DOWN_A_PORT PA
#define LED_OCTAVE_DOWN_A_PIN 6

#define LED_OCTAVE_DOWN_B PA7
#define LED_OCTAVE_DOWN_B_PORT PA
#define LED_OCTAVE_DOWN_B_PIN 7

void led_init()
{
	GPIO_SetMode(LED_ADVANCED_PORT, 1 << LED_ADVANCED_PIN, GPIO_PMD_OUTPUT);
	GPIO_SetMode(LED_OCTAVE_UP_A_PORT, 1 << LED_OCTAVE_UP_A_PIN, GPIO_PMD_OUTPUT);
	GPIO_SetMode(LED_OCTAVE_UP_B_PORT, 1 << LED_OCTAVE_UP_B_PIN, GPIO_PMD_OUTPUT);
	GPIO_SetMode(LED_OCTAVE_DOWN_A_PORT, 1 << LED_OCTAVE_DOWN_A_PIN, GPIO_PMD_OUTPUT);
	GPIO_SetMode(LED_OCTAVE_DOWN_B_PORT, 1 << LED_OCTAVE_DOWN_B_PIN, GPIO_PMD_OUTPUT);

	LED_ADVANCED = 1;
	LED_OCTAVE_UP_A = 1;
	LED_OCTAVE_UP_B = 1;
	LED_OCTAVE_DOWN_A = 1;
	LED_OCTAVE_DOWN_B = 1;
}

void led_set(LED_TYPE_T led, bool enable)
{
	switch(led)
	{
	case LED_TYPE_ADVANCED:
		LED_ADVANCED = enable ? 0 : 1;
		break;
	case LED_TYPE_OCTAVE_UP_A:
		LED_OCTAVE_UP_A = enable ? 0 : 1;
		break;
	case LED_TYPE_OCTAVE_UP_B:
		LED_OCTAVE_UP_B = enable ? 0 : 1;
		break;
	case LED_TYPE_OCTAVE_DOWN_A:
		LED_OCTAVE_DOWN_A = enable ? 0 : 1;
		break;
	case LED_TYPE_OCTAVE_DOWN_B:
		LED_OCTAVE_DOWN_B = enable ? 0 : 1;
		break;
	}
}
