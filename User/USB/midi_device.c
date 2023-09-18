/*
 * midi_device.c
 *
 *  Created on: Sep 18, 2023
 *      Author: matt
 */

#include "midi_device.h"

extern void midi_1_0_init();
extern void midi_1_0_note_on(uint8_t note, uint8_t channel, uint16_t velocity);
extern void midi_1_0_note_off(uint8_t note, uint8_t channel, uint16_t velocity);
extern void midi_1_0_pitch_wheel(uint8_t channel, int32_t pitch);
extern void midi_1_0_modulation_wheel(uint8_t channel, uint16_t modulation);
extern void midi_1_0_volume(uint8_t channel, uint16_t volume);
extern void midi_1_0_sense();
extern void midi_1_0_sustain(uint8_t channel, bool on);
extern void midi_1_0_task();

USBD_MIDI_DEVICE_T MIDI_DEVICE;

void midi_device_init()
{
	MIDI_DEVICE.init = midi_1_0_init;
	MIDI_DEVICE.note_on = midi_1_0_note_on;
	MIDI_DEVICE.note_off = midi_1_0_note_off;
	MIDI_DEVICE.pitch_wheel = midi_1_0_pitch_wheel;
	MIDI_DEVICE.modulation_wheel = midi_1_0_modulation_wheel;
	MIDI_DEVICE.volume = midi_1_0_volume;
	MIDI_DEVICE.sense = midi_1_0_sense;
	MIDI_DEVICE.sustain = midi_1_0_sustain;
	MIDI_DEVICE.task = midi_1_0_task;
	MIDI_DEVICE.version = MIDI_VERSION_1_0;

	MIDI_DEVICE.init();
}
