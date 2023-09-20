/*
 * keyboard.h
 *
 *  Created on: Sep 18, 2023
 *      Author: matt
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <stdint.h>
#include <stdbool.h>

#define KEYBOARD_TOTAL_KEYS 88
#define KEYBOARD_NOTES_PER_OCTAVE 12
#define KEYBOARD_START_NOTE 21

typedef enum
{
	KEYBOARD_EVENT_PRESS,
	KEYBOARD_EVENT_RELEASE,
} KEYBOARD_EVENT_T;

typedef void (*keyboard_event_callback)(KEYBOARD_EVENT_T event, uint8_t note, int16_t velocity);

void keyboard_init();
void keyboard_task();
void keyboard_register_callback(keyboard_event_callback callback);

#endif /* KEYBOARD_H_ */
