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
#define MIDI_VELOCITY_LOOKUP_DIVIDER 15 // fit slowest into 1024 positions
#define MIDI_1_0_VELOCITY_DIVIDER (KEYBOARD_SLOWEST_VELOCITY / 127)
#define MIDI_2_0_VELOCITY_MULTIPLIER (65536 / KEYBOARD_SLOWEST_VELOCITY)
//#define USE_MIDI_SENSE	// comment out to stop sense
#define MIDI_SENSE_RATE_MS 1000
//#define USE_MIDI_NOTE_OFF	// comment out to use note on with velocity 0 instead

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
#define ADVANCED_NOTE_PROGRAM 30
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
extern bool IS_MIDI_2_0;

// 1024 position velocity lookup table
const uint16_t VELOCITY_LOOKUP_TABLE[1024] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4,
	4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6,
	6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 9, 9, 9,
	9, 9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12,
	12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16,
	16, 16, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20,
	21, 21, 21, 21, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 25, 25,
	25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30,
	31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 36, 36,
	36, 37, 37, 37, 38, 38, 39, 39, 39, 40, 40, 41, 41, 41, 42, 42,
	43, 43, 43, 44, 44, 45, 45, 46, 46, 46, 47, 47, 48, 48, 49, 49,
	49, 50, 50, 51, 51, 52, 52, 53, 53, 53, 54, 54, 55, 55, 56, 56,
	57, 57, 58, 58, 59, 59, 60, 60, 60, 61, 61, 62, 62, 63, 63, 64,
	64, 65, 65, 66, 66, 67, 67, 68, 69, 69, 70, 70, 71, 71, 72, 72,
	73, 73, 74, 74, 75, 75, 76, 76, 77, 78, 78, 79, 79, 80, 80, 81,
	81, 82, 83, 83, 84, 84, 85, 85, 86, 87, 87, 88, 88, 89, 90, 90,
	91, 91, 92, 93, 93, 94, 94, 95, 96, 96, 97, 97, 98, 99, 99, 100,
	101, 101, 102, 102, 103, 104, 104, 105, 106, 106, 107, 108, 108, 109, 109, 110,
	111, 111, 112, 113, 113, 114, 115, 115, 116, 117, 117, 118, 119, 120, 120, 121,
	122, 122, 123, 124, 124, 125, 126, 126, 127, 128, 129, 129, 130, 131, 131, 132,
	133, 134, 134, 135, 136, 136, 137, 138, 139, 139, 140, 141, 142, 142, 143, 144,
	145, 145, 146, 147, 148, 148, 149, 150, 151, 151, 152, 153, 154, 155, 155, 156,
	157, 158, 158, 159, 160, 161, 162, 162, 163, 164, 165, 166, 166, 167, 168, 169,
	170, 170, 171, 172, 173, 174, 175, 175, 176, 177, 178, 179, 180, 180, 181, 182,
	183, 184, 185, 185, 186, 187, 188, 189, 190, 191, 191, 192, 193, 194, 195, 196,
	197, 198, 198, 199, 200, 201, 202, 203, 204, 205, 206, 206, 207, 208, 209, 210,
	211, 212, 213, 214, 215, 216, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225,
	226, 227, 228, 229, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240,
	241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256,
	257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272,
	273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 286, 287, 288, 289,
	290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 301, 302, 303, 304, 305, 306,
	307, 308, 309, 310, 311, 313, 314, 315, 316, 317, 318, 319, 320, 321, 323, 324,
	325, 326, 327, 328, 329, 330, 332, 333, 334, 335, 336, 337, 338, 340, 341, 342,
	343, 344, 345, 347, 348, 349, 350, 351, 352, 354, 355, 356, 357, 358, 359, 361,
	362, 363, 364, 365, 367, 368, 369, 370, 371, 373, 374, 375, 376, 377, 379, 380,
	381, 382, 384, 385, 386, 387, 388, 390, 391, 392, 393, 395, 396, 397, 398, 400,
	401, 402, 403, 405, 406, 407, 408, 410, 411, 412, 413, 415, 416, 417, 419, 420,
	421, 422, 424, 425, 426, 428, 429, 430, 431, 433, 434, 435, 437, 438, 439, 441,
	442, 443, 445, 446, 447, 448, 450, 451, 452, 454, 455, 456, 458, 459, 460, 462,
	463, 464, 466, 467, 469, 470, 471, 473, 474, 475, 477, 478, 479, 481, 482, 484,
	485, 486, 488, 489, 490, 492, 493, 495, 496, 497, 499, 500, 502, 503, 504, 506,
	507, 509, 510, 511, 513, 514, 516, 517, 518, 520, 521, 523, 524, 526, 527, 528,
	530, 531, 533, 534, 536, 537, 539, 540, 541, 543, 544, 546, 547, 549, 550, 552,
	553, 555, 556, 558, 559, 561, 562, 564, 565, 566, 568, 569, 571, 572, 574, 575,
	577, 578, 580, 581, 583, 584, 586, 587, 589, 591, 592, 594, 595, 597, 598, 600,
	601, 603, 604, 606, 607, 609, 610, 612, 614, 615, 617, 618, 620, 621, 623, 624,
	626, 628, 629, 631, 632, 634, 635, 637, 639, 640, 642, 643, 645, 646, 648, 650,
	651, 653, 654, 656, 658, 659, 661, 662, 664, 666, 667, 669, 670, 672, 674, 675,
	677, 679, 680, 682, 683, 685, 687, 688, 690, 692, 693, 695, 697, 698, 700, 702,
	703, 705, 707, 708, 710, 712, 713, 715, 717, 718, 720, 722, 723, 725, 727, 728,
	730, 732, 733, 735, 737, 738, 740, 742, 744, 745, 747, 749, 750, 752, 754, 756,
	757, 759, 761, 762, 764, 766, 768, 769, 771, 773, 775, 776, 778, 780, 781, 783,
	785, 787, 788, 790, 792, 794, 796, 797, 799, 801, 803, 804, 806, 808, 810, 811,
	813, 815, 817, 819, 820, 822, 824, 826, 828, 829, 831, 833, 835, 837, 838, 840,
	842, 844, 846, 847, 849, 851, 853, 855, 857, 858, 860, 862, 864, 866, 868, 869,
	871, 873, 875, 877, 879, 880, 882, 884, 886, 888, 890, 892, 894, 895, 897, 899,
	901, 903, 905, 907, 909, 910, 912, 914, 916, 918, 920, 922, 924, 926, 927, 929,
	931, 933, 935, 937, 939, 941, 943, 945, 947, 948, 950, 952, 954, 956, 958, 960,
	962, 964, 966, 968, 970, 972, 974, 976, 978, 980, 981, 983, 985, 987, 989, 991,
	993, 995, 997, 999, 1001, 1003, 1005, 1007, 1009, 1011, 1013, 1015, 1017, 1019, 1021, 1023,
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

