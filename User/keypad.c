/*
 * keypad.c
 *
 *  Created on: Sep 20, 2023
 *      Author: matt
 */

#include "keypad.h"
#include "NUC100Series.h"

static keypad_event_callback _keypad_callback = NULL;

#define KEYPAD_ADVANCED PA9
#define KEYPAD_ADVANCED_PORT PA
#define KEYPAD_ADVANCED_PIN 9

void keypad_init()
{
	GPIO_SetMode(KEYPAD_ADVANCED_PORT, 1 << KEYPAD_ADVANCED_PIN, GPIO_PMD_INPUT);
}

void keypad_task()
{
	if(KEYPAD_ADVANCED)
	{
		_keypad_callback(KEYPAD_EVENT_PRESS, KEYPAD_KEY_ADVANCED);
	}
}

void keypad_register_callback(keypad_event_callback callback)
{
	_keypad_callback = callback;
}
