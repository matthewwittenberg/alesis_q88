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
#include <string.h>
#include <stdlib.h>

#define DEFAULT_CHANNEL 0
#define ANALOG_SAMPLE_RATE_MS 25
#define ANALOG_RANGE 30
#define ANALOG_PITCH_MIDDLE 2090
#define ADVANCED_STATE_LED_BLINK_MS 500
#define MIDI_1_0_VELOCITY_DIVIDER (KEYBOARD_SLOWEST_VELOCITY / 127)
#define MIDI_2_0_VELOCITY_MULTIPLIER (65536 / KEYBOARD_SLOWEST_VELOCITY)
#define USE_MIDI_SENSE	// comment out to stop sense
#define MIDI_SENSE_RATE_MS 1000
#define USE_MIDI_NOTE_OFF	// comment out to use note on with velocity 0 instead

#define ADVANCED_NOTE_CANCEL 24
#define ADVANCED_NOTE_CHANNEL1 26
#define ADVANCED_NOTE_CHANNEL2 28
#define ADVANCED_NOTE_CHANNEL3 29
#define ADVANCED_NOTE_CHANNEL4 31
#define ADVANCED_NOTE_CHANNEL5 33
#define ADVANCED_NOTE_CHANNEL6 35
#define ADVANCED_NOTE_CHANNEL7 36
#define ADVANCED_NOTE_CHANNEL8 38
#define ADVANCED_NOTE_CHANNEL9 40
#define ADVANCED_NOTE_CHANNEL10 41
#define ADVANCED_NOTE_CHANNEL11 43
#define ADVANCED_NOTE_CHANNEL12 45
#define ADVANCED_NOTE_CHANNEL13 47
#define ADVANCED_NOTE_CHANNEL14 48
#define ADVANCED_NOTE_CHANNEL15 50
#define ADVANCED_NOTE_CHANNEL16 52
#define ADVANCED_NOTE_0 55
#define ADVANCED_NOTE_1 57
#define ADVANCED_NOTE_2 59
#define ADVANCED_NOTE_3 60
#define ADVANCED_NOTE_4 62
#define ADVANCED_NOTE_5 64
#define ADVANCED_NOTE_6 65
#define ADVANCED_NOTE_7 67
#define ADVANCED_NOTE_8 69
#define ADVANCED_NOTE_9 71
#define ADVANCED_NOTE_PROGRAM 72
#define ADVANCED_NOTE_ENTER 74


typedef struct
{
	bool in_sustain;
	uint8_t channel;
	int16_t modulation_last;
	int16_t pitch_last;
	int16_t volume_last;
	int16_t expression_last;
	bool in_advanced;
	int16_t note_offset;
	bool in_program;
} APP_STATE_T;

APP_STATE_T _state = {
	.in_sustain = false,
	.channel = DEFAULT_CHANNEL,
	.modulation_last = 30000,
	.pitch_last = 30000,
	.volume_last = 30000,
	.expression_last = 30000,
	.in_advanced = false,
	.note_offset = 0,
	.in_program = false,
};

uint32_t _number_buffer_index = 0;
char _number_buffer[8];

// built table in excel with formula: =EXP(A1*0.03815)
// response is decent compared to linear
uint8_t VELOCITY_LOOKUP_TABLE[127] =
{
	1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	1,  1,  1,  1,  1,  1,  1,  1,  2,  2,
	2,  2,  2,  2,  2,  2,  2,  2,  3,  3,
	3,  3,  3,  3,  3,  3,  4,  4,  4,  4,
	4,  4,  5,  5,  5,  5,  6,  6,  6,  6,
	6,  7,  7,  7,  8,  8,  8,  9,  9,  9,
	10, 10, 11, 11, 11, 12, 12, 13, 13, 14,
	15, 15, 16, 16, 17, 18, 18, 19, 20, 21,
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
	32, 33, 34, 36, 37, 38, 40, 42, 43, 45,
	47, 48, 50, 52, 54, 57, 59, 61, 63, 66,
	69, 71, 74, 77, 80, 83, 86, 90, 93, 97,
	101,105,109,113,117,122,127,
};


void enable_advanced_mode(bool enable)
{
	if(enable)
	{
		_state.in_advanced = true;
		led_set(LED_TYPE_ADVANCED, true);
	}
	else
	{
		_state.in_advanced = false;
		_state.in_program = false;
		led_set(LED_TYPE_ADVANCED, false);
	}
}

