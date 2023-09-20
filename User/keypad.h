/*
 * keypad.h
 *
 *  Created on: Sep 20, 2023
 *      Author: matt
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	KEYPAD_EVENT_PRESS,
	KEYPAD_EVENT_RELEASE,
} KEYPAD_EVENT_T;

typedef enum
{
	KEYPAD_KEY_ADVANCED,
	KEYPAD_KEY_OCTAVE_MINUS,
	KEYPAD_KEY_OCTAVE_PLUS,
} KEYPAD_KEY_T;

typedef void (*keypad_event_callback)(KEYPAD_EVENT_T event, KEYPAD_KEY_T key);

void keypad_init();
void keypad_task();
void keypad_register_callback(keypad_event_callback callback);

#endif
