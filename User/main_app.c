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
#include "sys_timer.h"
#include "analog.h"
#include "keypad.h"
#include "led.h"

#define DEFAULT_CHANNEL 0
#define ANALOG_SAMPLE_RATE_MS 25
#define ANALOG_RANGE 30
#define ANALOG_PITCH_MIDDLE 2090

typedef struct
{
	bool in_sustain;
	uint8_t channel;
	int16_t modulation_last;
	int16_t pitch_last;
	int16_t volume_last;
	int16_t expression_last;
	bool in_advanced;
} APP_STATE_T;

APP_STATE_T _state = {
	.in_sustain = false,
	.channel = DEFAULT_CHANNEL,
	.modulation_last = 30000,
	.pitch_last = 30000,
	.volume_last = 30000,
	.expression_last = 30000,
	.in_advanced = false,
};

void keypad_event_handler(KEYPAD_EVENT_T event, KEYPAD_KEY_T key)
{

}

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

void monitor_analog()
{
	static uint32_t analog_sample_tick = 0;
	static bool last_pitch_normal = false;

	// todo: collect samples and average if needed
	if(analog_sample_tick + ANALOG_SAMPLE_RATE_MS < get_ms())
	{
		int16_t modulation;
		int16_t pitch;
		int16_t volume;
		int16_t expression;

		// request analog values
		analog_get(&modulation, &pitch, &volume, &expression);

		// monitor modulation
		if((modulation < (_state.modulation_last - ANALOG_RANGE)) || (modulation > (_state.modulation_last + ANALOG_RANGE)))
		{
			_state.modulation_last = modulation;
			MIDI_DEVICE.modulation_wheel(_state.channel, modulation * 4);
		}

		// monitor pitch
		if((pitch < (ANALOG_PITCH_MIDDLE - ANALOG_RANGE)) || (pitch > (ANALOG_PITCH_MIDDLE + ANALOG_RANGE)))
		{
			MIDI_DEVICE.pitch_wheel(_state.channel, (pitch - 2048) * 4);
			last_pitch_normal = false;
		}
		else
		{
			if(last_pitch_normal == false)
			{
				MIDI_DEVICE.pitch_wheel(_state.channel, 0);
			}

			last_pitch_normal = true;
		}

		// monitor volume
		if((volume < (_state.volume_last - ANALOG_RANGE)) || (volume > (_state.volume_last + ANALOG_RANGE)))
		{
			_state.volume_last = volume;
			MIDI_DEVICE.volume(_state.channel, volume * 4);
		}

		// todo: monitor expression

		analog_sample_tick += ANALOG_SAMPLE_RATE_MS;
	}
}

void main_app()
{
	keyboard_register_callback(keyboard_event_handler);
	keypad_register_callback(keypad_event_handler);

	//led_set(LED_TYPE_ADVANCED, true);
	//led_set(LED_TYPE_OCTAVE_UP_A, true);
	//led_set(LED_TYPE_OCTAVE_UP_B, true);
	//led_set(LED_TYPE_OCTAVE_DOWN_A, true);
	//led_set(LED_TYPE_OCTAVE_DOWN_B, true);

	// can you say "super loop"?
	while(1)
	{
		keyboard_task();
		MIDI_DEVICE.task();
		monitor_input();
		monitor_analog();
		keypad_task();
	}
}