void keypad_event_handler(KEYPAD_EVENT_T event, KEYPAD_KEY_T key)
{
	if(event == KEYPAD_EVENT_PRESS)
	{
		if(key == KEYPAD_KEY_ADVANCE)
		{
			enable_advanced_mode(!_state.in_advanced);
		}
		else if(key == KEYPAD_KEY_OCTAVE_PLUS)
		{
			if(_state.note_offset + 12 < 63)
				_state.note_offset += 12;

			led_set(LED_TYPE_OCTAVE_DOWN_B, _state.note_offset > 0);
			led_set(LED_TYPE_OCTAVE_UP_B, _state.note_offset < 0);
		}
		else if(key == KEYPAD_KEY_OCTAVE_MINUS)
		{
			if(_state.note_offset - 12 >= -63)
				_state.note_offset -= 12;

			led_set(LED_TYPE_OCTAVE_DOWN_B, _state.note_offset > 0);
			led_set(LED_TYPE_OCTAVE_UP_B, _state.note_offset < 0);
		}
	}
}

void advanced_mode_append_number(char number)
{
	if(_state.in_program)
	{
		_number_buffer[_number_buffer_index++] = number;
		if(strlen(_number_buffer) > 3)
		{
			enable_advanced_mode(false);
		}

		if(atoi(_number_buffer) > 127)
		{
			enable_advanced_mode(false);
		}
	}
	else
	{
		enable_advanced_mode(false);
	}
}

void keyboard_event_handler(KEYBOARD_EVENT_T event, uint8_t note, int16_t velocity)
{
	if(_state.in_advanced)
	{
		if(event == KEYBOARD_EVENT_RELEASE)
			return;

		switch(note)
		{
		case ADVANCED_NOTE_CANCEL:
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL1:
			_state.channel = 0;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL2:
			_state.channel = 1;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL3:
			_state.channel = 2;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL4:
			_state.channel = 3;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL5:
			_state.channel = 4;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL6:
			_state.channel = 5;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL7:
			_state.channel = 6;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL8:
			_state.channel = 7;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL9:
			_state.channel = 8;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL10:
			_state.channel = 9;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL11:
			_state.channel = 10;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL12:
			_state.channel = 11;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL13:
			_state.channel = 12;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL14:
			_state.channel = 13;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL15:
			_state.channel = 14;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_CHANNEL16:
			_state.channel = 15;
			enable_advanced_mode(false);
			break;
		case ADVANCED_NOTE_0:
			advanced_mode_append_number('0');
			break;
		case ADVANCED_NOTE_1:
			advanced_mode_append_number('1');
			break;
		case ADVANCED_NOTE_2:
			advanced_mode_append_number('2');
			break;
		case ADVANCED_NOTE_3:
			advanced_mode_append_number('3');
			break;
		case ADVANCED_NOTE_4:
			advanced_mode_append_number('4');
			break;
		case ADVANCED_NOTE_5:
			advanced_mode_append_number('5');
			break;
		case ADVANCED_NOTE_6:
			advanced_mode_append_number('6');
			break;
		case ADVANCED_NOTE_7:
			advanced_mode_append_number('7');
			break;
		case ADVANCED_NOTE_8:
			advanced_mode_append_number('8');
			break;
		case ADVANCED_NOTE_9:
			advanced_mode_append_number('9');
			break;
		case ADVANCED_NOTE_PROGRAM:
			_state.in_program = true;
			_number_buffer_index = 0;
			memset(_number_buffer, 0, sizeof(_number_buffer));
			break;
		case ADVANCED_NOTE_ENTER:
			if(_state.in_program)
			{
				uint8_t program = atoi(_number_buffer);
				if(program > 0)
				{
					MIDI_USB_DEVICE.program_change(_state.channel, program - 1);
					MIDI_SERIAL_DEVICE.program_change(_state.channel, program - 1);
				}
				enable_advanced_mode(false);
			}
			break;
		default:
			enable_advanced_mode(false);
			break;
		}

		return;
	}

	if(event == KEYBOARD_EVENT_PRESS)
	{
		// convert to 0-126
		velocity /= MIDI_1_0_VELOCITY_DIVIDER;
		velocity = 126 - velocity;

		if(velocity > 126)
			velocity = 126;
		if(velocity < 0)
			velocity = 0;

		// translate from linear response to...
		velocity = VELOCITY_LOOKUP_TABLE[velocity];

		MIDI_SERIAL_DEVICE.note_on(note + _state.note_offset, _state.channel, velocity);

		if(MIDI_USB_DEVICE.version == MIDI_VERSION_2_0)
		{
			// convert to 16 bit
			velocity = velocity * 512;
		}

		MIDI_USB_DEVICE.note_on(note + _state.note_offset, _state.channel, velocity);
	}
	else if(event == KEYBOARD_EVENT_RELEASE)
	{
#ifdef USE_MIDI_NOTE_OFF
		MIDI_USB_DEVICE.note_off(note + _state.note_offset, _state.channel, 0);
		MIDI_SERIAL_DEVICE.note_off(note + _state.note_offset, _state.channel, 0);
#else
		MIDI_USB_DEVICE.note_on(note + _state.note_offset, _state.channel, 0);
	    MIDI_SERIAL_DEVICE.note_on(note + _state.note_offset, _state.channel, 0);
#endif
	}
}