void keyboard_event_handler(KEYBOARD_EVENT_T event, uint8_t note, int32_t velocity)
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
		// the velocity represents a time, the shorter the time, the higher the velocity
		int16_t velocity_lookup_index = 1024 - (velocity / MIDI_VELOCITY_LOOKUP_DIVIDER);

		if(velocity_lookup_index >= 1024) {
			velocity_lookup_index = 1023;
		}
		else if(velocity_lookup_index < 0) {
			velocity_lookup_index = 0;
		}

		int32_t velocity_7bit = VELOCITY_LOOKUP_TABLE[velocity_lookup_index] / 8;
		int32_t velocity_16bit = VELOCITY_LOOKUP_TABLE[velocity_lookup_index] * 64;

		MIDI_SERIAL_DEVICE.note_on(note + _state.note_offset, _state.channel, velocity_7bit);

		if(IS_MIDI_2_0)
		{
			MIDI_USB_DEVICE.note_on(note + _state.note_offset, _state.channel, velocity_16bit);
		}
		else
		{
			MIDI_USB_DEVICE.note_on(note + _state.note_offset, _state.channel, velocity_7bit);
		}
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

void usb_device_message_handler(uint8_t group, uint8_t status, uint8_t byte_2, uint8_t byte_3)
{
	if(group != 1) {
		return;
	}

	if((status & 0xF0) == MIDI_NOTE_OFF){
		MIDI_SERIAL_DEVICE.note_off(byte_2, status & 0x0F, byte_3);
	}
	else if((status & 0xF0) == MIDI_NOTE_ON){
		MIDI_SERIAL_DEVICE.note_on(byte_2, status & 0x0F, byte_3);
	}
	else if((status & 0xF0) == MIDI_AFTERTOUCH){
		MIDI_SERIAL_DEVICE.aftertouch(byte_2, status & 0x0F, byte_3);
	}
	else if((status & 0xF0) == MIDI_CONTROLLER){
		MIDI_SERIAL_DEVICE.controller(byte_2, status & 0x0F, byte_3);
	}
	else if((status & 0xF0) == MIDI_PROGRAM_CHANGE){
		MIDI_SERIAL_DEVICE.program_change(status & 0x0F, byte_2);
	}
	else if((status & 0xF0) == MIDI_CHANNEL_PRESSURE){
		MIDI_SERIAL_DEVICE.channel_pressure(status & 0x0F, byte_2);
	}
	else if((status & 0xF0) == MIDI_PITCH_WHEEL){
		int32_t pitch = byte_2 | (byte_3 << 7);
		pitch -= 8192;
		MIDI_SERIAL_DEVICE.pitch_wheel(status & 0x0F, pitch);
	}
}

void main_app()
{
	keyboard_register_callback(keyboard_event_handler);
	keypad_register_callback(keypad_event_handler);
	MIDI_USB_DEVICE.register_callback(usb_device_message_handler);

	// 2 blinks
	for(uint32_t i=0; i<2; i++)
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
