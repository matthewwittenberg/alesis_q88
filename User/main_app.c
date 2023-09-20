/*
 * main_app.c
 *
 *  Created on: Sep 20, 2023
 *      Author: matt
 */


#include "main_app.h"
#include "keyboard.h"
#include "midi_device.h"
#include "input.h"

#define DEFAULT_CHANNEL 0

typedef struct
{
	bool in_sustain;
	uint8_t channel;
} APP_STATE_T;

APP_STATE_T _state = {
	.in_sustain = false,
	.channel = DEFAULT_CHANNEL
};

void keyboard_event_handler(KEYBOARD_EVENT_T event, uint8_t note, int16_t velocity)
{
	if(event == KEYBOARD_EVENT_PRESS)
	{
		MIDI_DEVICE.note_on(note, _state.channel, velocity);
	}
	else if(event == KEYBOARD_EVENT_RELEASE)
	{
		MIDI_DEVICE.note_on(note, _state.channel, 0);
	}
}

void monitor_input()
{
	if(input_get_sustain())
	{
		if(_state.in_sustain == false)
		{
			MIDI_DEVICE.sustain(_state.channel, true);
			_state.in_sustain = true;
		}
	}
	else
	{
		if(_state.in_sustain)
		{
			MIDI_DEVICE.sustain(_state.channel, false);
			_state.in_sustain = false;
		}
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

		monitor_input();
	}
}