void monitor_input()
{
	if(input_get_sustain())
	{
		if(_state.in_sustain == false)
		{
			MIDI_USB_DEVICE.sustain(_state.channel, true);
			MIDI_SERIAL_DEVICE.sustain(_state.channel, true);
			_state.in_sustain = true;
		}
	}
	else
	{
		if(_state.in_sustain)
		{
			MIDI_USB_DEVICE.sustain(_state.channel, false);
			MIDI_SERIAL_DEVICE.sustain(_state.channel, false);
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
			MIDI_USB_DEVICE.modulation_wheel(_state.channel, modulation * 4);
			MIDI_SERIAL_DEVICE.modulation_wheel(_state.channel, modulation * 4);
		}

		// monitor pitch
		if((pitch < (ANALOG_PITCH_MIDDLE - ANALOG_RANGE)) || (pitch > (ANALOG_PITCH_MIDDLE + ANALOG_RANGE)))
		{
			MIDI_USB_DEVICE.pitch_wheel(_state.channel, (pitch - 2048) * 4);
			MIDI_SERIAL_DEVICE.pitch_wheel(_state.channel, (pitch - 2048) * 4);
			last_pitch_normal = false;

			if(pitch < (ANALOG_PITCH_MIDDLE - ANALOG_RANGE))
				led_set(LED_TYPE_OCTAVE_UP_A, true);
			else
				led_set(LED_TYPE_OCTAVE_DOWN_A, true);
		}
		else
		{
			if(last_pitch_normal == false)
			{
				MIDI_USB_DEVICE.pitch_wheel(_state.channel, 0);
				MIDI_SERIAL_DEVICE.pitch_wheel(_state.channel, 0);
				led_set(LED_TYPE_OCTAVE_DOWN_A, false);
				led_set(LED_TYPE_OCTAVE_UP_A, false);
			}

			last_pitch_normal = true;
		}

		// monitor volume
		if((volume < (_state.volume_last - ANALOG_RANGE)) || (volume > (_state.volume_last + ANALOG_RANGE)))
		{
			_state.volume_last = volume;
			MIDI_USB_DEVICE.volume(_state.channel, volume * 4);
			MIDI_SERIAL_DEVICE.volume(_state.channel, volume * 4);
		}

		// todo: monitor expression

		analog_sample_tick += ANALOG_SAMPLE_RATE_MS;
	}
}

void sense_task()
{
#ifdef USE_MIDI_SENSE
	static uint32_t sense_tick = 0;

	if(sense_tick + MIDI_SENSE_RATE_MS < get_ms())
	{
		MIDI_USB_DEVICE.sense();
		MIDI_SERIAL_DEVICE.sense();

		sense_tick += MIDI_SENSE_RATE_MS;
	}

#endif
}

void main_app()
{
	keyboard_register_callback(keyboard_event_handler);
	keypad_register_callback(keypad_event_handler);

	// blink the midi version
#if MIDI_VERSION == 1
	for(uint32_t i=0; i<1; i++)
#else
	for(uint32_t i=0; i<2; i++)
#endif
	{
		led_set(LED_TYPE_ADVANCED, true);
		wait_ms(200);
		led_set(LED_TYPE_ADVANCED, false);
		wait_ms(200);
	}

	// can you say "super loop"?
	while(1)
	{
		keyboard_task();
		MIDI_USB_DEVICE.task();
		MIDI_SERIAL_DEVICE.task();
		monitor_input();
		monitor_analog();
		keypad_task();
		sense_task();
	}
}
