/*
 * main_app.c
 *
 *  Created on: Sep 20, 2023
 *      Author: matt
 */


#include "main_app.h"
#include "keyboard.h"
#include "midi_device.h"

#define DEFAULT_CHANNEL 0

void keyboard_event_handler(KEYBOARD_EVENT_T event, uint8_t note, int16_t velocity)
{
	if(event == KEYBOARD_EVENT_PRESS)
	{
		MIDI_DEVICE.note_on(note, DEFAULT_CHANNEL, velocity);
	}
	else if(event == KEYBOARD_EVENT_RELEASE)
	{
		MIDI_DEVICE.note_on(note, DEFAULT_CHANNEL, 0);
	}
}

void main_app()
{
	keyboard_register_callback(keyboard_event_handler);

	// can you say "super loop"?
	while(1)
	{
		keyboard_task();
		MIDI_DEVICE.task();
	}
}
